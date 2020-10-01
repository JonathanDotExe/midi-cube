#include <iostream>
#include "midicube/midicube.h"
#include "midicube/gui/model.h"
#include "midicube/gui/gui.h"

using namespace std;


int main(int argc, char **argv) {
	MidiCube cube;
	try {
		cube.init();
		//Model
		GUIModel model;
		model.midi_cube = &cube;
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
