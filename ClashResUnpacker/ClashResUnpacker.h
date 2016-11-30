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
	filesystem::v1::path exePath;
	filesystem::v1::path outputPath;

	vector<Unpacker_sp> unpackers;
public:

	ClashResUnpacker() {
		InitPaths();

		DebugConsole::LogDebug("Exe path: " + exePath.string());

		//Init unpackers

		unpackers = vector<Unpacker_sp>(2);
		unpackers[0] = UnpackerFactory::CreateUnpacker(Unpacker::UnpackerType::UP_MAXIMUM);
		unpackers[1] = UnpackerFactory::CreateUnpacker(Unpacker::UnpackerType::UP_MUSIC);
	}

	void Unpack() {
		unpackers[0]->Unpack(exePath.string() + "//MAXIMUM.RES",outputPath.string());
		unpackers[1]->Unpack(exePath.string() + "//MUSIC.RES",	outputPath.string());
	}

	void ShowSplash() {
		cout << "Clash Res Unpacker v0.15 by Griz" << endl << endl;
		cout << "Supported files:" << endl;
		cout << "MAXIMUM.RES - 50%" << endl;
		//cout << "NORMAL.RES  - 0%" << endl;
		//cout << "MINIMUM.RES - 0%" << endl;
		//cout << "GFX3.RES    - 0%" << endl;
		//cout << "IS.RES      - 0%" << endl;
		//cout << "MAPS.RES    - 0%" << endl;
		cout << "MUSIC.RES   - 100%" << endl;
		//cout << "INFOANG.RES - 0%" << endl;
		//cout << "INFOPOL.RES - 0%" << endl;
	}
private:

	void InitPaths() {
		exePath = GetExePath();
		SetOutputPath("//output");
	}

	void SetOutputPath(string folderName) {
		outputPath = filesystem::v1::path(exePath.string() + folderName);

		//Check output directory
		bool outputExists = filesystem::v1::exists(outputPath);

		if (outputExists) {
			//All is ok
			DebugConsole::Log("Found output directory.");
		}
		else {
			//Create output directory
			filesystem::v1::create_directory(outputPath);
			DebugConsole::Log("Created output directory.");
		}
	}
};
