#pragma once
#include "Unpacker.h"
#include "MusicUnpacker.h"
#include "MaximumUnpacker.h"
#include "DefaultUnpacker.h"
#include <memory>

typedef shared_ptr<Unpacker> Unpacker_sp;

class UnpackerFactory {
public:
	static Unpacker_sp CreateUnpacker(Unpacker::UnpackerType type) {
		switch (type)
		{
		case Unpacker::UP_MAXIMUM:
			return make_shared<MaximumUnpacker>();
			break;
		case Unpacker::UP_MUSIC:
			return make_shared<MusicUnpacker>();
			break;
		case Unpacker::UP_UNKNOWN:
			return make_shared<DefaultUnpacker>();
			break;
		default:
			return make_shared<DefaultUnpacker>();
			break;
		}
	}
};