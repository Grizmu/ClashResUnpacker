#include <iostream>

#include "Utils.h"
#include "ClashResUnpacker.h"

//#include <Magick++.h>
//using namespace Magick;

//_____________________________________________________________________________
int main(int argc, char ** argv) {
	//InitializeMagick(*argv);

	ClashResUnpacker unpacker;
	unpacker.ShowSplash();

	DebugConsole::Log("Press any key to begin unpacking...");
	getchar();

	unpacker.Unpack();

	getchar();

	return 0;
}