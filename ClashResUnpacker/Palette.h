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
#include <iostream>

struct Palette {
	uint32_t color[256];
	bool isEidianFlipped = false;

	void Flip() {
		for (int i = 0; i < 256; i++) {
			std::reverse((char *)&color[i], (((char *)&color[i]) + 4)); //Flip to be bmp compatible - little eidian.
		}
		isEidianFlipped = !isEidianFlipped;
	}

	bool IsEidianFlipped() {
		return isEidianFlipped;
	}

	void SetData(std::string &paletteData) {
		memcpy(&color, paletteData.c_str(), sizeof(uint32_t) * 256); //Copy the palette to the variable.
	}

	void LoadDataFromFile(string palettePath) {
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

		SetData(paletteData);

		DebugConsole::Log("Palette loaded successfuly");
	}
};