#pragma once
#include "Utils.h"
#include "Unpacker.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
#include "S32Unpacker.h"

using namespace std;
using namespace std::experimental;

class NormalUnpacker : public Unpacker {
private:
	string pcxFileName = "resource";
	filesystem::v1::path outputPath;

public:
	
	NormalUnpacker():
	Unpacker(UnpackerType::UP_NORMAL){

	}

	virtual void Unpack(std::string file, std::string outputDir) {
		if (FileExists(file)) {
			DebugConsole::Log("Unpacking " + file);
		}
		else {
			DebugConsole::Log("File " + file + " not found!");
			return;
		}

		outputPath = CreateOutputFolder(outputDir + "\\normal");

		string data;
		size_t size;

		LoadFileData(data, file, size);

		S32Unpacker s32Unpacker;

		s32Unpacker.UnpackFile(data, outputPath);

		cout << "S32 unpack completed" << endl;
	}
};