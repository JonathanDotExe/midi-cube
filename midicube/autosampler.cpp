/*
 * autosampler.cpp
 *
 *  Created on: 1 Mar 2021
 *      Author: jojo
 */

#include "autosampler.h"
#include "synthesis.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <regex>
#include <fstream>
#include <streambuf>

namespace pt = boost::property_tree;

#define MAX_QUIET_TIME 2.0
#define PIANO_HIGH_NOTE_START 89

void AutoSampler::request_params() {
	std::cout << "Welcome to the Auto-Sample tool for MIDICube!" << std::endl;
	//MIDI device
	const unsigned int ports = rtmidi.getPortCount();
	for (unsigned int i = 0; i < ports; ++i) {
		std::cout << i << ": " << rtmidi.getPortName(i) << std::endl;
	}
	std::cout << "Which MIDI device should be sampled?" << std::endl;
	std::cin >> midi_device;

	//Audio device
	const unsigned int a_ports = rtaudio.getDeviceCount();
	for (unsigned int i = 0; i < a_ports; ++i) {
		std::cout << i << ": " << rtaudio.getDeviceInfo(i).name << std::endl;
	}
	std::cout << "Which audio device should be sampled?" << std::endl;
	std::cin >> audio_device;

	//Velocities
	int vel = 0;
	while (vel >= 0) {
		std::cout << "Which velocity should be sampled? (-1 to stop)" << std::endl;
		std::cin >> vel;
		if (vel >= 0) {
			velocities.push_back(vel);
		}
	}

	//Sample sustain
	std::cout << "Do you want to record with sustain pedal down? (0/1)" << std::endl;
	std::cin >> record_sustain;

	//Notes
	unsigned int note = 0;
	unsigned int end_note = 0;
	unsigned int step = 0;
	std::cout << "Which note do you want to start with?" << std::endl;
	std::cin >> note;
	std::cout << "Which note do you want to end with?" << std::endl;
	std::cin >> end_note;
	std::cout << "Which steps should be between the notes?" << std::endl;
	std::cin >> step;
	for (; note <= end_note; note += step) {
		notes.push_back(note);
	}

	std::cout << "Where should the audio files be saved?" << std::endl;
	std::cin >> folder;

	std::cout << "What should the filename prefix be?" << std::endl;
	std::cin >> prefix;
}

int sampler_process(void* output_buffer, void* input_buffer, unsigned int buffer_size, double time, RtAudioStreamStatus status, void* arg) {
	AutoSampler* handler = (AutoSampler*) arg;
	return handler->process((double*) output_buffer, (double*) input_buffer, buffer_size, time);
}


void AutoSampler::init() {
	//Open midi
	std::cout << "Opening port " << rtmidi.getPortName(midi_device) << std::endl;
	rtmidi.openPort(midi_device);

	//Open audio
	//Set up options
	RtAudio::StreamOptions options;
	options.flags = options.flags | RTAUDIO_SCHEDULE_REALTIME;
	options.priority = 90;

	//Set up input
	RtAudio::StreamParameters input_params;
	input_params.deviceId = audio_device;
	input_params.nChannels = AUTOSAMPLER_CHANNELS;
	input_params.firstChannel = 0;

	unsigned int buffer_size = 256;

	rtaudio.openStream(nullptr, &input_params, RTAUDIO_FLOAT64, sample_rate, &buffer_size, &sampler_process, this);
	rtaudio.startStream();
	sample_rate = rtaudio.getStreamSampleRate();
	std::cout << "Opened audio stream Sample Rate: " << sample_rate << " Hz ... Buffer Size: " << buffer_size << std::endl;
}

