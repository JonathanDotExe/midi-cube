/*
 * sampler.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#include "sampler.h"
#include "../../framework/util/sfz.h"
#include <iostream>
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem.hpp>
#include <regex>
#include <boost/algorithm/string.hpp>
#include "../view/SamplerView.h"

namespace pt = boost::property_tree;


//SampleSound
SampleSound::SampleSound() {

}

SampleSound::~SampleSound() {
	for (size_t i = 0; i < samples.size(); ++i) {
		delete samples[i];
	}
	samples.clear();
}

//SampleSoundStore
SampleSound* SampleSoundStore::get_sound(size_t index) {
	return samples.at(index);
}

void SampleSoundStore::load_sounds(std::string folder) {
	//Read folders
	std::regex reg(".*\\.xml");
	std::regex sfz_reg(".*\\.sfz");
	boost::filesystem::create_directory(folder);
	for (const auto& f : boost::filesystem::directory_iterator(folder)) {
		std::string file = f.path().string();
		if (boost::filesystem::is_directory(file)) {
			//Convert sfz files
			for (const auto& i : boost::filesystem::directory_iterator(file)) {
				std::string name = i.path().string();
				if (std::regex_match(name, sfz_reg)) {
					convert_sfz_to_sampler(name, file, file + "/" + i.path().stem().string() + ".xml", i.path().stem().string());
				}
			}
			//Load xml files
			for (const auto& i : boost::filesystem::directory_iterator(file)) {
				std::string name = i.path().string();
				if (std::regex_match(name, reg)) {
					SampleSound* s = load_sound(name, file, pool);
					if (s) {
						samples.push_back(s);
					}
				}
			}
		}
	}
}

std::vector<SampleSound*> SampleSoundStore::get_sounds() {
	return samples;
}

SampleSound* SampleSoundStore::get_sound(std::string name) {
	for (auto s : samples) {
		if (s->name == name) {
			return s;
		}
	}
	return nullptr;
}

SampleSoundStore::~SampleSoundStore() {
	for (auto s : samples) {
		delete s;
	}
}

//Sampler
Sampler::Sampler(PluginHost& h, Plugin& p, SampleSoundStore& s) : SoundEngine(h, p), store(s) {
	if (store.get_sounds().size() > 0) {
		set_sample(store.get_sound(0));
	}
}

inline size_t find_floor_block(double time, unsigned int sample_rate, size_t size) {
	return floor(time * sample_rate) / size;
}

inline size_t find_ceil_block(double time, unsigned int sample_rate, size_t size) {
	return ceil(time * sample_rate) / size;
}

inline size_t find_buffer_index(size_t block, size_t block_count) {
	return (block - 1) % block_count;
}

inline double velocity_amp_scale(double x, double amt) {
	return pow(x, amt * 4);
}

void Sampler::process_note_sample(const SampleInfo& info, SamplerVoice& note, size_t note_index) {
	double lsample = 0;
	double rsample = 0;
	double pan = 0;
	if (note.region && note.sample) {
		double vol = note.region->amplitude.apply_modulation(&note, this);
		double vel_amount = 0;

		double l = 0;
		double r = 0;
		pan = note.region->pan.apply_modulation(&note, this);
		double crossfade = 1;
		//Sound
		//Loop
		if (note.region->loop != NO_LOOP) {
			double loop_start_time = (double) note.sample->loop_start / note.sample->sample->sample_rate;
			double loop_crossfade_time = (double) note.sample->loop_crossfade / note.sample->sample->sample_rate;
			double loop_end_time = (double) note.sample->loop_end / note.sample->sample->sample_rate;
			double loop_duration_time = loop_end_time - loop_start_time;
			double crossfade_start_time = loop_start_time - loop_crossfade_time;
			double crossfade_end_time = loop_end_time - loop_crossfade_time;

			if (note.hit_loop) {
				//Loop again
				if (note.time >= loop_end_time) {
					note.time = loop_start_time + fmod(note.time - loop_start_time, loop_duration_time);
				}
				//Crossfade
				else if (note.time > crossfade_end_time) {
					double diff = note.time - crossfade_end_time;
					crossfade = 1 - (diff / loop_crossfade_time);
					lsample += note.sample->sample->isample(0, crossfade_start_time + diff, info.sample_rate) * (1 - crossfade);
					rsample += note.sample->sample->isample(1, crossfade_start_time + diff, info.sample_rate) * (1 - crossfade);
				}
			}
			else if (note.time >= loop_start_time) {
				note.hit_loop = true;
			}
		}

		if (note.region->trigger == TriggerType::ATTACK_TRIGGER || !note.pressed) {
			l += note.sample->sample->isample(0, note.time, info.sample_rate) * crossfade;
			r += note.sample->sample->isample(1, note.time, info.sample_rate) * crossfade;
		}

		//Filter
		if (note.region->filter.on) {
			FilterData filter { note.region->filter.filter_type };
			filter.cutoff = scale_cutoff(fmax(0, fmin(1, note.region->filter.filter_cutoff.apply_modulation(&note, this)))); //TODO optimize
			filter.resonance = note.region->filter.filter_resonance.apply_modulation(&note, this);

			if (note.region->filter.filter_kb_track) {
				double cutoff = filter.cutoff;
				//KB track
				cutoff *= 1 + ((double) note.note - 36) / 12.0 * note.region->filter.filter_kb_track;
				filter.cutoff = cutoff;
			}

			l = note.lfilter.apply(filter, l, info.time_step);
			r = note.rfilter.apply(filter, r, info.time_step);
		}

		//Env
		if (!note.region->env.sustain_entire_sample) {
			//Volume
			ADSREnvelopeData data = note.region->env.env.apply(&note, this);
			vol *= note.env.amplitude(data, info.time_step, note.pressed, host_environment.sustain);
		}
		vel_amount = note.region->env.velocity_amount.apply_modulation(&note, this); //TODO introduce curves
		double vel = velocity_amp_scale(note.velocity, vel_amount);
		vol *= vel;
		vol *= note.layer_amp  * note.region->volume.apply_modulation(&note, this);

		//Playback
		if (note.region->trigger == TriggerType::ATTACK_TRIGGER || !note.pressed) {
			double freq = note_to_freq(note.region->note + (note.note - note.region->note) * note.region->pitch_keytrack + note.region->pitch.apply_modulation(&note, this));
			note.time += freq/note_to_freq(note.region->note) * host_environment.pitch_bend * info.time_step;
			lsample += l * vol;
			rsample += r * vol;
		}
	}
	else {
		ADSREnvelopeData data = {0, 0, 0, 0};
		note.env.amplitude(data, info.time_step, note.pressed, host_environment.sustain);
	}

	//Playback
	outputs[0] += lsample * (1 - fmax(0, pan));
	outputs[1] += rsample * (1 - fmax(0, -pan));
}

bool Sampler::note_finished(const SampleInfo& info, SamplerVoice& note, size_t note_index) {
	if (note.region && note.sample) {
		return (note.region->loop == NO_LOOP && note.time > note.sample->sample->total_duration()) || (!note.region->env.sustain_entire_sample && note.env.is_finished()) || note.layer_amp <= 0.0005;
	}
	return true;
}

void Sampler::press_note(const SampleInfo& info, unsigned int note, unsigned int channel, double velocity) {
	if (this->sample) {
		for (SampleRegion* region : index.velocities[velocity * 127][note]) {
			//Check triggers
			bool trigger = true;
			for (auto t : region->control_triggers) {
				if (cc[t.first] < t.second.min_val || cc[t.first] > t.second.max_val) {
					trigger = false;
				}
			}
			if (trigger && preset_number >= region->min_preset && preset_number <= region->max_preset) {
				size_t slot = this->voice_mgr.press_note(info, note, note + host.get_transpose(), channel, velocity, 0);
				SamplerVoice& voice = this->voice_mgr.note[slot];
				voice.region = region;
				voice.layer_amp = sample->volume;
				voice.sample = &voice.region->sample;
				voice.unirand = (double) rand()/RAND_MAX;
				voice.birand = (double) rand()/RAND_MAX * 2 - 1.0;
				voice.alternate = alternate;
				alternate = !alternate;

				//TODO preload at start time
				if (voice.region && voice.region->loop == LoopType::ALWAYS_LOOP) {
					voice.time = (double) voice.sample->loop_start/voice.sample->sample->sample_rate;
				}
				else {
					voice.time = (double) voice.sample->start.apply_modulation(&voice, this)/voice.sample->sample->sample_rate;
				}
				voice.hit_loop = false;
				voice.env.reset();

				//Load sample
				LoadRequest req;
				req.sample = voice.sample->sample;
				store.pool.queue_request(req);
			}
		}
	}
}

void Sampler::release_note(const SampleInfo& info, unsigned int note, unsigned int channel, double velocity) {
	for (size_t i = 0; i < SAMPLER_POLYPHONY; ++i) {
		if (this->voice_mgr.note[i].real_note == note && this->voice_mgr.note[i].channel == channel && voice_mgr.note[i].pressed) {
			voice_mgr.note[i].pressed = false;
			voice_mgr.note[i].release_time = info.time;
			if (voice_mgr.note[i].region->trigger == TriggerType::RELEASE_TRIGGER) {
				voice_mgr.note[i].layer_amp *= pow(voice_mgr.note[i].region->release_decay, voice_mgr.note[i].release_time - voice_mgr.note[i].start_time);
			}
		}
	}
//	BaseSoundEngine<SamplerVoice, SAMPLER_POLYPHONY>::release_note(info, note);
}

std::string Sampler::get_name() {
	return "Sampler";
}

ssize_t Sampler::get_sound_index() {
	auto sounds = store.get_sounds();
	ssize_t index = std::find(sounds.begin(), sounds.end(), sample) - sounds.begin();
	if (index >= (ssize_t) sounds.size()) {
		index = -1;
	}
	return index;
}

void Sampler::set_sound_index(ssize_t index) {
	if (index < 0) {
		set_sample(nullptr);
	}
	else {
		set_sample(store.get_sound(index));
	}
}

void Sampler::save_program(PluginProgram **prog) {
	SamplerProgram* p = dynamic_cast<SamplerProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new SamplerProgram();
	}
	p->sound_name = this->sample ? this->sample->name : "";
	p->controls.clear();
	if (this->sample) {
		//Controls
		for (SampleControl& control : this->sample->controls) {
			if (control.save) {
				p->controls[control.cc] = cc[control.cc];
			}
		}
	}
	*prog = p;
}

void Sampler::apply_program(PluginProgram *prog) {
	SamplerProgram* p = dynamic_cast<SamplerProgram*>(prog);
	//Sample
	if (p) {
		set_sample(store.get_sound(p->sound_name));
		//Controls
		for (auto control : p->controls) {
			cc[control.first] = control.second;
		}
	}
	else {
		set_sample(nullptr);
	}
}

void Sampler::set_sample(SampleSound *sample) {
	this->sample = sample;
	index = {};
	set_preset_index(0);
	if (sample) {
		//Set controls
		cc = {};
		for (SampleControl control : sample->controls) {
			cc[control.cc] = control.default_value;
			index.controls.push_back(control.cc);
		}
		//Build index
		for (SampleRegion* region : sample->samples) {
			size_t max_vel = std::min((size_t) MIDI_NOTES, (size_t) region->max_velocity);
			size_t max_note = std::min((size_t) MIDI_NOTES, (size_t) region->max_note);
			for (size_t vel = region->min_velocity; vel <= max_vel; ++vel) {
				for (size_t note = region->min_note; note <= max_note; ++note) {
					index.velocities[vel][note].push_back(region);
				}
			}
		}
	}
}

Menu* Sampler::create_menu() {
	return new FunctionMenu([this]() { return new SamplerView(*this); }, nullptr);
}

Sampler::~Sampler() {
	set_sample(nullptr);
}

void load_region(pt::ptree tree, SampleRegion& region, bool load_sample, std::string folder, StreamedAudioPool& pool) {
	region.env.env.pedal_catch = true;
	region.env.velocity_amount.load(tree, "envelope.velocity_amount", region.env.velocity_amount.value);
	region.env.env.attack.load(tree, "envelope.attack", region.env.env.attack.value);
	region.env.env.decay.load(tree, "envelope.decay", region.env.env.decay.value);
	region.env.env.sustain.load(tree, "envelope.sustain", region.env.env.sustain.value);
	region.env.env.release.load(tree, "envelope.release", region.env.env.release.value);
	region.env.env.attack_hold.load(tree, "envelope.attack_hold", region.env.env.attack_hold.value);
	region.env.sustain_entire_sample = tree.get<bool>("envelope.sustain_entire_sample", region.env.sustain_entire_sample);

	region.filter.filter_cutoff.load(tree, "filter.cutoff", region.filter.filter_cutoff.value);
	region.filter.filter_kb_track = tree.get<double>("filter.kb_track", region.filter.filter_kb_track);
	region.filter.filter_kb_track_note = tree.get<unsigned int>("filter.kb_track_note", region.filter.filter_kb_track_note);
	region.filter.filter_resonance.load(tree, "filter.resonance", region.filter.filter_resonance.value);

	region.pitch_keytrack = tree.get<double>("pitch_keytrack", region.pitch_keytrack);
	region.release_decay = tree.get<double>("release_decay", region.release_decay);

	std::string type = tree.get<std::string>("filter.type", "");
	if (type == "LP_12") {
		region.filter.filter_type = FilterType::LP_12;
	}
	else if (type == "LP_24") {
		region.filter.filter_type = FilterType::LP_24;
	}
	else if (type == "HP_12") {
		region.filter.filter_type = FilterType::HP_12;
	}
	else if (type == "HP_24") {
		region.filter.filter_type = FilterType::HP_24;
	}
	else if (type == "LP_12_BP") {
		region.filter.filter_type = FilterType::LP_12_BP;
	}
	else if (type == "LP_24_BP") {
		region.filter.filter_type = FilterType::LP_24_BP;
	}
	else if (type == "LP_6") {
		region.filter.filter_type = FilterType::LP_6;
	}
	else if (type == "HP_6") {
		region.filter.filter_type = FilterType::HP_6;
	}
	else if (type == "BP_12") {
		region.filter.filter_type = FilterType::BP_12;
	}

	region.note = tree.get<double>("note", 60.0);
	region.pitch.load(tree, "pitch", region.pitch.value);
	region.min_note = tree.get<unsigned int>("min_note", region.min_note);
	region.max_note = tree.get<unsigned int>("max_note", region.max_note);
	region.min_velocity = tree.get<unsigned int>("min_velocity", region.min_velocity);
	region.max_velocity = tree.get<unsigned int>("max_velocity", region.max_velocity);
	region.min_preset = tree.get<unsigned int>("min_preset", region.min_preset);
	region.max_preset = tree.get<unsigned int>("max_preset", region.max_preset);

	//Control
	auto control_triggers = tree.get_child_optional("control_triggers");
	if (control_triggers) {
		for (auto trigger : control_triggers.get()) {
			region.control_triggers[trigger.second.get("cc", 0)] = {trigger.second.get("min_value", 0)/127.0, trigger.second.get("max_value", 127)/127.0};
		}
	}

	region.volume.load(tree, "volume", region.volume.value);
	region.amplitude.load(tree, "amplitude", region.amplitude.value);
	region.pan.load(tree, "pan", region.pan.value);

	std::string trigger = tree.get<std::string>("trigger", "");
	if (trigger == "attack") {
		region.trigger = TriggerType::ATTACK_TRIGGER;
	}
	else if (trigger == "release") {
		region.trigger = TriggerType::RELEASE_TRIGGER;
	}

	std::string file = "";
	//Sample
	file = tree.get<std::string>("sample.name", "");

	std::string loop = tree.get<std::string>("loop_type", "");
	if (loop == "no_loop") {
		region.loop = LoopType::NO_LOOP;
	}
	else if (loop == "attack_loop") {
		region.loop = LoopType::ATTACK_LOOP;
	}
	else if (loop == "always_loop") {
		region.loop = LoopType::ALWAYS_LOOP;
	}

	region.sample.start.load(tree, "sample.start", region.sample.start.value);
	region.sample.loop_start = tree.get<unsigned int>("sample.loop_start", region.sample.loop_start);
	region.sample.loop_end = tree.get<unsigned int>("sample.loop_end", region.sample.loop_end);
	region.sample.loop_crossfade = tree.get<unsigned int>("sample.loop_crossfade", region.sample.loop_crossfade);

	if (load_sample && file != "") {
		std::string filename = folder + "/" + file;
		boost::replace_all(filename, "\\", "/");
		region.sample.sample = pool.load_sample(filename);
		if (!region.sample.sample) {
			std::cerr << "Couldn't load sample file " << folder + "/" + file << std::endl;
		}
		if (region.sample.loop_start == 0 && region.sample.loop_end == 0 && region.sample.sample->looped) {
			if (region.loop == NO_LOOP) {
				region.loop = ATTACK_LOOP;
			}
			region.sample.loop_start = region.sample.sample->loop_start;
			region.sample.loop_end = region.sample.sample->loop_end;
		}
		if (region.sample.loop_end < region.sample.loop_start) {
			region.sample.loop_end = region.sample.sample->total_size;
		}
	}
}

void load_groups(pt::ptree tree, std::vector<SampleRegion*>& regions, SampleRegion region, std::string folder, StreamedAudioPool& pool) {
	for (auto c : tree) {
		SampleRegion r = region;
		pt::ptree t = c.second;

		load_region(t, r, false, folder, pool);

		//Regions
		auto rs = t.get_child_optional("regions");
		if (rs) {
			for (auto re : rs.get()) {
				SampleRegion* region = new SampleRegion(r);
				load_region(re.second, *region, true, folder, pool);
				if (region->sample.sample) {
					regions.push_back(region);
				}
				else {
					delete region;
				}
			}
		}
		//Groups
		auto groups = t.get_child_optional("groups");
		if (groups) {
			load_groups(groups.get(), regions, r, folder, pool);
		}

	}
}

void load_control(pt::ptree tree, SampleControl& control) {
	control.cc = tree.get("cc", control.cc);
	control.default_value = tree.get("default_value", control.default_value);
	control.name = tree.get("name", control.name);
	control.save = tree.get("save", control.save);
}

extern SampleSound* load_sound(std::string file, std::string folder, StreamedAudioPool& pool) {
	//Load file
	pt::ptree tree;
	SampleSound* sound = nullptr;
	try {
		pt::read_xml(file, tree);

		//Parse
		sound = new SampleSound();
		sound->name = tree.get<std::string>("sound.name", "Sound");
		sound->default_path = tree.get<std::string>("sound.default_path", ".");
		sound->volume = tree.get<double>("sound.master_volume", 1);
		SampleRegion master;
		if (tree.get_child_optional("sound")) {
			load_region(tree.get_child("sound"), master, false, folder, pool);
		}
		if (sound->default_path.rfind("/", 0) == 0) { //FIXME might be better options
			folder = sound->default_path;
		}
		else {
			folder += "/" + sound->default_path;
		}
		//Load controls
		auto controls = tree.get_child_optional("sound.controls");
		if (controls) {
			for (auto child : controls.get()) {
				SampleControl control;
				load_control(child.second, control);
				sound->controls.push_back(control);
			}
		}
		//Load presets
		auto presets = tree.get_child_optional("sound.presets");
		if (presets) {
			for (auto child : presets.get()) {
				SamplePreset preset;
				preset.name = child.second.get("name", "Default");
				preset.preset_number = child.second.get("index", 0);
				sound->presets.push_back(preset);
			}
		}
		if (sound->presets.empty()) {
			sound->presets.push_back({"Default", 0});
		}
		//Load groups
		auto groups = tree.get_child_optional("sound.groups");
		if (groups) {
			load_groups(groups.get(), sound->samples, master, folder, pool);
		}
	}
	catch (pt::xml_parser_error& e) {
		std::cerr << "Couldn't load " << folder << "/sound.xml: " << e.message() << std::endl;
		delete sound;
		sound = nullptr;
	}
	return sound;
}

extern void save_sound(std::string file) {

}

//SamplerProgram
void SamplerProgram::load(boost::property_tree::ptree tree) {
	sound_name = tree.get("sound_name", "");
	auto c = tree.get_child_optional("controls");
	if (c) {
		for (auto control : c.get()) {
			controls[control.second.get("cc", 0)] = control.second.get("value", 0.0);
		}
	}
}

boost::property_tree::ptree SamplerProgram::save() {
	boost::property_tree::ptree tree;
	tree.put("sound_name", sound_name);
	for (auto control : controls) {
		boost::property_tree::ptree child;
		child.put("cc", control.first);
		child.put("value", control.second);
		tree.add_child("controls.control", child);
	}
	return tree;
}

std::string SamplerProgram::get_plugin_name() {
	return SAMPLER_INDENTIFIER;
}

void Sampler::control_change(unsigned int control, unsigned int value) {
	SoundEngine::control_change(control, value);
	if (std::find(index.controls.begin(), index.controls.end(), control) == index.controls.end()) {
		cc[control] = value/127.0;
	}
}
