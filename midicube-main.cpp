#include <iostream>

#include "midicube/gui/engine/core.h"
#include "midicube/midicube.h"

using namespace std;

struct MidiCubePtr {
	MidiCube* cube;
	~MidiCubePtr() {
		delete cube;
	}
};

int main(int argc, char **argv) {
	MidiCubePtr ptr;
	try {
		ptr.cube = new MidiCube();
		ptr.cube->init();
		//View
		Frame frame(1024, 600, "MIDICube - universal MIDI and synthesis workstation");
		//Run frame
		frame.run();
	}
	catch (AudioException& e) {
		cerr << e.what() << endl;
		return -1;
	}

	return 0;
}
