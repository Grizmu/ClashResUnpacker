#pragma once
#include "Utils.h"
#include "Unpacker.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;
using namespace std::experimental;

class MaximumUnpacker : public Unpacker {
private:
	string pcxFileStartString;
	const size_t pcxHeaderSize = 128;

	string pcxFileName = "resource";
	int pcxIndex = 0;

	filesystem::v1::path outputPath;

	void CreateOutputFolder(std::string outputDir) {
		outputPath = filesystem::v1::path(outputDir + "//maximum");

		//Check output directory
		bool outputExists = filesystem::v1::exists(outputPath);

		if (outputExists) {
			//All is ok
			DebugConsole::Log("Found maximum output directory.");
		}
		else {
			//Create output directory
			filesystem::v1::create_directory(outputPath);
			DebugConsole::Log("Created maximum output directory.");
		}
	}

public:
	
	MaximumUnpacker():
	Unpacker(UnpackerType::UP_MAXIMUM){

		const size_t len = 12;
		BYTE pcxStartBytes[len] = { 0x0A, 0x05, 0x01, 0x08, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x02, 0xDF, 0x01 };
		pcxFileStartString = string(reinterpret_cast<char const*>(pcxStartBytes), len);
	}

	virtual void Unpack(std::string file, std::string outputDir) {
		if (FileExists(file)) {
			DebugConsole::Log("Unpacking " + file);
		}
		else {
			DebugConsole::Log("File " + file + " not found!");
			return;
		}

		CreateOutputFolder(outputDir);

		string data;
		size_t size;

		LoadFileData(data, file, size);

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
		DebugConsole::Log("Maximum unpacking complete.");
	}

	PCXHeader GetPCXHeaderData(char *start) {
		PCXHeader pcxHeaderData;
		memcpy(&pcxHeaderData, start, pcxHeaderSize); //Get the pcxHeader
		return pcxHeaderData;
	}

	void PrintPCXHeaderInfo(const PCXHeader &pcxHeader) {
		string message = "";

		message += "ID :" + to_string(pcxHeader.Identifier) + " ";		/* PCX Id Number (Always 0x0A) */	//BYTE	
		message += "Ver:" + to_string(pcxHeader.Version) + " ";			/* Version Number */				//BYTE	
		message += "Enc:" + to_string(pcxHeader.Encoding) + " ";		/* Encoding Format */				//BYTE	
		message += "BPP:" + to_string(pcxHeader.BitsPerPixel) + " ";	/* Bits per Pixel */				//BYTE	
		message += "XSt:" + to_string(pcxHeader.XStart) + " ";			/* Left of image */					//WORD	
		message += "YSt:" + to_string(pcxHeader.YStart) + " ";			/* Top of Image */					//WORD	
		message += "XEn:" + to_string(pcxHeader.XEnd) + " ";			/* Right of Image */				//WORD	
		message += "YEn:" + to_string(pcxHeader.YEnd) + " ";			/* Bottom of image */				//WORD	
		message += "Hres:" + to_string(pcxHeader.HorzRes) + " ";		/* Horizontal Resolution */			//WORD	
		message += "VRes:" + to_string(pcxHeader.VertRes) + " ";		/* Vertical Resolution */			//WORD	
		message += "Pal[48] ";											/* 16-Color EGA Palette */			//BYTE	
		string pal = string(49, '\0');
		for (int i = 0; i < 48; i++) {
			pal[i] = (char)(pcxHeader.Palette[i]);
		}
		//message += "   " + pal + "\n";

		message += "Res:"		+ to_string(pcxHeader.Reserved1) + " ";         /* Reserved (Always 0) */		//BYTE	
		message += "Planes:"	+ to_string(pcxHeader.NumBitPlanes) + " ";      /* Number of Bit Planes */		//BYTE	
		message += "BPL:"		+ to_string(pcxHeader.BytesPerLine) + " ";      /* Bytes per Scan-line */		//WORD	
		message += "PalType:"	+ to_string(pcxHeader.PaletteType) + " ";       /* Palette Type */				//WORD	
		message += "HScSize:"	+ to_string(pcxHeader.HorzScreenSize) + " ";    /* Horizontal Screen Size */	//WORD	
		message += "VScSize:"	+ to_string(pcxHeader.VertScreenSize) + " ";    /* Vertical Screen Size */		//WORD	
		message += "Res[54] \n";													/* Reserved (Always 0) */	//BYTE	

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

		//size_t dataSizeTest = width * height * components;

		char byte;
		int index = startIndex + 128;
		int currentPixelAmount = 0;
		int currentRow = 0;
		
		do {
			byte = data[index];
			if ((byte & 0xC0) == 0xC0) { // 11xx xxxx means that the pixel color is encoded, where xx xxxx is the amount of the pixels that share the same color.
				currentPixelAmount += (byte & 0x3F);
				index += 2;
			}
			else {
				currentPixelAmount++;
				index++;
			}

			if (currentPixelAmount >= width) {
				currentPixelAmount -= width;
				currentRow++;
			}

		} while (currentRow < height);

		index += 769;

		size_t dataSize = index - startIndex;

		stringstream consoleLog;

		consoleLog << "Data size : " << dataSize << endl;
		consoleLog << "Last 5 bytes: ";

		for (int i = 4; i >= 0; i--) {
			unsigned char c = data[index - i];
			consoleLog << std::hex << (int)c << std::dec << " ";
		}

		consoleLog << endl;

		DebugConsole::Log(consoleLog.str());

		//pcxData = drpcx_load_memory(&data[startIndex], dataSize, DR_FALSE, NULL, NULL, NULL, 4);
		pcxData = data.substr(startIndex, dataSize);

		string fileName = pcxFileName + to_string(pcxIndex) + ".pcx";
		ofstream newPCXFile(outputPath.string() + "//" + fileName, ios::binary);
		newPCXFile << pcxData;
		pcxIndex++;
	}
};