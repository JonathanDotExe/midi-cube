#include "midicube/workstation/midicube.h"
#include <iostream>

#include "midicube/workstation/view/SoundEngineView.h"
#include "midicube/framework/gui/frame.h"

using namespace std;

struct MidiCubePtr {
	MidiCubeWorkstation* cube = nullptr;
	~MidiCubePtr() {
		delete cube;
	}
};

int main(int argc, char **argv) {
	//Create MIDICube
	MidiCubePtr ptr;
	try {
		ptr.cube = new MidiCubeWorkstation();
		ptr.cube->init();
		//View
		load_resources();
		Frame frame(1024, 600, "MIDICube - universal MIDI and synthesis workstation", ptr.cube->get_config().screen_sleep);
		ptr.cube->get_menu_handler().init(&frame, ptr.cube);
		//Run frame
		frame.run(VIEW_MENU(new SoundEngineView(*ptr.cube), &ptr));
	}
	catch (AudioException& e) {
		cerr << e.what() << endl;
		return -1;
	}

	return 0;
}
