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

#include <vector>
#include "Palette.h"

#include "Utils.h"

#include <png.h>
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

	void ConvertToPNG(const std::string fileName, std::string &outputPath, Palette &palette) {
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

							pixels[pixelsPtr] = palette.color[(unsigned char)binData[ptr + j]];
							pixelsPtr++;
						}

						ptr += BytesToParse;
					}
				}
			}

			//Export result to file.
			string resName = fileName;
			std::replace(resName.begin(), resName.end(), '.', '_');

			string pathString = (outputPath + "//" + resName + "_" + to_string(i) + ".png");
			const char * path = pathString.c_str();

			//PNG
			FILE * fp;
			png_structp png_ptr = NULL;
			png_infop info_ptr = NULL;
			size_t x, y;
			png_byte ** row_pointers = NULL;
			/* "status" contains the return value of this function. At first
			it is set to a value which means 'failure'. When the routine
			has finished its work, it is set to a value which means
			'success'. */
			int status = -1;
			/* The following number is set by trial and error only. I cannot
			see where it it is documented in the libpng manual.
			*/

			int pixel_size = 4;
			int depth = 8;

			fp = fopen(path, "wb");
			if (!fp) {
				cout << "Failed to open the file!" << endl;
				if (pixels != nullptr) { delete[] pixels; }
				return;
			}

			png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
			if (png_ptr == NULL) {
				cout << "Failed to create png write struct!" << endl;;
				fclose(fp);
				if (pixels != nullptr) { delete[] pixels; }
				return;
			}

			info_ptr = png_create_info_struct(png_ptr);
			if (info_ptr == NULL) {
				cout << "Failed to create info struct!" << endl;
				fclose(fp);
				if (pixels != nullptr) { delete[] pixels; }
				return;
			}

			/* Set up error handling. */

			if (setjmp(png_jmpbuf(png_ptr))) {
				cout << "Failed to create a png!" << endl;
				fclose(fp);
				if (pixels != nullptr) { delete[] pixels; }
				return;
			}

			/* Set image attributes. */

			png_set_IHDR(png_ptr,
				info_ptr,
				width,
				height,
				depth,
				PNG_COLOR_TYPE_RGBA,
				PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_DEFAULT,
				PNG_FILTER_TYPE_DEFAULT);

			/* Initialize rows of PNG. */

			int pixelIndex;

			row_pointers = (png_bytep*)png_malloc(png_ptr, height * sizeof(png_byte *));
			for (y = 0; y < height; y++) {
				png_byte *row =
					(png_bytep)png_malloc(png_ptr, sizeof(uint8_t) * width * pixel_size);
				row_pointers[y] = row;
				for (x = 0; x < width; x++) {
					pixelIndex = x + y * width;
					*row++ = (pixels[pixelIndex] >> 24) & 0xFF;
					*row++ = (pixels[pixelIndex] >> 16) & 0xFF;
					*row++ = (pixels[pixelIndex] >> 8 ) & 0xFF;
					*row++ = (pixels[pixelIndex]      ) & 0xFF;
				}
			}

			/* Write the image data to "fp". */

			png_init_io(png_ptr, fp);
			png_set_rows(png_ptr, info_ptr, row_pointers);
			png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

			/* The routine has successfully written the file, so we set
			"status" to a value which indicates success. */

			status = 0;

			fclose(fp);

			//Clean memory
			for (y = 0; y < height; y++) {
				png_free(png_ptr, row_pointers[y]);
			}
			png_free(png_ptr, row_pointers);

			if (png_ptr && info_ptr) {
				png_destroy_write_struct(&png_ptr, &info_ptr);
			}
		}

		if (pixels != nullptr) {
			delete[] pixels; //DEL PIXEL POINTER	
		}
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

		size_t ptr;
		size_t pixelsPtr;

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

							pixels[pixelsPtr] = palette.color[(unsigned char)binData[ptr + j]];
							pixelsPtr++;
						}

						ptr += BytesToParse;
					}
				}
			}

			//Export result to file
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
