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

struct MapResData {
	char name[14];
	int32_t unk_value;
	int32_t offset;
	int32_t size;
};

class MapUnpacker {
public:

	std::list<size_t> mapDataPositions;
	filesystem::v1::path outputPath;

	void findDataPositions(std::string lookFor, const std::string &data) {
		//Find all .pcx occurences.
		char dataSize = 14 + 4 + 4 + 4;

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
				mapDataPositions.push_back(pos);
				pos = data.find(lookFor, pos + dataSize);

				isValid = false;
			}
			else {
				pos = data.find(lookFor, pos + 1);
			}
		}
	}
	void UnpackFile(const std::string &data, filesystem::v1::path outputPath) {
		this->outputPath = outputPath;
		
		findDataPositions(".MAP", data);
		findDataPositions(".MAB", data);

		//Create S32ResDatas.
		std::vector<MapResData> mapDatas(mapDataPositions.size());
		size_t nextPos;
		size_t curPos;

		for (int i = 0; i < mapDatas.size(); i++) {
			nextPos = mapDataPositions.front();
			mapDataPositions.pop_front();
			curPos = nextPos;
			memcpy(&mapDatas[i].name		, data.substr(curPos, 14).c_str(), 14); curPos += 14;
			memcpy(&mapDatas[i].unk_value	, data.substr(curPos, 4).c_str() , 4);  curPos += 4;
			memcpy(&mapDatas[i].offset		, data.substr(curPos, 4).c_str() , 4);  curPos += 4;
			memcpy(&mapDatas[i].size		, data.substr(curPos, 4).c_str() , 4);
		}

		stringstream ss;
		ss << mapDatas.size();
		DebugConsole::Log("Found " + ss.str() + " map/mab files!");

		//Unpack S32 files.
		for (int i = 0; i < mapDatas.size(); i++) {
			UnpackPcx(data, mapDatas[i]);
		}

		cout << "Map unpack completed." << endl;
	}

	void UnpackPcx(const std::string &data, const MapResData &mapData) {

		string mapName(mapData.name);
		cout << ("Extracting " + mapName + "...");

		std::experimental::filesystem::v1::path pcxOutputPath = outputPath.string() + "\\" + mapName;

		ofstream newPcxFile(pcxOutputPath.string(), ios::binary);

		newPcxFile << data.substr(mapData.offset, mapData.size);
		newPcxFile.flush();
		newPcxFile.close();

		cout << "Success!" << endl;
	}
};