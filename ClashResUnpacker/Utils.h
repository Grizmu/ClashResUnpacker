#pragma once
#define UNPACKER_DEBUG
#include <iostream>
#include <windows.h>
#include <string> 
#include <filesystem>
#include <experimental/filesystem>
#include <fstream>

using namespace std;
using namespace std::experimental;

//Header size is bOffBits

struct BMPHeader
{
	uint16_t bfType;					//specifies the file type
	uint32_t bfSize;					//specifies the size in bytes of the bitmap file
	uint16_t bfReserved1;				//reserved; must be 0
	uint16_t bfReserved2;				//reserved; must be 0
	uint32_t bOffBits;					//species the offset in bytes from the bitmapfileheader to the bitmap bits
	uint32_t biSize;					//specifies the number of bytes required by the struct
	uint32_t biWidth;					//specifies width in pixels
	uint32_t biHeight;					//species height in pixels
	uint16_t biPlanes;					//specifies the number of color planes, must be 1
	uint16_t biBitCount;				//specifies the number of bit per pixel
	uint32_t biCompression;				//spcifies the type of compression
	uint32_t biSizeImage;				//size of image in bytes
	uint32_t biXPelsPerMeter;			//number of pixels per meter in x axis
	uint32_t biYPelsPerMeter;			//number of pixels per meter in y axis
	uint32_t biClrUsed;					//number of colors used by th ebitmap
	uint32_t biClrImportant;			//number of colors that are important
	unsigned char extraData[16] = { 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00 };

	//Only support 32 bit bmps.
	static BMPHeader CreateBMPHeader(int width, int height) {
		BMPHeader header;
		header.bfType = 0x4D42;
		header.bfSize = width * height * 4 + 0x00000046;
		header.bfReserved1 = 0x0000;
		header.bfReserved2 = 0x0000;
		header.bOffBits = 0x00000046;

		header.biSize = 0x0000038;
		header.biWidth = width;
		header.biHeight = height;
		header.biPlanes = 0x0001;
		header.biBitCount = 0x0020; //32 bit
		header.biCompression = 0x00000003; //Bitfields
		header.biSizeImage = width * height * 4;
		header.biXPelsPerMeter = 0x0064;
		header.biYPelsPerMeter = 0x0064;
		header.biClrUsed = 0x00000000;
		header.biClrImportant = 0x00000000;

		return header;
	}

	std::string ToString() {
		char charData [100];
		std::string data;
		size_t ptr = 0;

		memcpy(&charData[ptr], &bfType, sizeof(uint16_t)); ptr += sizeof(uint16_t);		     //uint16_t bfType;					//specifies the file type
		memcpy(&charData[ptr], &bfSize, sizeof(uint32_t)); ptr += sizeof(uint32_t);		     //uint32_t bfSize;					//specifies the size in bytes of the bitmap file
		memcpy(&charData[ptr], &bfReserved1, sizeof(uint16_t)); ptr += sizeof(uint16_t);	 //uint16_t bfReserved1;			//reserved; must be 0
		memcpy(&charData[ptr], &bfReserved2, sizeof(uint16_t)); ptr += sizeof(uint16_t);	 //uint16_t bfReserved2;			//reserved; must be 0
		memcpy(&charData[ptr], &bOffBits, sizeof(uint32_t)); ptr += sizeof(uint32_t);		 //uint32_t bOffBits;				//species the offset in bytes from the bitmapfileheader to the bitmap bits
		
		memcpy(&charData[ptr], &biSize, sizeof(uint32_t)); ptr += sizeof(uint32_t);		     //uint32_t biSize;					//specifies the number of bytes required by the struct
		memcpy(&charData[ptr], &biWidth, sizeof(uint32_t)); ptr += sizeof(uint32_t);		 //uint32_t biWidth;				//specifies width in pixels
		memcpy(&charData[ptr], &biHeight, sizeof(uint32_t)); ptr += sizeof(uint32_t);		 //uint32_t biHeight;				//species height in pixels
		memcpy(&charData[ptr], &biPlanes, sizeof(uint16_t)); ptr += sizeof(uint16_t);		 //uint16_t biPlanes;				//specifies the number of color planes, must be 1
		memcpy(&charData[ptr], &biBitCount, sizeof(uint16_t)); ptr += sizeof(uint16_t);		 //uint16_t biBitCount;				//specifies the number of bit per pixel
		memcpy(&charData[ptr], &biCompression, sizeof(uint32_t)); ptr += sizeof(uint32_t);	 //uint32_t biCompression;			//spcifies the type of compression
		memcpy(&charData[ptr], &biSizeImage, sizeof(uint32_t)); ptr += sizeof(uint32_t);	 //uint32_t biSizeImage;			//size of image in bytes
		memcpy(&charData[ptr], &biXPelsPerMeter, sizeof(uint32_t)); ptr += sizeof(uint32_t); //uint32_t biXPelsPerMeter;		//number of pixels per meter in x axis
		memcpy(&charData[ptr], &biYPelsPerMeter, sizeof(uint32_t)); ptr += sizeof(uint32_t); //uint32_t biYPelsPerMeter;		//number of pixels per meter in y axis
		memcpy(&charData[ptr], &biClrUsed, sizeof(uint32_t)); ptr += sizeof(uint32_t);		 //uint32_t biClrUsed;				//number of colors used by th ebitmap
		memcpy(&charData[ptr], &biClrImportant, sizeof(uint32_t)); ptr += sizeof(uint32_t);  //uint32_t biClrImportant;			//number of colors that are important
		memcpy(&charData[ptr], &extraData, 16); ptr += 16;		                             //unsigned char extraData[16] = { 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00 };

		data = string(charData, ptr);

		return data;
	}
};

