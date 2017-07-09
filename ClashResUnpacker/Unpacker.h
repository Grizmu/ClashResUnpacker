#pragma once
#include <string>

class Unpacker {
public:
	enum UnpackerType {
		UP_MAXIMUM,
		UP_NORMAL,
		UP_MUSIC,
		UP_MINIMUM,
		UP_UNKNOWN
	};
private:
	UnpackerType type;

	Unpacker() {}

protected:

	Unpacker(UnpackerType type) {
		this->type = type;
	}

public:

	virtual ~Unpacker() {};
	virtual void Unpack(std::string file, std::string outputDir) {};
};