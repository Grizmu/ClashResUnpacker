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
#include "Utils.h"
#include "Unpacker.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
#include "PcxUnpacker.h"

using namespace std;
using namespace std::experimental;

class InfoPolUnpacker : public Unpacker {
private:
	string pcxFileName = "resource";
	filesystem::v1::path outputPath;

public:

	InfoPolUnpacker() :
		Unpacker(UnpackerType::UP_INFOPOL) {

	}

	virtual void Unpack(std::string file, std::string outputDir) {
		if (FileExists(file)) {
			DebugConsole::Log("Unpacking " + file);
		}
		else {
			DebugConsole::Log("File " + file + " not found!");
			return;
		}

		outputPath = CreateOutputFolder(outputDir + "\\infopol");

		string data;
		size_t size;

		LoadFileData(data, file, size);

		PcxUnpacker pcxUnpacker;
		pcxUnpacker.UnpackFile(data, outputPath);

		cout << "INFOPOL.RES unpack completed." << endl;
	}
};