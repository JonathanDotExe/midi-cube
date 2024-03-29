/*
 * arpeggiator.cpp
 *
 *  Created on: Oct 14, 2021
 *      Author: jojo
 */

#include "arpeggiator.h"
#include "../view/ArpeggiatorView.h"

//Arpeggiator
void ArpeggiatorInstance::apply(const SampleInfo& info, const Metronome& master, bool sustain) {
	//Clean sustained notes
	if (!preset.hold && preset.sustain && !sustain) {
		for (size_t i = 0; i < this->note.note.size(); ++i) {
			if (this->note.note[i].state != VOICE_PRESSED) {
				this->note.note[i].state = VOICE_RELEASED;
			}
		}
	}
	//Reset if no keys are pressed
	bool released = true;
	for (size_t i = 0; i < this->note.note.size() && released; ++i) {
		released = this->note.note[i].state == VOICE_INACTIVE;
	}
	if (!restart) {
		restart = released;
	}
	if (restart) {
		second = false;
		//Keyboard sync
		if (preset.kb_sync) {
			metronome.init(info.sample_time);
		}
	}
	//Pattern
	if ((preset.master_sync ? master.is_beat(info.sample_time, info.sample_rate, preset.value) : metronome.is_beat(info.sample_time, info.sample_rate, preset.value))) {
		int next_note = 128;
		long int next_index = -1;

		unsigned int lowest_note = 128;
		long int lowest_index = -1;
		int highest_note = -1;
		long int highest_index = -1;

		switch (preset.pattern) {
		case ArpeggiatorPattern::ARP_UP:
			for (size_t i = 0; i < this->note.note.size(); ++i) {
				if (this->note.note[i].state != VOICE_INACTIVE) {
					if (this->note.note[i].note < lowest_note + preset.play_duplicates) {
						lowest_note = this->note.note[i].note;
						lowest_index = i;
					}
					//Find next highest note
					for (unsigned int octave = 0; octave < preset.octaves; ++octave) {
						int n = this->note.note[i].note + octave * 12;
						if ((n < next_note || (n == next_note && i < (size_t) next_index)) && (n > (int) curr_note || (n == (int) curr_note && preset.play_duplicates && i > note_index))) {
							next_note = n;
							next_index = i;
							break;
						}
					}
				}
			}
			//Restart from lowest
			if (restart || next_index < 0) {
				next_note = lowest_note;
				next_index = lowest_index;
			}
			break;
		case ArpeggiatorPattern::ARP_DOWN:
			next_note = -1;
			for (size_t i = 0; i < this->note.note.size(); ++i) {
				if (this->note.note[i].state != VOICE_INACTIVE) {
					if ((int) (this->note.note[i].note + (preset.octaves - 1) * 12) > highest_note - preset.play_duplicates) {
						highest_note = this->note.note[i].note  + (preset.octaves - 1) * 12;
						highest_index = i;
					}
					//Find next lowest note
					for (unsigned int o = 0; o < preset.octaves; ++o) {
						int octave = preset.octaves - o - 1;
						int n = this->note.note[i].note + octave * 12;
						if ((n > next_note || (n == next_note && i < (size_t) next_index)) && (n < (int) curr_note  || (n == (int) curr_note && preset.play_duplicates && i > note_index))) {
							next_note = n;
							next_index = i;
							break;
						}
					}
				}
			}
			//Restart from highest
			if (restart || next_index < 0) {
				next_note = highest_note;
				next_index = highest_index;
			}
			break;
		case ArpeggiatorPattern::ARP_UP_DOWN:
			if (second) {
				//Down
				next_note = -1;
				for (size_t i = 0; i < this->note.note.size(); ++i) {
					if (this->note.note[i].state != VOICE_INACTIVE) {
						if (this->note.note[i].note < lowest_note + preset.play_duplicates) {
							lowest_note = this->note.note[i].note;
							lowest_index = i;
						}
						//Find next lowest note
						for (unsigned int o = 0; o < preset.octaves; ++o) {
							int octave = preset.octaves - o - 1;
							int n = this->note.note[i].note + octave * 12;
							if ((n > next_note || (n == next_note && i < (size_t) next_index)) && (n < (int) curr_note  || (n == (int) curr_note && preset.play_duplicates && i > note_index))) {
								next_note = n;
								next_index = i;
								break;
							}
						}
					}
				}
				//Restart from lowest
				if (next_index < 0 || (!preset.repeat_edges && next_index == lowest_index && next_note == (int) lowest_note)) {
					next_note = lowest_note;
					next_index = lowest_index;
					second = !second;
				}
			}
			else {
				//Up
				for (size_t i = 0; i < this->note.note.size(); ++i) {
					if (this->note.note[i].state != VOICE_INACTIVE) {
						if ((int) (this->note.note[i].note + (preset.octaves - 1) * 12) > highest_note - preset.play_duplicates) {
							highest_note = this->note.note[i].note  + (preset.octaves - 1) * 12;
							highest_index = i;
						}
						if (this->note.note[i].note < lowest_note + preset.play_duplicates) {
							lowest_note = this->note.note[i].note;
							lowest_index = i;
						}
						//Find next highest note
						for (unsigned int octave = 0; octave < preset.octaves; ++octave) {
							int n = this->note.note[i].note + octave * 12;
							if ((n < next_note || (n == next_note && i < (size_t) next_index)) && (n > (int) curr_note || (n == (int) curr_note && preset.play_duplicates && i > note_index))) {
								next_note = n;
								next_index = i;
								break;
							}
						}
					}
				}
				//Restart from highest
				if (next_index < 0 || (!preset.repeat_edges && next_index == highest_index && next_note == highest_note)) {
					next_note = highest_note;
					next_index = highest_index;
					second = !second;
				}
			}
			//Restart from lowest
			if (restart) {
				next_note = lowest_note;
				next_index = lowest_index;
				second = false;
			}
			break;
		case ArpeggiatorPattern::ARP_DOWN_UP:
			if (second) {
				//Up
				for (size_t i = 0; i < this->note.note.size(); ++i) {
					if (this->note.note[i].state != VOICE_INACTIVE) {
						if ((int) (this->note.note[i].note + (preset.octaves - 1) * 12) > highest_note - preset.play_duplicates) {
							highest_note = this->note.note[i].note  + (preset.octaves - 1) * 12;
							highest_index = i;
						}
						//Find next highest note
						for (unsigned int octave = 0; octave < preset.octaves; ++octave) {
							int n = this->note.note[i].note + octave * 12;
							if ((n < next_note || (n == next_note && i < (size_t) next_index)) && (n > (int) curr_note || (n == (int) curr_note && preset.play_duplicates && i > note_index))) {
								next_note = n;
								next_index = i;
								break;
							}
						}
					}
				}
				//Restart from highest
				if (next_index < 0 || (!preset.repeat_edges && next_index == highest_index && next_note == highest_note)) {
					next_note = highest_note;
					next_index = highest_index;
					second = !second;
				}
			}
			else {
				//Down
				next_note = -1;
				for (size_t i = 0; i < this->note.note.size(); ++i) {
					if (this->note.note[i].state != VOICE_INACTIVE) {
						if (this->note.note[i].note < lowest_note + preset.play_duplicates) {
							lowest_note = this->note.note[i].note;
							lowest_index = i;
						}
						if ((int) (this->note.note[i].note + (preset.octaves - 1) * 12) > highest_note - preset.play_duplicates) {
							highest_note = this->note.note[i].note  + (preset.octaves - 1) * 12;
							highest_index = i;
						}
						//Find next lowest note
						for (unsigned int o = 0; o < preset.octaves; ++o) {
							int octave = preset.octaves - o - 1;
							int n = this->note.note[i].note + octave * 12;
							if ((n > next_note || (n == next_note && i < (size_t) next_index)) && (n < (int) curr_note || (n == (int) curr_note && preset.play_duplicates && i > note_index))) {
								next_note = n;
								next_index = i;
								break;
							}
						}
					}
				}
				//Restart from lowest
				if (next_index < 0 || (!preset.repeat_edges && next_index == lowest_index && next_note == (int) lowest_note)) {
					next_note = lowest_note;
					next_index = lowest_index;
					second = !second;
				}
			}
			//Restart from highest
			if (restart) {
				next_note = highest_note;
				next_index = highest_index;
				second = false;
			}
			break;
		case ArpeggiatorPattern::ARP_RANDOM:
			//Count valid notes
			size_t count = 0;
			for (size_t i = 0; i < this->note.note.size(); ++i) {
				count += this->note.note[i].state != VOICE_INACTIVE;
			}
			if (count > 0) {
				size_t next = rand() % count;
				size_t octave = rand() % preset.octaves;
				//Find next note
				count = 0;
				for (size_t i = 0; i < this->note.note.size() && count <= next; ++i) {
					if (this->note.note[i].state != VOICE_INACTIVE) {
						next_note = this->note.note[i].note + octave * 12;
						next_index = i;
						++count;
					}
				}
			}

			break;
		}
		//Press note
		if (!released || note_change) {
			note_change = false;
			MidiMessage msg;
			msg.type = MessageType::NOTE_OFF;
			msg.set_note(curr_note);
			msg.set_velocity(0);
			this->send_midi(msg, info);
		}
		if (next_index >= 0) {
			curr_note = next_note;
			this->note_index = next_index;
			MidiMessage msg;
			msg.type = MessageType::NOTE_ON;
			msg.set_note(curr_note);
			msg.set_velocity(this->note.note[note_index].velocity * 127);
			this->send_midi(msg, info);
			restart = false;

			//Clean held notes
			if (!preset.hold && !(sustain || preset.sustain)) {
				bool released = true;
				for (size_t i = 0; i < this->note.note.size() && released; ++i) {
					released = this->note.note[i].state != VOICE_PRESSED;
				}
				if (released) {
					for (size_t i = 0; i < this->note.note.size(); ++i) {
						this->note.note[i].state = VOICE_INACTIVE;
					}
				}
				note_change = true;
			}
		}
	}
}

