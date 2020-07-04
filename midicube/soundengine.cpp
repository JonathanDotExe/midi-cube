/*
 * soundengine.cpp
 *
 *  Created on: 28 Jun 2020
 *      Author: jojo
 */

#include "soundengine.h"
#include "synthesis.h"
#include <iostream>
#include <cmath>

//PresetSynth
PresetSynth::PresetSynth() {
	detune = note_to_freq_transpose(0.1);
	ndetune = note_to_freq_transpose(-0.1);
	vibrato = 0;
}

void PresetSynth::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info, TriggeredNote& note) {
	double sample = 0.0;
	double freq = note.freq;
	double t = info.time + note.phase_shift;
	sample += saw_wave(t, freq);
	sample += saw_wave(t, freq * detune);
	sample += saw_wave(t, freq * ndetune);

	if (vibrato) {
		note.phase_shift += info.time_step * (note_to_freq_transpose(SYNTH_VIBRATO_DETUNE * sine_wave(info.time, SYNTH_VIBRATO_RATE) * vibrato) - 1);
	}

	double amp = env.amplitude(info.time, note);
	sample *= 0.1 * amp;

	for (size_t i = 0; i < channels.size() ; ++i) {
		channels[i] += sample;
	}
}

bool PresetSynth::note_finished(SampleInfo& info, TriggeredNote& note) {
	return !note.pressed && note.release_time + env.release < info.time;
}

void PresetSynth::control_change(unsigned int control, unsigned int value) {
	if (control == 1) {
		vibrato = value/127.0;
	}
}

std::string PresetSynth::get_name() {
	return "Preset Synth";
}

//SampleDrums
SampleDrums::SampleDrums () {
	//Create Test drumkit for testing purposes
	//TODO create universal drumkit loader
	drumkit = new SampleDrumKit();
	drumkit->notes[44] = {};
	read_wav(drumkit->notes[44], "./data/drumkits/test/ride.wav");
	drumkit->notes[45] = {};
	read_wav(drumkit->notes[45], "./data/drumkits/test/snare.wav");
	drumkit->notes[46] = {};
	read_wav(drumkit->notes[46], "./data/drumkits/test/hi_hat.wav");
	drumkit->notes[47] = {};
	read_wav(drumkit->notes[47], "./data/drumkits/test/bass.wav");
	drumkit->notes[48] = {};
	read_wav(drumkit->notes[48], "./data/drumkits/test/percussion1.wav");
	drumkit->notes[49] = {};
	read_wav(drumkit->notes[49], "./data/drumkits/test/crash1.wav");
	drumkit->notes[50] = {};
	read_wav(drumkit->notes[50], "./data/drumkits/test/tom1.wav");
	drumkit->notes[51] = {};
	read_wav(drumkit->notes[51], "./data/drumkits/test/tom3.wav");
}

void SampleDrums::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note) {
	if (drumkit->notes.find(note.note) != drumkit->notes.end()) {
		WAVAudio& audio = drumkit->notes[note.note];
		for (size_t i = 0; i < channels.size(); ++i) {
			channels[i] = audio.sample(i, info.time - note.start_time, info.sample_rate);
		}
	}
}

bool SampleDrums::note_finished(SampleInfo& info, TriggeredNote& note) {
	if (drumkit->notes.find(note.note) != drumkit->notes.end()) {
		return info.time - note.start_time > (double) drumkit->notes[note.note].duration(info.sample_rate);
	}
	return true;
}

std::string SampleDrums::get_name() {
	return "Sample Drums";
}

SampleDrums::~SampleDrums() {
	delete drumkit;
	drumkit = nullptr;
}



//B3Organ
B3Organ::B3Organ() {
	drawbar_harmonics = { 0.5, 0.5 * 3, 1, 2, 3, 4, 5, 6, 8 };
}

static inline unsigned int sound_delay(double rotation, double radius, unsigned int sample_rate) {
	/*double dst =
			rotation >= 0 ?
					(SPEAKER_RADIUS - radius * rotation) :
					(SPEAKER_RADIUS + radius * rotation + radius * 2);
	return round(dst / SOUND_SPEED * sample_rate);*/
	return (1 + rotation) * radius / SOUND_SPEED * sample_rate;
}

