/*
 * sampler.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#include "sampler.h"
#include <iostream>
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem.hpp>

namespace pt = boost::property_tree;


SampleSoundStore global_sample_store = {};

//SampleSound
SampleSound::SampleSound() {

}

void SampleSound::get_sample(double freq, double velocity, SamplerVoice& voice, bool sustain) {
	//Find regions
	double last_vel = 0;
	double curr_vel = 0;
	SampleZone* zone = nullptr;
	const size_t velocity_size = samples.size();
	if (velocity_size >= 1) {
		size_t i = 0;
		for (; i < velocity_size; ++i) {
			curr_vel = samples[i]->max_velocity;
			if (velocity <= samples[i]->max_velocity) {
				break;
			}
			last_vel = samples[i]->max_velocity;
		}
		SampleVelocityLayer* layer = samples[std::max((ssize_t) 0, (ssize_t) i - 1)];
		const size_t zones_size = layer->zones.size();
		if (zones_size >= 1) {
			size_t j = 0;
			for (; j < zones_size; ++j) {
				if (freq <= layer->zones[j]->max_freq) {
					break;
				}
			}
			zone = layer->zones[std::max((ssize_t) 0, (ssize_t) j - 1)];
		}
	}
	//Update zone
	voice.region = zone;
	if (voice.region) {
		voice.layer_amp = (1 - voice.region->layer_velocity_amount * (1 - (velocity - last_vel)/(curr_vel - last_vel))) * volume;
		voice.sample = (sustain && voice.region->sustain_sample.sample.samples.size()) ? &voice.region->sustain_sample : &voice.region->sample;
		if (voice.region->env >= 0 && (size_t) voice.region->env < envelopes.size()) {
			voice.env_data = &envelopes[voice.region->env];
		}
		if (voice.region->filter >= 0 && (size_t) voice.region->filter < filters.size()) {
			voice.filter = &filters[voice.region->filter];
		}
	}
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
	for (const auto& f : boost::filesystem::directory_iterator(folder)) {
		std::string file = f.path().string();
		SampleSound* s = load_sound(file);
		if (s) {
			samples.push_back(s);
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
Sampler::Sampler() {
	set_sample(global_sample_store.get_sound(0));
}

void Sampler::process_note_sample(double& lsample, double& rsample, SampleInfo& info, SamplerVoice& note, KeyboardEnvironment& env, size_t note_index) {
	if (note.region && note.sample) {
		double vol = 1;
		double vel_amount = 0;

		double l = 0;
		double r = 0;
		double crossfade = 1;
		//Sound
		//Loop
		if (note.region->loop) {
			double loop_start_time = (double) note.sample->loop_start / note.sample->sample.sample_rate;
			double loop_duration_time = (double) note.sample->loop_duration / note.sample->sample.sample_rate;
			double loop_crossfade_time = (double) note.sample->loop_crossfade / note.sample->sample.sample_rate;
			double loop_end_time = loop_start_time + loop_duration_time;
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
					l += note.sample->sample.isample(0, crossfade_start_time + diff, info.sample_rate) * (1 - crossfade);
					r += note.sample->sample.isample(1, crossfade_start_time + diff, info.sample_rate) * (1 - crossfade);
				}
			}
			else if (note.time >= loop_start_time) {
				note.hit_loop = true;
			}
		}

		l += note.sample->sample.isample(0, note.time, info.sample_rate) * crossfade;
		r += note.sample->sample.isample(1, note.time, info.sample_rate) * crossfade;

		//Filter
		if (note.filter) {
			FilterData filter { note.filter->filter_type };
			filter.cutoff = scale_cutoff(fmax(0, fmin(1, note.filter->filter_cutoff + note.filter->filter_velocity_amount * note.velocity))); //TODO optimize
			filter.resonance = note.filter->filter_resonance;

			if (note.filter->filter_kb_track) {
				double cutoff = filter.cutoff;
				//KB track
				cutoff *= 1 + ((double) note.note - 36) / 12.0 * note.filter->filter_kb_track;
				filter.cutoff = cutoff;
			}

			l = note.lfilter.apply(filter, l, info.time_step);
			r = note.rfilter.apply(filter, r, info.time_step);
		}
		//Env
		if (note.env_data) {
			if (!note.env_data->sustain_entire_sample) {
				//Volume
				vol = note.env.amplitude(note.env_data->env, info.time_step, note.pressed, env.sustain);
				vel_amount += note.env_data->amp_velocity_amount;
			}
		}
		vol *= vel_amount * (note.velocity - 1) + 1;
		vol *= note.layer_amp;

		note.time += note.freq/note.region->freq * env.pitch_bend * info.time_step;
		//Playback
		lsample += l * vol;
		rsample += r * vol;
	}
	else {
		ADSREnvelopeData data = {0, 0, 0, 0};
		note.env.amplitude(data, info.time_step, note.pressed, env.sustain);
	}
}

bool Sampler::note_finished(SampleInfo& info, SamplerVoice& note, KeyboardEnvironment& env, size_t note_index) {
	if (note.region && note.sample) {
		return note.env_data->sustain_entire_sample ? note.time < note.sample->sample.duration() : note.env.is_finished();
	}
	return true;
}

void Sampler::press_note(SampleInfo& info, unsigned int real_note, unsigned int note, double velocity) {
	if (this->sample) {
		for (SampleRegion* region : index.velocities[velocity][note]) {
			size_t slot = this->note.press_note(info, real_note, note, velocity);
			SamplerVoice& voice = this->note.note[slot];
			voice.region = region;
			voice.layer_amp = (1 - voice.region->layer_velocity_amount * (1 - (velocity - voice.region->min_velocity)/(voice.region->max_velocity - voice.region->min_velocity))) * sample->volume;
			voice.sample = (sustain && voice.region->sustain_sample.sample.samples.size()) ? &voice.region->sustain_sample : &voice.region->sample;

			if (voice.region && voice.region->loop == LoopType::ALWAYS_LOOP) {
				voice.time = voice.sample->loop_start;
			}
			else {
				voice.time = 0;
			}
			voice.hit_loop = false;
			voice.env.reset();
		}
	}
}

void Sampler::release_note(SampleInfo& info, unsigned int note) {
	BaseSoundEngine<SamplerVoice, SAMPLER_POLYPHONY>::release_note(info, note);
}

std::string Sampler::get_name() {
	return "Sampler";
}

ssize_t Sampler::get_sound_index() {
	auto sounds = global_sample_store.get_sounds();
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
		set_sample(global_sample_store.get_sound(index));
	}
}

void Sampler::save_program(EngineProgram **prog) {
	std::cout << "Save" << std::endl;
	SamplerProgram* p = dynamic_cast<SamplerProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new SamplerProgram();
	}
	p->sound_name = this->sample ? this->sample->name : "";
	*prog = p;
}

void Sampler::apply_program(EngineProgram *prog) {
	SamplerProgram* p = dynamic_cast<SamplerProgram*>(prog);
	//Create new
	if (p) {
		set_sample(global_sample_store.get_sound(p->sound_name));
	}
	else {
		set_sample(nullptr);
	}
}

void Sampler::set_sample(SampleSound *sample) {
	this->sample = sample;
	index = {};
	for (SampleRegion* region : sample->samples) {
		size_t max_vel = std::min((size_t) MIDI_NOTES, (size_t) region->max_velocity);
		size_t max_note = std::min((size_t) MIDI_NOTES, (size_t) region->max_note);
		for (size_t vel = region->min_velocity; vel < max_vel; ++vel) {
			for (size_t note = max_vel; note < max_note; ++note) {
				index.velocities[vel][note].push_back(region);
			}
		}
	}
}

Sampler::~Sampler() {
	set_sample(nullptr);
}


extern SampleSound* load_sound(std::string folder) {
	//Load file
	pt::ptree tree;
	SampleSound* sound = nullptr;
	try {
		pt::read_xml(folder + "/sound.xml", tree);

		//Parse
		sound = new SampleSound();
		sound->name = tree.get<std::string>("sound.name", "Sound");
		sound->volume = tree.get<double>("sound.volume", 1);
		//Load Envelopes
		for (auto e : tree.get_child("sound.envelopes")) {
			SampleEnvelope env = {};
			env.amp_velocity_amount = e.second.get<double>("velocity_amount", 0);
			env.env.attack = e.second.get<double>("attack", 0);
			env.env.decay = e.second.get<double>("decay", 0);
			env.env.sustain = e.second.get<double>("sustain", 1);
			env.env.release = e.second.get<double>("release", 0);
			env.sustain_entire_sample = e.second.get<bool>("sustain_entire_sample", false);

			sound->envelopes.push_back(env);
		}
		//Load Filters
		for (auto f : tree.get_child("sound.filters")) {
			SampleFilter filter = {};
			filter.filter_cutoff = f.second.get<double>("cutoff", 1);
			filter.filter_kb_track = f.second.get<double>("kb_track", 0);
			filter.filter_kb_track_note = f.second.get<unsigned int>("kb_track_note", 36);
			filter.filter_resonance = f.second.get<double>("resonance", 0);
			filter.filter_velocity_amount = f.second.get<double>("velocity_amount", 0);

			std::string type = f.second.get<std::string>("type");
			if (type == "LP_12") {
				filter.filter_type = FilterType::LP_12;
			}
			else if (type == "LP_24") {
				filter.filter_type = FilterType::LP_24;
			}
			else if (type == "HP_12") {
				filter.filter_type = FilterType::HP_12;
			}
			else if (type == "HP_24") {
				filter.filter_type = FilterType::HP_24;
			}
			else if (type == "BP_12") {
				filter.filter_type = FilterType::LP_12;
			}
			else if (type == "BP_24") {
				filter.filter_type = FilterType::LP_24;
			}

			sound->filters.push_back(filter);
		}
		//Load velocity layers
		for (auto r : tree.get_child("sound.velocity_layers")) {
			SampleVelocityLayer* layer = new SampleVelocityLayer();
			layer->max_velocity = r.second.get<double>("velocity", 1.0);
			//Load zones
			for (auto z : r.second.get_child("zones")) {
				SampleZone* zone = new SampleZone();
				zone->freq = note_to_freq(z.second.get<double>("note", 60.0));
				zone->layer_velocity_amount = z.second.get<double>("layer_velocity_amount", 0);
				zone->max_freq = note_to_freq(z.second.get<double>("max_note", 127.0));
				zone->env = z.second.get<double>("envelope", 0);
				zone->filter = z.second.get<double>("filter", 0);
				//Loop
				std::string type = z.second.get<std::string>("loop_type", "NO_LOOP");
				if (type == "NO_LOOP") {
					zone->loop = LoopType::NO_LOOP;
				}
				else if (type == "ATTACK_LOOP") {
					zone->loop = LoopType::ATTACK_LOOP;
				}
				else if (type == "ALWAYS_LOOP") {
					zone->loop = LoopType::ALWAYS_LOOP;
				}
				std::string file;
				//Sample
				if (z.second.get_child_optional("sample.name")) {
					file = folder + "/" + z.second.get<std::string>("sample.name", "");
					zone->sample.loop_start = z.second.get<unsigned int>("sample.loop_start", 0);
					zone->sample.loop_duration = z.second.get<unsigned int>("sample.loop_duration", 0);
					zone->sample.loop_crossfade = z.second.get<unsigned int>("sample.loop_crossfade", 0);
				}
				else {
					file = folder + "/" + z.second.get<std::string>("sample", "");
				}
				if (!read_audio_file(zone->sample.sample, file)) {
					std::cerr << "Couldn't load sample file " << file << std::endl;
				}
				std::string sfile;
				if (z.second.get_child_optional("sustain_sample.name")) {
					sfile = folder + "/" + z.second.get<std::string>("sustain_sample.name", "");
					zone->sustain_sample.loop_start = z.second.get<unsigned int>("sustain_sample.loop_start", 0);
					zone->sustain_sample.loop_duration = z.second.get<unsigned int>("sustain_sample.loop_duration", 0);
					zone->sustain_sample.loop_crossfade = z.second.get<unsigned int>("sustain_sample.loop_crossfade", 0);
				}
				else {
					sfile = folder + "/" + z.second.get<std::string>("sustain_sample", "");
				}
				if (sfile != folder + "/") {
					if (!read_audio_file(zone->sustain_sample.sample, sfile)) {
						std::cerr << "Couldn't load sample file " << sfile << std::endl;
					}
				}
				layer->zones.push_back(zone);
			}
			sound->samples.push_back(layer);
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

template<>
std::string get_engine_name<Sampler>() {
	return "Sampler";
}

void __fix_link_sampler_name__ () {
	get_engine_name<Sampler>();
}

//SamplerProgram
void SamplerProgram::load(boost::property_tree::ptree tree) {
	sound_name = tree.get("sound_name", "");
}

boost::property_tree::ptree SamplerProgram::save() {
	boost::property_tree::ptree tree;
	tree.put("sound_name", sound_name);
	return tree;
}
