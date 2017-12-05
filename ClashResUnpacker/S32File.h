#pragma once

#include <vector>
#include "Palette.h"

#include "Utils.h"

struct S32Image {
	uint32_t imageOffset;
	uint32_t imageSize;
};

class S32File {
	//S32ResData resData;
	std::string binData;
	std::vector<S32Image> images;

	int16_t imageAmount; //Max supported image amount by s32 format is 1000
public:

	S32File() {

	}

	S32File(std::string &data) {
		//resData = s32ResData;
		//binData = data.substr(s32ResData.offset, s32ResData.size);
		binData = data;

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
		imageSizes.push_back(binData.length() - lastOffset);

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

	void SaveToFile(string path) {
		ofstream newS32File(path, ios::binary);
		newS32File << binData;
	}

	void ConvertToBMP(const std::string fileName, std::string &outputPath, Palette &palette) {
		if (!isUsable) {
			return;
		}

		size_t imageAmount = images.size();
		uint16_t width;
		uint16_t height;
		uint16_t method;
		uint32_t buffer;

		uint32_t *pixels = nullptr;
		uint32_t pixelAmount;

		//std::string imageData;
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

		size_t offset;
		size_t imageOffsetEnd;

		BMPHeader bmpHeader = BMPHeader::CreateBMPHeader(0, 0);

		//pixel array optimisation. We don't need to create a new one if the current one is the same size.
		uint16_t lastWidth = 0;
		uint16_t lastHeight = 0;

		for (int i = 0; i < imageAmount; i++) {
			ptr = images[i].imageOffset; //Set ptr to image start
			imageOffsetEnd = images[i].imageOffset + images[i].imageSize; //set image offset end to image end

			memcpy(&width, binData.substr(ptr, 2).c_str(), 2); ptr += 2;
			memcpy(&height, binData.substr(ptr, 2).c_str(), 2); ptr += 8;

			pixelAmount = width * height;

			if (lastWidth == width && lastHeight == height) {
				std::fill_n(pixels, pixelAmount, 0x00000000); // RRGGBBAA
			}
			else {
				if (pixels != nullptr) {
					delete[] pixels;
					pixels = nullptr;
				}

				pixels = new uint32_t[pixelAmount]; //NEW PIXEL POINTER
				std::fill_n(pixels, pixelAmount, 0x00000000); // RRGGBBAA
			}

			lastWidth = width;
			lastHeight = height;

			pixelsPtr = 0;
			while (ptr < imageOffsetEnd) {
				if ((unsigned char)(commandMask & binData[ptr]) > 0) {
					//IS A COMMAND
					pixelsPtr += (binData[ptr] & valueMask);
					ptr++;
				}
				else {
					//THIS MIGHT BE ROW FILL OR PIXEL COUNT
					BytesToParse = (unsigned char)binData[ptr];
					ptr++;

					if (BytesToParse == 0x00) { //Use command from different offset!

						offset = *(uint32_t *)&binData[ptr];
						offset = ptr - offset;

						if ((unsigned char)(commandMask & binData[offset]) > 0) {
							//IS A COMMAND
							pixelsPtr += (binData[offset] & valueMask);
						}
						else {
							BytesToParse = (unsigned char)binData[offset];
							offset++;

							for (int j = 0; j < BytesToParse; j++) {

								//Poormans anti overflow
								if (pixelsPtr >= pixelAmount) {
									break;
								}

								//color = palette.color[(unsigned char)binData[offset + j]];
								//std::reverse((char *)&color, ((char *)&color) + 4); //Flip to be bmp compatible - little eidian.
								//pixels[pixelsPtr] = color;
								pixels[pixelsPtr] = palette.color[(unsigned char)binData[offset + j]];
								pixelsPtr++;
							}
						}

						ptr += 4;

					}
					else { //PIXEL COUNT
						for (int j = 0; j < BytesToParse; j++) {

							//Poormans anti overflow
							if (pixelsPtr >= pixelAmount) {
								break;
							}

							//color = palette.color[(unsigned char)binData[ptr + j]];
							//std::reverse((char *)&color, ((char *)&color) + 4); //Flip to be bmp compatible - little eidian.
							//pixels[pixelsPtr] = color;
							pixels[pixelsPtr] = palette.color[(unsigned char)binData[ptr + j]];
							pixelsPtr++;
						}

						ptr += BytesToParse;
					}
				}

				//ptr++;
			}

			//Export result to file.
			string resName = fileName;
			std::replace(resName.begin(), resName.end(), '.', '_');

			//BMPHeader *addr = &header;
			bmpHeader.UpdateBMPHeader(width, height);

			ofstream newBMPFile(outputPath + "//" + resName + "_" + to_string(i) + ".bmp", ios::binary);
			string headerStr = bmpHeader.ToString();
			newBMPFile.write(headerStr.c_str(), headerStr.size()); //Write the bmp header.

			for (int y = height - 1; y >= 0; y--) {
				newBMPFile.write((char *)&pixels[y*width], width * 4);
			}

			newBMPFile.flush();
			newBMPFile.close();

			//delete[] pixels; //DEL PIXEL POINTER			
		}

		if (pixels != nullptr) {
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