void B3Organ::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info, TriggeredNote& note) {
	double horn_sample = 0;
	double bass_sample = 0;

	double time = info.time + note.phase_shift;

	/*double horn_time = phase_mul;
	double bass_time = phase_mul;

	//Rotary speaker
	if (data.rotary) {
		double horn_speed = data.rotary_fast ? ROTARY_HORN_FAST_FREQUENCY : ROTARY_HORN_SLOW_FREQUENCY;
		double bass_speed = data.rotary_fast ? ROTARY_BASS_FAST_FREQUENCY : ROTARY_BASS_SLOW_FREQUENCY;

		horn_time -= (double) sound_delay(sine_wave(info.time, horn_speed), HORN_RADIUS, info.sample_rate)/info.sample_rate;
		bass_time -= (double) sound_delay(sine_wave(info.time, bass_speed), BASS_RADIUS, info.sample_rate)/info.sample_rate;
	}*/
	//Organ sound
	for (size_t i = 0; i < data.drawbars.size(); ++i) {
		double f = note.freq * drawbar_harmonics[i];
		while (f > 5593) {
			f /= 2.0;
		}
		if (f > ROTARY_CUTOFF) {
			horn_sample += data.drawbars[i] / 8.0 * sine_wave(time, f) / data.drawbars.size();
		} else {
			bass_sample += data.drawbars[i] / 8.0 * sine_wave(time, f) / data.drawbars.size();
		}
	}
	double sample = 0;

	//Rotary speaker
	if (data.rotary) {
		double horn_speed = data.rotary_fast ? ROTARY_HORN_FAST_FREQUENCY : ROTARY_HORN_SLOW_FREQUENCY;
		double bass_speed = data.rotary_fast ? ROTARY_BASS_FAST_FREQUENCY : ROTARY_BASS_SLOW_FREQUENCY;

		//Horn
		double horn_rot = sine_wave(info.time, horn_speed);
		unsigned int lhorn_delay = sound_delay(horn_rot, HORN_RADIUS, info.sample_rate);
		unsigned int rhorn_delay = sound_delay(-horn_rot, HORN_RADIUS, info.sample_rate);
		//Bass
		double bass_rot = sine_wave(info.time, bass_speed);
		unsigned int lbass_delay = sound_delay(bass_rot, BASS_RADIUS, info.sample_rate);
		unsigned int rbass_delay = sound_delay(-bass_rot, BASS_RADIUS, info.sample_rate);

		//Process
		left_horn_del.add_sample(horn_sample * (0.5 + horn_rot * 0.5), lhorn_delay);
		left_bass_del.add_sample(bass_sample * (0.5 + bass_rot * 0.5), lbass_delay);
		right_horn_del.add_sample(horn_sample * (0.5 - horn_rot * 0.5), rhorn_delay);
		right_bass_del.add_sample(bass_sample * (0.5 - bass_rot * 0.5), rbass_delay);
	} else {
		sample += horn_sample + bass_sample;
		sample *= 0.3;
		for (size_t i = 0; i < channels.size() ; ++i) {
			channels[i] += sample;
		}
	}
	/*sample += horn_sample + bass_sample;
	sample *= 0.3;
	for (size_t i = 0; i < channels.size() ; ++i) {
		channels[i] += sample;
	}*/
}

void B3Organ::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info) {
	double left = (left_horn_del.process() + left_bass_del.process()) * 0.3;
	double right = (right_horn_del.process() + right_bass_del.process()) * 0.3;

	for (size_t i = 0; i < channels.size(); ++i) {
		if (i % 2 == 0) {
			channels[i] += left + right * 0.7;
		}
		else {
			channels[i] += right + left * 0.7;
		}
	}
}

void B3Organ::control_change(unsigned int control, unsigned int value) {
	//Drawbars
	for (size_t i = 0; i < data.drawbar_ccs.size(); ++i) {
		if (data.drawbar_ccs[i] == control) {
			data.drawbars[i] = round((double) value/127 * 8);
		}
	}
	//Rotary
	if (control == data.rotary_cc) {
		data.rotary = value > 0;
	}
	if (control == data.rotary_speed_cc) {
		data.rotary_fast = value > 0;
	}
}

std::string B3Organ::get_name() {
	return "B3 Organ";
}

//SoundEngineDevice
SoundEngineDevice::SoundEngineDevice(std::string identifier) {
	this->identifier = identifier;
	engine = new SampleDrums();
	//Init notes
	for (size_t i = 0; i < note.size(); ++i) {
		note[i].start_time = -1024;
		note[i].release_time = -1024;
	}
}

std::string SoundEngineDevice::get_identifier() {
	return identifier;
}

void SoundEngineDevice::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info) {
	for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
		if (!engine->note_finished(info, note[i])) {
			engine->process_note_sample(channels, info, note[i]);
			note[i].phase_shift += pitch_bend * info.time_step;
		}
	}
	engine->process_sample(channels, info);
}

size_t SoundEngineDevice::next_freq_slot() {
	for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
		if (!note[i].pressed) {
			return i;
		}
	}
	//TODO return longest used slot
	return 0;
}

void SoundEngineDevice::send(MidiMessage &message) {
	//Note on
	if (message.get_message_type() == MessageType::NOTE_ON) {
		size_t slot = next_freq_slot();
		note[slot].freq = note_to_freq(message.get_note());
		note[slot].velocity = message.get_velocity()/127.0;
		note[slot].note = message.get_note();
		note[slot].pressed = true;
		note[slot].start_time = handler->sample_info().time;
		note[slot].release_time = 0;
		note[slot].phase_shift = 0;
	}
	//Note off
	else if (message.get_message_type() == MessageType::NOTE_OFF) {
		double f = note_to_freq(message.get_note());
		for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
			if (note[i].freq == f) {
				note[i].pressed = false;
				note[i].release_time = handler->sample_info().time;
			}
		}
	}
	//Control change
	else if (message.get_message_type() == MessageType::CONTROL_CHANGE) {
		engine->control_change(message.get_control(), message.get_value());
	}
	//Pitch bend
	else if (message.get_message_type() == MessageType::PITCH_BEND) {
		double pitch = (message.get_pitch_bend()/8192.0 - 1.0) * 2;
		pitch_bend = note_to_freq_transpose(pitch) - 1;
	}
}

SoundEngineDevice::~SoundEngineDevice() {
	delete engine;
	engine = nullptr;
}
