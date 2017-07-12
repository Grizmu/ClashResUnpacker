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
#include <string>
#include <list>
#include <vector>
#include "Utils.h"
#include <iomanip>
#include <algorithm>
#include <sstream>

using namespace std;

struct WavResData {
	char name[14];
	int32_t unk_value;
	int32_t offset;
	int32_t size;
};

class WavUnpacker {

public:
	std::list<size_t> wavDataPositions;
	filesystem::v1::path outputPath;
	bool shouldNumberFiles = true;

	void UnpackFile(const std::string &data, filesystem::v1::path outputPath, bool shouldNumberFiles = true) {\
		this->shouldNumberFiles = shouldNumberFiles;
		this->outputPath = outputPath;

		//Find all .wav occurences.
		char dataSize = 14+4+4+4;

		std::string lookFor = ".WAV"; //4 is the lookFor size.
		bool isValid = false;

		size_t pos = data.find(lookFor, 0);
		while (pos != std::string::npos)
		{			
			for (int i = 4; i < 14; i++) {
				if (data[pos + i] == 0x01) {
					isValid = true;
					pos = pos - (14 - i); //Move the position to the resData start.
					break;
				}
			}

			if (isValid) {
				wavDataPositions.push_back(pos);
				pos = data.find(lookFor, pos + dataSize);

				isValid = false;
			}
			else {
				pos = data.find(lookFor, pos + 1);
			}
		}

		//Create S32ResDatas.
		std::vector<WavResData> wavDatas(wavDataPositions.size());
		size_t nextPos;
		size_t curPos;

		for (int i = 0; i < wavDatas.size(); i++) {
			nextPos = wavDataPositions.front();
			wavDataPositions.pop_front();
			curPos = nextPos;
			memcpy(&wavDatas[i].name		, data.substr(curPos, 14).c_str(), 14); curPos += 14;
			memcpy(&wavDatas[i].unk_value	, data.substr(curPos, 4).c_str() , 4);  curPos += 4;
			memcpy(&wavDatas[i].offset		, data.substr(curPos, 4).c_str() , 4);  curPos += 4;
			memcpy(&wavDatas[i].size		, data.substr(curPos, 4).c_str() , 4);
		}

		stringstream ss;
		ss << wavDatas.size();
		DebugConsole::Log("Found " + ss.str() + " wav files!");

		//Unpack wav files.
		for (int i = 0; i < wavDatas.size(); i++) {
			UnpackWav(i, data, wavDatas[i]);
		}

		cout << "wav unpack completed." << endl;
	}

	void UnpackWav(int id, const std::string &data, const WavResData &wavData) {
		string wavName(wavData.name);
		cout << ("Extracting " + wavName + "...");

		std::experimental::filesystem::v1::path wavOutputPath;

		if (shouldNumberFiles) {
			wavOutputPath = outputPath.string() + "\\" + to_string(id) + "_" + wavName;
		}
		else {
			wavOutputPath = outputPath.string() + "\\" + wavName;
		}

		ofstream newWavFile(wavOutputPath.string(), ios::binary);

		if ((unsigned char) data[wavData.offset + 20] == 0x13 && (unsigned char) data[wavData.offset + 21] == 0x80) {
			newWavFile << data.substr(wavData.offset, 20) << (char)0x01 << (char)0x00 << data.substr(wavData.offset + 22, wavData.size - 22);

			newWavFile.flush();
			newWavFile.close();

			cout << " unknown compression type 0x1380! Forced PCM." << endl;
		}
		else {
			newWavFile << data.substr(wavData.offset, wavData.size);
			newWavFile.flush();
			newWavFile.close();

			cout << " success!" << endl;
		}

	}
};