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
		View* view = new MainMenuView();
		view->init_model(&model);
		Frame frame;
		frame.change_view(view);
		//Run frame
		frame.run();
	}
	catch (AudioException& e) {
		cerr << e.what() << endl;
		return -1;
	}

	return 0;
}