void ArpeggiatorInstance::press_note(const SampleInfo& info, unsigned int note, unsigned int channel, double velocity, bool sustain) {
	//Clean held notes
	if (preset.hold) {
		bool released = true;
		for (size_t i = 0; i < this->note.note.size() && released; ++i) {
			released = this->note.note[i].state != VOICE_PRESSED;
		}
		if (released) {
			for (size_t i = 0; i < this->note.note.size(); ++i) {
				this->note.note[i].state = VOICE_INACTIVE;
			}
		}
	}
	note_change = true;
	this->note.press_note(info, note, note, channel, velocity);
}

void ArpeggiatorInstance::release_note(const SampleInfo& info, unsigned int note, unsigned int channel, bool sustain) {
	this->note.release_note(info, note, channel, !preset.hold && (!sustain || !preset.sustain));
	note_change = true;
}


void ArpeggiatorProgram::load(boost::property_tree::ptree tree) {
	on = tree.get<bool>("on", false);
	bpm = tree.get<unsigned int>("bpm", 120);
	preset.pattern = static_cast<ArpeggiatorPattern>(tree.get<size_t>("pattern", 0));
	preset.octaves = tree.get<unsigned int>("octaves", 1);
	preset.value = tree.get<unsigned int>("note_value", 1);
	preset.hold = tree.get<bool>("hold", false);
	preset.kb_sync = tree.get<bool>("kb_sync", true);
	preset.repeat_edges = tree.get<bool>("repeat_edges", false);
	preset.play_duplicates = tree.get<bool>("play_duplicates", false);
	preset.master_sync = tree.get<bool>("master_sync", false);
	preset.sustain = tree.get<bool>("sustain", false);
}


