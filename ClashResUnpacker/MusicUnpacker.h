#pragma once
#include "Utils.h"
#include "Unpacker.h"
#include <filesystem>
#include <fstream>
#include <vector>

using namespace std;
using namespace std::experimental;

class MusicUnpacker : public Unpacker {
private:

	string resourceFileName = "song";
	int index = 0;

	filesystem::v1::path outputPath;
	string fileNames[11];

	void CreateOutputFolder(std::string outputDir) {
		outputPath = filesystem::v1::path(outputDir + "//music");

		//Check output directory
		bool outputExists = filesystem::v1::exists(outputPath);

		if (outputExists) {
			//All is ok
			DebugConsole::Log("Found music output directory.");
		}
		else {
			//Create output directory
			filesystem::v1::create_directory(outputPath);
			DebugConsole::Log("Created music output directory.");
		}
	}

public:

	MusicUnpacker() :
		Unpacker(UnpackerType::UP_MUSIC) {

		fileNames[0] = "BATTLE";
		fileNames[1] = "CASTLE1";
		fileNames[2] = "CASTLE2";
		fileNames[3] = "CASTLE3";
		fileNames[4] = "CASTLE5";
		fileNames[5] = "CASTLE4";
		fileNames[6] = "MENU";
		fileNames[7] = "MAINMAP3";
		fileNames[8] = "MAINMAP2";
		fileNames[9] = "MAINMAP4";
		fileNames[10] = "MAINMAP1";
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
		
		DebugConsole::Log("File size : " + to_string(size) + " bytes.");
		//DebugConsole::Log("Symbol : " + to_string(data[startPos]));

		//Find all WAV files
		vector<size_t> startPositions;
		vector<size_t> endPositions; 

		string wavHeader = "RIFF";
		string wavFooter = "LIST";

		//Find start positions
		size_t pos = data.find(wavHeader, 0);
		while (pos != string::npos)
		{
			startPositions.push_back(pos);
			pos = data.find(wavHeader, pos + 1);
		}

		//Find end positions
		pos = data.find(wavFooter, 0);
		while (pos != string::npos)
		{
			endPositions.push_back(pos);
			pos = data.find(wavFooter, pos + 1);
		}

		DebugConsole::Log("Found " + to_string(startPositions.size()) + " wav files.");

		size_t footerSize = 32;

		//Save to wav
		for (int i = 0; i < startPositions.size(); i++) {
			//DebugConsole::Log("File no " + to_string(i) + ":");
			ExtractWav(data, startPositions[i], endPositions[i]+footerSize, fileNames[i]);
		}

		DebugConsole::Log("");
		DebugConsole::Log("Music Unpacking complete.");
		
	}

	void ExtractWav(string const &data, int startIndex, int endIndex, string name) {
		string wavData;
		//todo
		//pcxData = drpcx_load_memory(&data[startIndex], dataSize, DR_FALSE, NULL, NULL, NULL, 4);
		wavData = data.substr(startIndex, endIndex - startIndex);

		string fileName = name + ".wav";
		ofstream newWavFile(outputPath.string() + "//" + fileName, ios::binary);
		newWavFile << wavData;
		index++;
	}

};