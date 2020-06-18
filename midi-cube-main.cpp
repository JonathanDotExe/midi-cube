#include <iostream>
#include "midicube/audio.h"
using namespace std;

int main(int argc, char **argv) {
	cout << "A simple jack test!" << endl;

	AudioHandler handler;
	try {
		handler.init();
		cout << "Pointer-Adresse " << &handler << endl;
	}
	catch (AudioException& e) {
		cerr << e.what() << endl;
	}

	return 0;
}
