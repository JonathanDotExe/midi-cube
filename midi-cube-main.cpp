#include <iostream>
#include <jack/jack.h>
using namespace std;

int main(int argc, char **argv) {
	cout << "A simple jack test!";
	jack_options_t options = JackNullOption;
	jack_status_t status;
	jack_client_t* client = NULL;

	client = jack_client_open("MIDICube", options, &status, NULL);
	if (client == NULL) {
		cout << "Failed to connect to jack server!" << endl;
	}

	jack_client_close(client);

	return 0;
}
