#include <iostream>

#include "midicube/gui/view/SoundEngineView.h"
#include "midicube/midicube.h"

using namespace std;

struct MidiCubePtr {
	MidiCube* cube;
	~MidiCubePtr() {
		delete cube;
	}
};

int main(int argc, char **argv) {
	//Parse args
	int device = -1;
	if (argc > 1) {
		device = atoi(argv[1]);
		cout << device << endl;
	}
	//Create MIDICube
	MidiCubePtr ptr;
	try {
		ptr.cube = new MidiCube();
		ptr.cube->init(device);
		load_resources();
		//View
		Frame frame(*ptr.cube, 1024, 600, "MIDICube - universal MIDI and synthesis workstation");
		//Run frame
		frame.run(new SoundEngineView());
	}
	catch (AudioException& e) {
		cerr << e.what() << endl;
		return -1;
	}

	return 0;
}
