#include <iostream>
#include "midicube/audio.h"
#include "midicube/synthesis.h"
#include "midicube/midi.h"
#include "midicube/device.h"
using namespace std;

double process(unsigned int channel, double time) {
	double sample = square_wave(time, 440.0) * 0.3;
	return sample;
}

void midi_callback(double deltatime, MidiMessage& msg, void* arg) {
	cout << "Callback" << endl;
	cout << msg.to_string() << endl;
}

int main(int argc, char **argv) {
	/*cout << "Printing MIDI messages from device 0!" << endl;
	MidiInput* input;
	PortInputDevice* device = nullptr;
	try {
		input = new MidiInput();
		std::string name = "PORT";
		if (input->available_ports() > 1) {
			name = input->port_name(1);
			cout << "Using port " << name << endl;
			input->open(0);
			device = new PortInputDevice(input, name);
			device->set_midi_callback(&midi_callback, nullptr);
		}
	}
	catch (MidiException& e) {
		cerr << e.what() << endl;
	}

	getchar();

	delete device;*/

	cout << "Playing a cool square wave sound!" << endl;

	AudioHandler handler;
	try {
		handler.set_sample_callback(process);
		handler.init();
		cout << "Press any key to exit!" << endl;
		getchar();
	}
	catch (AudioException& e) {
		cerr << e.what() << endl;
	}

	return 0;
}
