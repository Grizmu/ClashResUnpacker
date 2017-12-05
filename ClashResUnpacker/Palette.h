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