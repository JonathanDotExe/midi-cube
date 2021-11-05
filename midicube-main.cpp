#include <iostream>

#include "midicube/workstation/view/SoundEngineView.h"
#include "midicube/framework/gui/frame.h"
#include "midicube/workstation/midicube.h"

using namespace std;

struct MidiCubePtr {
	MidiCube* cube = nullptr;
	~MidiCubePtr() {
		delete cube;
	}
};

int main(int argc, char **argv) {
	//Create MIDICube
	MidiCubePtr ptr;
	try {
		ptr.cube = new MidiCube();
		ptr.cube->init();
		//View
		Frame frame(1024, 600, "MIDICube - universal MIDI and synthesis workstation", *ptr.cube, ptr.cube->get_config().screen_sleep);
		load_resources();
		//Run frame
		frame.run(new SoundEngineView(*ptr.cube));
	}
	catch (AudioException& e) {
		cerr << e.what() << endl;
		return -1;
	}

	return 0;
}