inline int AutoSampler::process(double *output_buffer, double *input_buffer,
		unsigned int buffer_size, double time) {

	for (size_t i = 0; i < buffer_size; ++i) {
		double l = *input_buffer++;
		double r = *input_buffer++;

		bool loud = std::abs(l) > 0.0002 || std::abs(r) > 0.0002;

		//Update last signal time
		if (loud) {
			last_signal_time = sample.duration();
		}
		//Press note
		if (!pressed) {
			if (curr_note < notes.size() && curr_velocity < velocities.size()) {
				sample.clear();
				sample.channels = AUTOSAMPLER_CHANNELS;
				sample.sample_rate = sample_rate;

				//Sustain pedal
				unsigned char status = 0xB0 | ((char) channel & 0x0F);
				if (record_sustain) {
					std::vector<unsigned char> msg = {status, (unsigned char) 64, (unsigned char) 127};
					rtmidi.sendMessage(&msg);
				}

				//First note
				status = 0x90 | ((char) channel & 0x0F);
				std::vector<unsigned char> msg = {status, (unsigned char) notes.at(curr_note), (unsigned char) velocities.at(curr_velocity)};
				rtmidi.sendMessage(&msg);

				std::cout << "Sampling note " << notes.at(curr_note) << " at velocity " << velocities.at(curr_velocity) << "!" << std::endl;
				pressed = true;
			}
			else {
				running = false;
			}
		}

		if (pressed) {
			//Check if audio started
			if (!started_audio && loud) {
				std::cout << "Started recording" << std::endl;
				started_audio = true;
			}

			//Process signal
			if (started_audio) {
				sample.samples.push_back(l);
				sample.samples.push_back(r);

				//Check end
				if (last_signal_time + MAX_QUIET_TIME < sample.duration()) {
					started_audio = false;
					last_signal_time = 0;
					//Shorten sample
					while (!sample.samples.at(sample.samples.size() - 1)) {
						sample.samples.pop_back();
					}
					//Release old note
					unsigned char status = 0x80 | ((char) channel & 0x0F);
					std::vector<unsigned char> msg = {status, (unsigned char) notes.at(curr_note), 0};
					std::cout << "Finished sampling note " << notes.at(curr_note) << " at velocity " << velocities.at(curr_velocity) << "!" << std::endl;
					//Sustain pedal
					status = 0xB0 | ((char) channel & 0x0F);
					if (record_sustain) {
						std::vector<unsigned char> msg = {status, (unsigned char) 64, (unsigned char) 0};
						rtmidi.sendMessage(&msg);
					}
					//Save
					write_audio_file(sample, folder + "/" + prefix + "_" + std::to_string(notes.at(curr_note)) + "_" + std::to_string(velocities.at(curr_velocity)) + (record_sustain ? "_sustain" : "") + ".wav");

					rtmidi.sendMessage(&msg);
					pressed = false;

					//Find next note/velocity
					curr_note++;
					if (curr_note >= notes.size()) {
						curr_note = 0;
						curr_velocity++;
					}
				}
			}
		}
	}
	return 0;
}

void SampleSoundCreator::request_params() {
	std::cout << "Welcome to the sample configuration tool for MIDICube!" << std::endl;

	std::cout << "How should your sound be called?" << std::endl;
	std::cin >> sound_name;

	std::cout << "Do you want to smoothen the velocity layers (different volumes)? (0/1)" << std::endl;
	std::cin >> smoothen_layers;

	std::cout << "Do you want ignore the lowest velocity layer (only use it as reference volume)? (0/1)" << std::endl;
	std::cin >> ignore_first_layer;

	std::cout << "Do you want to normalize the audio? (0/1)" << std::endl;
	std::cin >> normalize;

	std::cout << "Where should your sound be saved?" << std::endl;
	std::cin >> path;
}

