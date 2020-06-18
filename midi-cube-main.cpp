#include <iostream>
#include "midicube/audio.h"
using namespace std;

int main(int argc, char **argv) {
	cout << "A simple jack test!" << endl;

	AudioHandler* handler = create_audio_handler();
	try {
		cout << "Pointer-Adresse " << handler << endl;
	}
	catch (AudioException& e) {
		cerr << e.what() << endl;
	}

	dispose_audio_handler();

	return 0;
}
