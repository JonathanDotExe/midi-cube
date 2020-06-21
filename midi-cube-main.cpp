#include <iostream>
#include "midicube/audio.h"
#include "midicube/synthesis.h"
using namespace std;

double process(unsigned int channel, double time) {
	double sample = saw_wave(time, 440.0) * 0.3;
	return sample;
}

int main(int argc, char **argv) {
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