struct PCXHeader
{
	BYTE	Identifier;        /* PCX Id Number (Always 0x0A) */
	BYTE	Version;           /* Version Number */
	BYTE	Encoding;          /* Encoding Format */
	BYTE	BitsPerPixel;      /* Bits per Pixel */
	WORD	XStart;            /* Left of image */
	WORD	YStart;            /* Top of Image */
	WORD	XEnd;              /* Right of Image */
	WORD	YEnd;              /* Bottom of image */
	WORD	HorzRes;           /* Horizontal Resolution */
	WORD	VertRes;           /* Vertical Resolution */
	BYTE	Palette[48];       /* 16-Color EGA Palette */
	BYTE	Reserved1;         /* Reserved (Always 0) */
	BYTE	NumBitPlanes;      /* Number of Bit Planes */
	WORD	BytesPerLine;      /* Bytes per Scan-line */
	WORD	PaletteType;       /* Palette Type */
	WORD	HorzScreenSize;    /* Horizontal Screen Size */
	WORD	VertScreenSize;    /* Vertical Screen Size */
	BYTE	Reserved2[54];     /* Reserved (Always 0) */
};

//_____________________________________________________________________________
filesystem::v1::path GetExePath()
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	wstring ws(buffer);
	string str(ws.begin(), ws.end());

	string::size_type pos = str.find_last_of("\\/");

	filesystem::v1::path exePath(string(str).substr(0, pos));

	return exePath;
}

//_____________________________________________________________________________
class DebugConsole {
public:
	static void LogDebug(string msg) {
#ifdef UNPACKER_DEBUG
		cout << msg << endl;
#endif
	}

	static void Log(string msg) {
		cout << msg << endl;
	}
};

bool FileExists(string file) {
	filesystem::v1::path filePath(file);
	return filesystem::v1::exists(filePath);
}
//_____________________________________________________________________________
void LoadFileData(string &data, string file, size_t &fileSize) {
	filesystem::v1::path filePath(file);
	bool fileExists = filesystem::v1::exists(filePath);

	/*
	if (fileExists) {
		DebugConsole::Log("Unpacking " + file);
	}
	else {
		DebugConsole::Log("File " + file + " not found!");
		return;
	}

	
	std::ifstream in(filePath.string());
	ostringstream ss = std::ostringstream{};
	ss << in.rdbuf();

	string s = ss.str();
	*/

	std::ifstream in(filePath.string(), ios::binary);
	in.seekg(0, std::ios::end);
	fileSize = in.tellg();
	data = string(fileSize, ' ');
	in.seekg(0);
	in.read(&data[0], fileSize);

	in.close();
}

//Returns output path.
filesystem::v1::path CreateOutputFolder(std::string outputDir) {
	filesystem::v1::path outputPath = filesystem::v1::path(outputDir);

	//Check output directory
	bool outputExists = filesystem::v1::exists(outputPath);

	if (outputExists) {
		//All is ok
		//DebugConsole::Log("Found output directory.");
	}
	else {
		//Create output directory
		filesystem::v1::create_directory(outputPath);
		//DebugConsole::Log("Created output directory.");
	}

	return outputPath;
}

void LoadFileData(string &data, string file) {
	size_t fileSize;

	LoadFileData(data, file, fileSize);
}