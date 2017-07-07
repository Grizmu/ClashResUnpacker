#pragma once
#include <string>
#include <list>
#include <vector>
#include "Utils.h"
#include <iomanip>
#include <algorithm>

struct Palette {
	uint32_t color [256];
};

struct S32ResData {
	char name[14];
	int32_t unk_value;
	int32_t offset;
	int32_t size;
};

struct S32Image {
	uint32_t imageOffset;
	uint32_t imageSize;
};

class S32File {
	S32ResData resData;
	std::string binData;
	std::vector<S32Image> images;
	
	int16_t imageAmount; //Max supported image amount by s32 format is 1000
public:

	S32File() {

	}

	S32File(const std::string &data, const S32ResData &s32ResData) {
		resData = s32ResData;
		binData = data.substr(s32ResData.offset, s32ResData.size);

		//Count and find images
		std::list<int32_t> imageOffsets(0);
		std::list<int32_t> imageSizes(0);
		imageOffsets.push_back(4096); //Push first offset that is always on 4096

		imageAmount = 1;
		size_t pos = 4;
		uint32_t offset = 4096;
		uint32_t lastOffset = 4096;

		while (pos < 4096) {
			lastOffset = offset;
			memcpy(&offset, binData.substr(pos, 4).c_str(), 4); //Load new offset

			if (offset == 0) {
				break;
			}
			else {
				imageOffsets.push_back(offset);
				imageSizes.push_back(offset - lastOffset);
				pos += 4;
				imageAmount++;
			}
		}
		imageSizes.push_back(s32ResData.size - lastOffset);

		images = std::vector<S32Image>(imageAmount);

		//Load image offsets
		for (int i = 0; i < imageAmount; i++) {
			images[i].imageOffset = imageOffsets.front();
			images[i].imageSize = imageSizes.front();

			imageOffsets.pop_front();
			imageSizes.pop_front();
		}

		isUsable = true;
	}

	void ConvertToBMP(const std::string &outputPath, Palette &palette) {
		if (!isUsable) {
			return;
		}

		size_t imageAmount = images.size();
		uint16_t width;
		uint16_t height;
		uint16_t method;
		uint32_t buffer;

		uint32_t *pixels;
		uint32_t pixelAmount;

		std::string imageData;
		size_t ptr;
		size_t pixelsPtr;

		//bool lastWasCommand = false;
		uint8_t commandMask = 0x80;
		uint8_t valueMask = 0x7F;

		uint16_t BytesToParse; //Amount of bytes to parse after pixel amount value.
		uint32_t color;

		uint8_t red;
		uint8_t green;
		uint8_t blue;

		for (int i = 0; i < imageAmount; i++) {
			imageData = binData.substr(images[i].imageOffset, images[i].imageSize);

			ptr = 0;
			//width = reinterpret_cast<int16_t>(imageData.substr(ptr, 2).c_str()); ptr += 2;
			//height = reinterpret_cast<int16_t>(imageData.substr(ptr, 2).c_str()); ptr += 8; //ptr skips the method and buffer.
			memcpy(&width, imageData.substr(ptr, 2).c_str(), 2); ptr += 2;
			memcpy(&height, imageData.substr(ptr, 2).c_str(), 2); ptr += 8;

			pixelAmount = width * height;

			pixels = new uint32_t [pixelAmount]; //NEW PIXEL POINTER

			std::fill_n(pixels, pixelAmount, 0x000000ff); // RRGGBBAA

			pixelsPtr = 0;
			while (ptr < images[i].imageSize) {
				if ((unsigned char)(commandMask & imageData[ptr]) > 0) {
					//IS A COMMAND
					pixelsPtr += (imageData[ptr] & valueMask);
					ptr++;
					//lastWasCommand = true;
				}
				else {
					//if (lastWasCommand) {
					//THIS MIGHT BE ROW FILL OR PIXEL COUNT
					BytesToParse = (unsigned char) imageData[ptr];
					ptr++;

					if (BytesToParse == 0x00) { //ROW FILL!
						red = ((uint8_t)imageData[ptr]);
						green = ((uint8_t)imageData[ptr+1]);
						blue = ((uint8_t)imageData[ptr+2]);
						color = 0;
						color = (color + (red)) << 8;
						color = (color + (green)) << 8;
						color = (color + (blue)) << 8;
						color = color + (0xff);
						ptr += 4;

						if (pixelsPtr + width > pixelAmount) { 
							//End of the pixels array!
							//Poormans anti overflow

							if (pixelsPtr > pixelAmount) {
								break; //Sometimes pixelsPtr is already overflowed.
							}
							uint32_t amount = pixelAmount - pixelsPtr;
							std::fill_n(&pixels[pixelsPtr], amount, color);
							break;
						}
						else {
							std::fill_n(&pixels[pixelsPtr], width, color);
						}
						
						pixelsPtr += width;
					}
					else { //PIXEL COUNT
						for (int j = 0; j < BytesToParse; j++) {

							//Poormans anti overflow
							if (pixelsPtr >= pixelAmount) {
								break;
							}

							color = palette.color[(unsigned char)imageData[ptr + j]];
							std::reverse((char *)&color, ((char *)&color) + 4); //Flip to be bmp compatible - little eidian.
							pixels[pixelsPtr] = color;
							pixelsPtr++;
						}

						ptr += BytesToParse;
					}
				}

				//ptr++;
			}
			
			//Export result to file.
			string resName = resData.name;
			std::replace(resName.begin(), resName.end(), '.', '_');
			
			BMPHeader header = BMPHeader::CreateBMPHeader(width, height);
			//BMPHeader *addr = &header;

			ofstream newBMPFile(outputPath + "//" + resName + "_" + to_string(i) + ".bmp", ios::binary);
			string headerStr = header.ToString();
			newBMPFile.write(headerStr.c_str(), headerStr.size()); //Write the bmp header.

			for (int y = height - 1; y >= 0; y--) {
				for (int x = 0; x < width; x++) {
					newBMPFile.write((char *) &pixels[y*width + x], 4);
				}
			}

			//string fileData((char *)pixels, pixelAmount * 4);

			//for (int i = 0; i < pixelAmount; i++) {
			//newBMPFile << fileData;
			//}

			newBMPFile.flush();
			newBMPFile.close();

			delete[] pixels; //DEL PIXEL POINTER			
		}

	}

	const std::string &GetBinData() {
		return binData;
	}

	bool IsUsable() {
		return isUsable;
	}
private:
	bool isUsable = false;

};

class S32Unpacker {
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

		memcpy(&palette, paletteData.c_str(), sizeof(Palette)); //Copy the palette to the variable.

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