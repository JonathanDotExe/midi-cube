#include <iostream>

#include "midicube/gui/model.h"
#include "midicube/gui/gui.h"
#include "midicube/midicube.h"

using namespace std;

static MidiCube cube;

int main(int argc, char **argv) {
	try {
		cube.init();
		//Model

		//View
		ViewController* view = new MainMenuView();
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
