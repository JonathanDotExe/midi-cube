#include <iostream>

#include "midicube/gui/gui.h"
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
		ViewController* view = new MainMenuView(ptr.cube);
		Frame frame("MIDICube - universal MIDI and synthesis workstation", 1024, 600);
		//Run frame
		frame.run(view);
	}
	catch (AudioException& e) {
		cerr << e.what() << endl;
		return -1;
	}

	return 0;
}
