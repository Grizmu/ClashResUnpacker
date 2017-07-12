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

struct InfostreamResData {
	char name[14];
	int32_t unk_value;
	int32_t offset;
	int32_t size;
};

class InfostreamUnpacker {

public:
	std::list<size_t> infostreamDataPositions;
	filesystem::v1::path outputPath;

	void UnpackFile(const std::string &data, filesystem::v1::path outputPath) {
		this->outputPath = outputPath;

		//Find all .wav occurences.
		char dataSize = 14+4+4+4;

		std::string lookFor = ".IS"; //4 is the lookFor size.
		bool isValid = false;

		size_t pos = data.find(lookFor, 0);
		while (pos != std::string::npos)
		{			
			for (int i = 3; i < 15; i++) {
				if (data[pos + i] == 0x01) {
					isValid = true;
					pos = pos - (14 - i); //Move the position to the resData start.
					break;
				}
			}

			if (isValid) {
				infostreamDataPositions.push_back(pos);
				pos = data.find(lookFor, pos + dataSize);

				isValid = false;
			}
			else {
				pos = data.find(lookFor, pos + 1);
			}
		}

		//Create InfostreamResDatas.
		std::vector<InfostreamResData> isDatas(infostreamDataPositions.size());
		size_t nextPos;
		size_t curPos;

		for (int i = 0; i < isDatas.size(); i++) {
			nextPos = infostreamDataPositions.front();
			infostreamDataPositions.pop_front();
			curPos = nextPos;
			memcpy(&isDatas[i].name		, data.substr(curPos, 14).c_str(), 14); curPos += 14;
			memcpy(&isDatas[i].unk_value	, data.substr(curPos, 4).c_str() , 4);  curPos += 4;
			memcpy(&isDatas[i].offset		, data.substr(curPos, 4).c_str() , 4);  curPos += 4;
			memcpy(&isDatas[i].size		, data.substr(curPos, 4).c_str() , 4);
		}

		stringstream ss;
		ss << isDatas.size();
		DebugConsole::Log("Found " + ss.str() + " infostream files!");

		//Unpack wav files.
		for (int i = 0; i < isDatas.size(); i++) {
			UnpackIs(i, data, isDatas[i]);
		}

		cout << "Infostream unpack completed." << endl;
	}

	void UnpackIs(int id, const std::string &data, const InfostreamResData &isData) {

		string isName(isData.name);
		cout << ("Extracting " + isName + "...");

		std::experimental::filesystem::v1::path isOutputPath = outputPath.string() + "\\" + isName;

		ofstream newIsFile(isOutputPath.string(), ios::binary);

		newIsFile << data.substr(isData.offset, isData.size);
		newIsFile.flush();
		newIsFile.close();

		cout << " success!" << endl;
		

	}
};