#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include "Utils.h"
#include <fstream>

using namespace std::experimental;
using namespace std;

class ClashResUnpacker {
private:
	filesystem::v1::path exePath;
	filesystem::v1::path outputPath;
	string pcxFileStartString;
	const size_t pcxHeaderSize = 128;

	string pcxFileName = "resource";
	int pcxIndex = 0;
public:

	void Unpack() {
		Init();
		InitPaths();

		DebugConsole::LogDebug("Exe path: " + exePath.string());


		UnpackMaximumRes();
	}

	void ShowSplash() {
		cout << "Clash Res Unpacker v0.1 by Griz" << endl << endl;
		cout << "Supported files:" << endl;
		cout << "MAXIMUM.RES - 50%" << endl;
		//cout << "NORMAL.RES  - 0%" << endl;
		//cout << "MINIMUM.RES - 0%" << endl;
		//cout << "GFX3.RES    - 0%" << endl;
		//cout << "IS.RES      - 0%" << endl;
		//cout << "MAPS.RES    - 0%" << endl;
		//cout << "MUSIC.RES   - 0%" << endl;
		//cout << "INFOANG.RES - 0%" << endl;
		//cout << "INFOPOL.RES - 0%" << endl;
	}
private:

	void Init() {
		//Init pcxFileEndString

		const size_t len = 12;
		BYTE pcxStartBytes[len] = { 0x0A, 0x05, 0x01, 0x08, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x02, 0xDF, 0x01 };
		pcxFileStartString = string(reinterpret_cast<char const*>(pcxStartBytes), len);
	}

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
	void UnpackMaximumRes() {

		filesystem::v1::path filePath(exePath.string() + "//MAXIMUM.RES");
		bool fileExists = filesystem::v1::exists(filePath);

		if (fileExists) {
			DebugConsole::Log("Unpacking MAXIMUM.RES...");
		}
		else {
			DebugConsole::Log("File MAXIMUM.RES not found!");
			return;
		}

		/*
		std::ifstream in(filePath.string());
		ostringstream ss = std::ostringstream{};
		ss << in.rdbuf();

		string s = ss.str();
		*/

		std::ifstream in(filePath.string(), ios::binary);
		in.seekg(0, std::ios::end);
		size_t size = in.tellg();
		std::string data(size, ' ');
		in.seekg(0);
		in.read(&data[0], size);

		int startPos = 468081; //First byte of first image
		DebugConsole::Log("File size : " + to_string(size) + " bytes.");
		//DebugConsole::Log("Symbol : " + to_string(data[startPos]));

		//Find all PCX files
		vector<size_t> positions; // holds all the positions that sub occurs within str

		size_t pos = data.find(pcxFileStartString, 0);
		while (pos != string::npos)
		{
			positions.push_back(pos);
			pos = data.find(pcxFileStartString, pos + 1);
		}

		DebugConsole::Log("Found " + to_string(positions.size()) + " images!");

		//http://www.fileformat.info/format/pcx/egff.htm //Decoding info

		PCXHeader pcxHeaderData;
		const size_t pcxHeaderSize = 128;

		//Save to pcx
		for (int i = 0; i < positions.size(); i++) {
			//memcpy(&pcxHeaderData, &data[positions[i]], pcxHeaderSize); //Get the pcxHeader
			pcxHeaderData = GetPCXHeaderData(&data[positions[i]]);

			DebugConsole::Log("File no " + to_string(i) + ":");
			PrintPCXHeaderInfo(pcxHeaderData);
			ExtractPCX(data, positions[i], pcxHeaderData);
		}

		DebugConsole::Log("");
		DebugConsole::Log("Unpacking complete.");
	}

	PCXHeader GetPCXHeaderData(char *start) {
		PCXHeader pcxHeaderData;
		memcpy(&pcxHeaderData, start, pcxHeaderSize); //Get the pcxHeader
		return pcxHeaderData;
	}

