/*
Copyright (c) 2016-2017 Arkadiusz Kalinowski

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include "Utils.h"
#include <fstream>
#include "UnpackerFactory.h"

using namespace std::experimental;
using namespace std;

class ClashResUnpacker {
private:
	bool convertToPNG = true;
	filesystem::v1::path exePath;
	filesystem::v1::path outputPath;

	vector<Unpacker_sp> unpackers;
public:

	ClashResUnpacker(int argc, char ** argv) {
		ShowSplash();
		InitPaths();
		InitUnpackers();
	}

	void Unpack() {
		unpackers[0]->Unpack(exePath.string() + "\\MUSIC.RES",	outputPath.string());
		unpackers[1]->Unpack(exePath.string() + "\\INFOANG.RES", outputPath.string());
		unpackers[2]->Unpack(exePath.string() + "\\INFOPOL.RES", outputPath.string());
		unpackers[3]->Unpack(exePath.string() + "\\MAPS.RES", outputPath.string());
		unpackers[4]->Unpack(exePath.string() + "\\SETUP.RES", outputPath.string());
		unpackers[5]->Unpack(exePath.string() + "\\GFX3.RES", outputPath.string());
		unpackers[6]->Unpack(exePath.string() + "\\MINIMUM.RES", outputPath.string());
		unpackers[7]->Unpack(exePath.string() + "\\NORMAL.RES", outputPath.string());
		unpackers[8]->Unpack(exePath.string() + "\\MAXIMUM.RES",outputPath.string());
		unpackers[9]->Unpack(exePath.string() + "\\IS.RES", outputPath.string());

		DebugConsole::Log("Unpack completed!");
		DebugConsole::Log("Press any key to exit.");
		//unpackers[0]->Unpack(exePath.string() + "\\NORMAL.RES", outputPath.string());
	}

	void ShowSplash() {
		cout << "Clash Res Unpacker v0.84 by Griz" << endl << endl;
		cout << "Supported files:"   << endl;
		cout << "MAXIMUM.RES - 85%"  << endl;
		cout << "NORMAL.RES  - 95%"  << endl;
		cout << "MINIMUM.RES - 95%"  << endl;
		cout << "SETUP.RES   - 10%"  << endl;
		cout << "GFX3.RES    - 100%" << endl;
		cout << "IS.RES      - 100%" << endl;
		cout << "MAPS.RES    - 100%" << endl;
		cout << "MUSIC.RES   - 100%" << endl;
		cout << "INFOANG.RES - 100%" << endl;
		cout << "INFOPOL.RES - 100%" << endl;
	}
private:

	void InitPaths() {
		exePath = GetExePath();
		SetOutputPath("\\output");

		//DebugConsole::LogDebug("Exe path: " + exePath.string());
	}

	void InitUnpackers() {
		unpackers = vector<Unpacker_sp>(10);
		unpackers[0] = UnpackerFactory::CreateUnpacker(Unpacker::UnpackerType::UP_MUSIC);
		unpackers[1] = UnpackerFactory::CreateUnpacker(Unpacker::UnpackerType::UP_INFOANG);
		unpackers[2] = UnpackerFactory::CreateUnpacker(Unpacker::UnpackerType::UP_INFOPOL);
		unpackers[3] = UnpackerFactory::CreateUnpacker(Unpacker::UnpackerType::UP_MAPS);
		unpackers[4] = UnpackerFactory::CreateUnpacker(Unpacker::UnpackerType::UP_SETUP);
		unpackers[5] = UnpackerFactory::CreateUnpacker(Unpacker::UnpackerType::UP_GFX3);
		unpackers[6] = UnpackerFactory::CreateUnpacker(Unpacker::UnpackerType::UP_MINIMUM);
		unpackers[7] = UnpackerFactory::CreateUnpacker(Unpacker::UnpackerType::UP_NORMAL);
		unpackers[8] = UnpackerFactory::CreateUnpacker(Unpacker::UnpackerType::UP_MAXIMUM);
		unpackers[9] = UnpackerFactory::CreateUnpacker(Unpacker::UnpackerType::UP_IS);

		//unpackers = vector<Unpacker_sp>(1);
		//unpackers[0] = UnpackerFactory::CreateUnpacker(Unpacker::UnpackerType::UP_NORMAL);

	}

	void SetOutputPath(string folderName) {
		outputPath = filesystem::v1::path(exePath.string() + folderName);

		//Check output directory
		bool outputExists = filesystem::v1::exists(outputPath);

		if (outputExists) {
			//All is ok
			//DebugConsole::Log("Found output directory " + folderName);
		}
		else {
			//Create output directory
			filesystem::v1::create_directory(outputPath);
			//DebugConsole::Log("Created output directory " + folderName);
		}
	}
};