std::string ArpeggiatorProgram::get_plugin_name() {
	return ARPEGGIATOR_IDENTIFIER;
}

boost::property_tree::ptree ArpeggiatorProgram::save() {
	boost::property_tree::ptree tree;
	tree.put("on", on);
	tree.put("bpm", bpm);
	tree.put("pattern", static_cast<size_t>(preset.pattern));
	tree.put("octaves", preset.octaves);
	tree.put("note_value", preset.value);
	tree.put("hold", preset.hold);
	tree.put("kb_sync", preset.kb_sync);
	tree.put("repeat_edges", preset.repeat_edges);
	tree.put("play_duplicates", preset.play_duplicates);
	tree.put("master_sync", preset.master_sync);
	tree.put("sustain", preset.sustain);
	return tree;
}

void ArpeggiatorInstance::apply_program(PluginProgram *prog) {
	ArpeggiatorProgram* p = dynamic_cast<ArpeggiatorProgram*>(prog);
	if (p) {
		preset = p->preset;
		on = p->on;
		metronome.set_bpm(p->bpm);
	}
	else {
		preset = {};
		on = true;
		metronome.set_bpm(120);
	}
}

void ArpeggiatorInstance::save_program(PluginProgram **prog) {
	ArpeggiatorProgram* p = dynamic_cast<ArpeggiatorProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new ArpeggiatorProgram();
	}
	p->preset = preset;
	p->on = on;
	p->bpm = metronome.get_bpm();
	*prog = p;
}

void ArpeggiatorInstance::process(const SampleInfo &info) {
	if (on) {
		apply(info, host_metronome, host_environment.sustain);
	}
}

ArpeggiatorInstance::ArpeggiatorInstance(PluginHost &h, Plugin &p) : PluginInstance(h, p){
}

void ArpeggiatorInstance::recieve_midi(const MidiMessage &message, const SampleInfo &info) {
	if (on) {
		switch (message.type) {
		case MessageType::NOTE_ON:
			press_note(info, message.note(), message.velocity()/127.0, message.channel, host_environment.sustain);
			break;
		case MessageType::NOTE_OFF:
			release_note(info, message.note(), message.channel, host_environment.sustain);
			/* no break */
		default:
			this->send_midi(message, info);
			break;
		}
	}
	else {
		this->send_midi(message, info);
	}
}

ArpeggiatorInstance::~ArpeggiatorInstance() {
}

PluginProgram* ArpeggiatorPlugin::create_program() {
	return new ArpeggiatorProgram();
}

PluginInstance* ArpeggiatorPlugin::create(PluginHost *host) {
	return new ArpeggiatorInstance(*host, *this);
}

Menu* ArpeggiatorInstance::create_menu() {
	return new FunctionMenu([this](){ return new ArpeggiatorView(*this); }, nullptr);
}
