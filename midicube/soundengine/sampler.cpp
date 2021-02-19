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
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;


//SampleSound
SampleSound::SampleSound() {

}

SampleZone* SampleSound::get_sample(double freq, double velocity) {
	//Find regions
	SampleZone* zone = nullptr;
	const size_t velocity_size = samples.size();
	if (velocity_size >= 1) {
		size_t i = 0;
		for (; i < velocity_size; ++i) {
			if (velocity > samples[i]->max_velocity) {
				break;
			}
		}
		SampleVelocityLayer* layer = samples[std::max((ssize_t) 0, (ssize_t) (i - 1))];
		const size_t zones_size = layer->zones.size();
		if (zones_size >= 1) {
			size_t j = 0;
			for (; j < velocity_size; ++j) {
				if (freq > layer->zones[j]->max_freq) {
					break;
				}
			}
			zone = layer->zones[std::max((ssize_t) 0, (ssize_t) (j - 1))];
		}
	}
	return zone;
}

SampleSound::~SampleSound() {
	for (size_t i = 0; i < samples.size(); ++i) {
		delete samples[i];
	}
	samples.clear();
}

//SampleSoundStore
SampleSound* SampleSoundStore::get_sound(std::string name) {
	return samples[name];
}

void SampleSoundStore::load_sounds(std::string folder) {
	//TODO
}

void SampleSoundStore::load_sound(std::string folder) {
	//TODO
}

SampleSoundStore::~SampleSoundStore() {
	for (std::pair<std::string, SampleSound*> s : samples) {
		delete s.second;
	}
}

//Sampler
Sampler::Sampler() {
	sample = load_sound("./data/samples/piano");
}

void Sampler::process_note_sample(double& lsample, double& rsample, SampleInfo& info, SamplerVoice& note, KeyboardEnvironment& env, size_t note_index) {
	if (note.env.is_finished()) {
		note.env.reset();
		note.zone = this->sample->get_sample(note.freq, note.velocity);
	}

	if (note.zone) {
		double vol = note.env.amplitude(note.zone->env, info.time_step, note.pressed, env.sustain);
		double time = (info.time - note.start_time) * note.zone->freq/note.freq * env.pitch_bend;
		lsample += note.zone->sample.isample(0, time, info.sample_rate) * vol;
		rsample += note.zone->sample.isample(1, time, info.sample_rate) * vol;
	}
}

bool Sampler::note_finished(SampleInfo& info, SamplerVoice& note, KeyboardEnvironment& env, size_t note_index) {
	return !note.pressed && note.env.is_finished();
}

std::string Sampler::get_name() {
	return "Sampler";
}

Sampler::~Sampler() {
	delete sample;
	sample = nullptr;
}


extern SampleSound* load_sound(std::string folder) {
	//Load file
	pt::ptree tree;
	SampleSound* sound = nullptr;
	try {
		pt::read_json(folder + "/sound.json", tree);

		//Envelope
		ADSREnvelopeData env;
		env.attack = tree.get<double>("attack", 0.0005);
		env.decay = tree.get<double>("decay", 0.0);
		env.sustain = tree.get<double>("sustain", 1);
		env.release = tree.get<double>("release", 0.003);
		//Parse
		sound = new SampleSound();
		sound->set_envelope(env);
		try {
			//Load sounds
			for (auto r : tree.get_child("regions")) {
				SampleZone* region = new SampleZone();
				try {
					region->freq = note_to_freq(r.second.get<int>("note", 0));
					std::string file = folder + "/" + r.second.get<std::string>("file");
					if (!read_audio_file(region->sample, file)) {
						throw std::runtime_error("Couldn't load sample file " + file);
					}
				}
				catch (std::exception& e) {
					delete region;
					region = nullptr;
					throw e;
				}
				sound->push_sample(region);
			}
		}
		catch (std::exception& e) {
			delete sound;
			sound = nullptr;
			throw e;
		}
	}
	catch (pt::json_parser_error& e) {
		std::cerr << "Couldn't load sound.json" << std::endl;
	}
	return sound;
}

template<>
std::string get_engine_name<Sampler>() {
	return "Sampler";
}

void __fix_link_sampler_name__ () {
	get_engine_name<Sampler>();
}