void SampleSoundCreator::generate_sound() {
	pt::ptree tree;
	//Piano algorithm
	tree.put("sound.name", sound_name);
	//ADSR
	//Piano envelopes
	{
		pt::ptree env;
		env.put("velocity_amount", 0.0);
		env.put("attack", 0.0);
		env.put("decay", 0.0);
		env.put("sustain", 1.0);
		env.put("release", 0.15);

		tree.add_child("sound.envelopes.envelope", env);
	}
	{
		pt::ptree env;
		env.put("velocity_amount", 0.0);
		env.put("sustain_entire_sample", true);

		tree.add_child("sound.envelopes.envelope", env);
	}

	tree.put("sound.filters", "");

	//Determine velocity layers and notes
	std::vector<unsigned int> notes = {};
	std::vector<unsigned int> velocities = {};
	std::regex reg(".*\\.wav");
	std::string prefix = "";
	bool sustain = false;
	double max_vol = 0;
	for (const auto& f : boost::filesystem::directory_iterator(path)) {
		if (std::regex_match(f.path().string(), reg)) {
			//Get params
			std::vector<std::string> split;
			boost::split(split, f.path().stem().string(), boost::is_any_of("_"));

			prefix = split.at(0); //FIXME
			unsigned int note = std::stoi(split.at(1));
			unsigned int velocity = std::stoi(split.at(2));

			sustain = sustain || (split.size() > 3 && split.at(3) == "sustain"); //FIXME

			//Add to list
			if (std::find(notes.begin(), notes.end(), note) == notes.end()) {
				notes.push_back(note);
			}
			if (std::find(velocities.begin(), velocities.end(), velocity) == velocities.end()) {
				velocities.push_back(velocity);
			}
		}
	}
	//Sort
	std::sort(notes.begin(), notes.end());
	std::sort(velocities.begin(), velocities.end());


	std::vector<std::vector<double>> vols = {};
	//Generate
	size_t v = 0;
	for (unsigned int velocity : velocities) {
		size_t n = 0;

		vols.push_back({});
		pt::ptree layer;
		layer.put("velocity", velocity/127.0);
		bool reached_high_part = false;
		unsigned int last_note = 0;
		for (unsigned int note : notes) {
			double velocity_amount = 0;
			//Find volume
			double vol = 0;
			double sustain_vol = 0;
			AudioSample sample;
			std::string file = path + "/" + prefix + "_" + std::to_string(note) + "_" + std::to_string(velocity) + ".wav";
			if (!read_audio_file(sample, file)) {
				std::cerr << "Couldn't load sample file " << file << std::endl;
			}
			for (double s : sample.samples) {
				if (fabs(s) > vol) {
					vol = fabs(s);
				}
			}
			//Sustain
			if (sustain) {
				file = path + "/" + prefix + "_" + std::to_string(note) + "_" + std::to_string(velocity) + ".wav";
				if (!read_audio_file(sample, file)) {
					std::cerr << "Couldn't load sample file " << file << std::endl;
				}
				for (double s : sample.samples) {
					if (fabs(s) > sustain_vol) {
						sustain_vol = fabs(s);
					}
				}

			}
			if (vol > max_vol) {
				max_vol = vol;
			}
			if (sustain_vol > max_vol) {
				max_vol = sustain_vol;
			}
			//Smooth velocity
			if (smoothen_layers) {
				//Calc velocity amount
				//TODO seperate volumes for sustain
				double last = v > 0 ? vols.at(v - 1).at(n) : 0;
				velocity_amount = fmax(1 - last/vol, 0);

				vols.at(v).push_back(vol);
			}
			//Zone
			if (v > 0 || !ignore_first_layer) {
				if (!reached_high_part) {
					if (note >= PIANO_HIGH_NOTE_START) {
						if (last_note < PIANO_HIGH_NOTE_START) {
							pt::ptree zone;
							zone.put("note", note);
							zone.put("max_note", PIANO_HIGH_NOTE_START - 1); //TODO different methods
							zone.put("envelope", 0);
							zone.put("filter", -1);
							zone.put("sample", prefix + "_" + std::to_string(note) + "_" + std::to_string(velocity) + ".wav");
							if (sustain) {
								zone.put("sustain_sample", prefix + "_" + std::to_string(note) + "_" + std::to_string(velocity) + "_sustain.wav");
							}
							layer.add_child("zones.zone", zone);
						}
						reached_high_part = true;
					}
				}

				pt::ptree zone;
				zone.put("note", note);
				zone.put("max_note", note); //TODO different methods
				zone.put("envelope", reached_high_part ? 1 : 0);
				zone.put("filter", -1);
				zone.put("layer_velocity_amount", velocity_amount);
				zone.put("sample", prefix + "_" + std::to_string(note) + "_" + std::to_string(velocity) + ".wav");
				if (sustain) {
					zone.put("sustain_sample", prefix + "_" + std::to_string(note) + "_" + std::to_string(velocity) + "_sustain.wav");
				}
				layer.add_child("zones.zone", zone);

				last_note = note;
				n++;
			}
		}
		if (v > 0 || !ignore_first_layer) {
			tree.add_child("sound.velocity_layers.velocity_layer", layer);
		}
		v++;
	}

	tree.put("sound.volume", 1/max_vol);

	//Save to file
	try {
		pt::write_xml(path + "/sound.xml", tree);
	}
	catch (pt::xml_parser_error& e) {
		std::cerr << "Couldn't save file!" << std::endl;
	}
}