	void PrintPCXHeaderInfo(const PCXHeader &pcxHeader) {
		string message = "";
		message += "Identifier:     " + to_string(pcxHeader.Identifier) + "\n";		/* PCX Id Number (Always 0x0A) */	//BYTE	
		message += "Version:        " + to_string(pcxHeader.Version) + "\n";		/* Version Number */				//BYTE	
		message += "Encoding:       " + to_string(pcxHeader.Encoding) + "\n";		/* Encoding Format */				//BYTE	
		message += "BitsPerPixel:   " + to_string(pcxHeader.BitsPerPixel) + "\n";	/* Bits per Pixel */				//BYTE	
		message += "XStart:         " + to_string(pcxHeader.XStart) + "\n";			/* Left of image */					//WORD	
		message += "YStart:         " + to_string(pcxHeader.YStart) + "\n";			/* Top of Image */					//WORD	
		message += "XEnd:           " + to_string(pcxHeader.XEnd) + "\n";			/* Right of Image */				//WORD	
		message += "YEnd:           " + to_string(pcxHeader.YEnd) + "\n";			/* Bottom of image */				//WORD	
		message += "HorzRes:        " + to_string(pcxHeader.HorzRes) + "\n";		/* Horizontal Resolution */			//WORD	
		message += "VertRes:        " + to_string(pcxHeader.VertRes) + "\n";		/* Vertical Resolution */			//WORD	
		message += "Palette[48]:    \n";											/* 16-Color EGA Palette */			//BYTE	
		string pal = string(49, '\0');
		for (int i = 0; i < 48; i++) {
			pal[i] = (char)(pcxHeader.Palette[i]);
		}
		//message += "   " + pal + "\n";

		message += "Reserved1:      " + to_string(pcxHeader.Reserved1) + "\n";         /* Reserved (Always 0) */		//BYTE	
		message += "NumBitPlanes:   " + to_string(pcxHeader.NumBitPlanes) + "\n";      /* Number of Bit Planes */		//BYTE	
		message += "BytesPerLine:   " + to_string(pcxHeader.BytesPerLine) + "\n";      /* Bytes per Scan-line */		//WORD	
		message += "PaletteType:    " + to_string(pcxHeader.PaletteType) + "\n";       /* Palette Type */				//WORD	
		message += "HorzScreenSize: " + to_string(pcxHeader.HorzScreenSize) + "\n";    /* Horizontal Screen Size */		//WORD	
		message += "VertScreenSize: " + to_string(pcxHeader.VertScreenSize) + "\n";    /* Vertical Screen Size */		//WORD	
		message += "Reserved2[54] \n";													/* Reserved (Always 0) */		//BYTE	

		DebugConsole::Log(message);
	}

	void ExtractPCX(string const &data, int startIndex, PCXHeader header) {
		if (header.Identifier != 10) {
			DebugConsole::Log("[E] Not a PCX file!");
			return;    // Not a PCX file.
		}

		if (header.Encoding != 1) {
			DebugConsole::Log("[E] Unsupported encoding!");
			return;    // Not supporting non-RLE encoding. Would assume a value of 0 indicates raw, unencoded, but that is apparently never used.
		}

		if (header.BitsPerPixel != 1 && header.BitsPerPixel != 2 && header.BitsPerPixel != 4 && header.BitsPerPixel != 8) {
			DebugConsole::Log("[E] Unsupported pixel format!");
			return;    // Unsupported pixel format.
		}

		string pcxData;
		int width = header.XEnd - header.XStart + 1;
		int height = header.YEnd - header.YStart + 1;
		int components = 1;
		components = (header.BitsPerPixel == 8 && header.NumBitPlanes == 4) ? 4 : components;

		size_t dataSize = width * height * components;

		//pcxData = drpcx_load_memory(&data[startIndex], dataSize, DR_FALSE, NULL, NULL, NULL, 4);
		pcxData = data.substr(startIndex, dataSize);

		string fileName = pcxFileName + to_string(pcxIndex) + ".pcx";
		ofstream newPCXFile(outputPath.string() + "//" + fileName, ios::binary);
		newPCXFile << pcxData;
		pcxIndex++;
	}
};
