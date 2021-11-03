#include <iostream>

#include "midicube/workstation/view/SoundEngineView.h"
#include "midicube/workstation/midicube.h"

using namespace std;

struct MidiCubePtr {
	MidiCube* cube = nullptr;
	~MidiCubePtr() {
		delete cube;
	}
};

int main(int argc, char **argv) {
	//Parse args
	int out_device = -1;
	int in_device = -1;
	bool screen_sleep = true;
	if (argc > 1) {
		out_device = atoi(argv[1]);
		cout << out_device << endl;
	}
	if (argc > 2) {
		in_device = atoi(argv[2]);
		cout << in_device << endl;
	}
	if (argc > 3) {
		screen_sleep = atoi(argv[3]);
		cout << screen_sleep << endl;
	}
	//Create MIDICube
	MidiCubePtr ptr;
	try {
		//View
		Frame frame(1024, 600, "MIDICube - universal MIDI and synthesis workstation", , screen_sleep);

		ptr.cube = new MidiCube([&frame](void* source, void* prop) {
			frame.propterty_change(source, prop);
		});
		ptr.cube->init(out_device, in_device);
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