void SfzSampleConverter::request_params() {
	std::cout << "Enter input file path!" << std::endl;
	std::cin >> src;
	std::cout << "Enter output file path!" << std::endl;
	std::cin >> dst;
	std::cout << "Enter the name of created instrument!" << std::endl;
	std::cin >> name;
}

static void parse_opcodes(std::unordered_map<std::string, std::string> opcodes, pt::ptree& tree) {
	//Opcodes
	for (auto opcode : opcodes) {
		if (opcode.first == "amp_veltrack") {
			tree.put("envelope.velocity_amount", std::stod(opcode.second)/100.0);
		}
		else if (opcode.first == "ampeg_attack") {
			tree.put("envelope.attack", std::stod(opcode.second));
		}
		else if (opcode.first == "ampeg_decay") {
			tree.put("envelope.decay", std::stod(opcode.second));
		}
		else if (opcode.first == "ampeg_sustain") {
			tree.put("envelope.sustain", std::stod(opcode.second));
		}
		else if (opcode.first == "ampeg_release") {
			tree.put("envelope.release", std::stod(opcode.second));
		}
		else if (opcode.first == "lokey") {
			tree.put("min_note", parse_sfz_note(opcode.second));
		}
		else if (opcode.first == "hikey") {
			tree.put("max_note", parse_sfz_note(opcode.second));
		}
		else if (opcode.first == "pitch_keycenter") {
			tree.put("note", parse_sfz_note(opcode.second));
		}
		else if (opcode.first == "lovel") {
			tree.put("min_velocity", std::stoi(opcode.second));
		}
		else if (opcode.first == "hivel") {
			tree.put("max_velocity", std::stoi(opcode.second));
		}
		else if (opcode.first == "volume") {
			tree.put("volume", db_to_amp(std::stod(opcode.second)));
		}
		else if (opcode.first == "pitch_keytrack") {
			tree.put("pitch_keytrack", std::stod(opcode.second)/100.0);
		}
		else if (opcode.first == "rt_decay") {
			tree.put("release_decay", db_to_amp(-std::stod(opcode.second)));
		}
		else if (opcode.first == "trigger") {
			tree.put("trigger", opcode.second);
			if (opcode.second != "attack") {
				tree.put("envelope.sustain_entire_sample", true);
			}
		}
		else if (opcode.first == "sample") {
			std::string path = opcode.second;
			boost::replace_all(path, "\\", "/");
			tree.put("sample.name", path);
		}
		else if (opcode.first == "loop_mode") {
			if (opcode.second == "one_shot") {
				tree.put("envelope.sustain_entire_sample", true);
				tree.put("loop_type", "no_loop");
			}
			else if (opcode.second == "no_loop") {
				tree.put("loop_type", "no_loop");
			}
			else if (opcode.second == "loop_continuous") {
				tree.put("loop_type", "attack_loop");
			}
		}
		else if (opcode.first == "loop_start") {
			tree.put("loop_start", std::stoi(opcode.second));
		}
		else if (opcode.first == "loop_end") {
			tree.put("loop_end", std::stoi(opcode.second));
		}
		else if (opcode.first == "ampeg_hold") {
			tree.put("envelope.attack_hold", std::stod(opcode.second));
		}
		//TODO tune
		else {
			std::cout << "Skipping unrecognized opcode " << opcode.first << "="
					<< opcode.second << std::endl;
		}
	}
}

void SfzSampleConverter::convert() {
	std::fstream f(src);
	std::string text;
	std::string t;

	while (getline(f, t)) {
		text += t + "\n";
	}

	SfzInstrument instrument = parser.parse(text, dst);
	std::cout << "Loaded instrument" << std::endl;
	pt::ptree tree;
	//Name
	tree.put("sound.name", name);

	//Groups
	for (SfzGroup group : instrument.groups) {
		pt::ptree g;
		parse_opcodes(group.opcodes, g);
		//Regions
		for (SfzRegion region : group.regions) {
			pt::ptree r;
			parse_opcodes(region.opcodes, r);
			g.add_child("regions.region", r);
		}
		tree.add_child("sound.groups.group", g);
	}

	//Save to file
	try {
		pt::write_xml(dst + "/sound.xml", tree);
		std::cout << "Finished converting sound!" << std::endl;
	}
	catch (pt::xml_parser_error& e) {
		std::cerr << "Couldn't save file!" << std::endl;
	}
}
