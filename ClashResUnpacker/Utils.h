#pragma once
#define UNPACKER_DEBUG
#include <iostream>
#include <windows.h>
#include <string> 
#include <filesystem>
#include <experimental/filesystem>

using namespace std;
using namespace std::experimental;

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
