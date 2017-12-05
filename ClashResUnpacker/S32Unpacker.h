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
#include "FormatUnpacker.h"
#include "Palette.h"
#include "S32File.h"

class S32Unpacker : FormatUnpacker {
private:
	bool unpackRawS32 = false;

public:

	std::list<size_t> s32DataPositions;
	filesystem::v1::path outputPath;
	Palette palette;

	void LoadPalette() {
		std::string palettePath = GetExePath().string() + "\\palette.data";
		std::string paletteData;
		size_t paletteDataSize;

		if (!FileExists(palettePath)) {
			DebugConsole::Log("Couldn't find palette file! " + palettePath);
			return;
		}

		LoadFileData(paletteData, palettePath, paletteDataSize);

		if (paletteDataSize != 1024) {
			DebugConsole::Log("Wrong palette data size! Expected 1024, got " + paletteDataSize);
			return;
		}

		palette.SetData(paletteData);
		palette.Flip(); //Flips to little eidian.

		DebugConsole::Log("Palette loaded successfuly");
	}

	void UnpackFile(const std::string &data, filesystem::v1::path outputPath) {
		this->outputPath = outputPath;

		//Load the palette;
		LoadPalette();

		//Find all .s32 occurences.
		char dataSize = 14+4+4+4;

		std::string lookFor = ".S32"; //4 is the lookFor size.
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
				s32DataPositions.push_back(pos);
				pos = data.find(lookFor, pos + dataSize);

				isValid = false;
			}
			else {
				pos = data.find(lookFor, pos + 1);
			}
		}

		//Create S32ResDatas.
		std::vector<S32ResData> s32Datas(s32DataPositions.size());
		size_t nextPos;
		size_t curPos;

		for (int i = 0; i < s32Datas.size(); i++) {
			nextPos = s32DataPositions.front();
			s32DataPositions.pop_front();
			curPos = nextPos;
			memcpy(&s32Datas[i].name		, data.substr(curPos, 14).c_str(), 14); curPos += 14;
			memcpy(&s32Datas[i].unk_value	, data.substr(curPos, 4).c_str() , 4);  curPos += 4;
			memcpy(&s32Datas[i].offset		, data.substr(curPos, 4).c_str() , 4);  curPos += 4;
			memcpy(&s32Datas[i].size		, data.substr(curPos, 4).c_str() , 4);
		}

		stringstream ss;
		ss << s32Datas.size();
		DebugConsole::Log("Found " + ss.str() + " s32 files!");

		//Unpack S32 files.
		for (int i = 0; i < s32Datas.size(); i++) {
			UnpackS32(data, s32Datas[i]);
		}

		cout << "S32 unpack completed." << endl;
	}

	void UnpackS32(const std::string &data, const S32ResData &s32Data) {
		S32File s32File(data, s32Data);

		if (unpackRawS32) {
			const std::string &binData = s32File.GetBinData();
			string fileName = s32Data.name;
			ofstream newS32File(outputPath.string() + "//" + fileName, ios::binary);
			newS32File << binData;
		}

		string folderName(s32Data.name);
		cout << ("Converting " + folderName + "...");
		std:replace(folderName.begin(), folderName.end(), '.', '_');

		std::experimental::filesystem::v1::path s32OutputPath = outputPath.string() + "\\" + folderName;

		CreateOutputFolder(s32OutputPath.string());
		s32File.ConvertToBMP(s32OutputPath.string(), palette);

		cout << " success!" << endl;
		//std::cout << setw(14) << s32Data.name << " : " << std::hex << setw(2) << (int16_t)binData[4096] << " " << (int16_t)binData[4097] << " " << (int16_t)binData[4098] << std::dec << std::endl;
	}
};