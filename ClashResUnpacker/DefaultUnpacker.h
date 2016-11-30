#pragma once
#include "Unpacker.h"

class DefaultUnpacker : public Unpacker {
public:
	DefaultUnpacker() :
		Unpacker(UnpackerType::UP_UNKNOWN) {
	}
};
