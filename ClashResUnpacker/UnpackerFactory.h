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
#include "Unpacker.h"
#include "MusicUnpacker.h"
#include "MaximumUnpacker.h"
#include "NormalUnpacker.h"
#include "MinimumUnpacker.h"
#include "DefaultUnpacker.h"
#include "Gfx3Unpacker.h"
#include "InfoAngUnpacker.h"
#include "InfoPolUnpacker.h"
#include "SetupUnpacker.h"
#include "IsUnpacker.h"
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
		case Unpacker::UP_NORMAL:
			return make_shared<NormalUnpacker>();
			break;
		case Unpacker::UP_MINIMUM:
			return make_shared<MinimumUnpacker>();
			break;
		case Unpacker::UP_MUSIC:
			return make_shared<MusicUnpacker>();
			break;
		case Unpacker::UP_GFX3:
			return make_shared<Gfx3Unpacker>();
			break;
		case Unpacker::UP_IS:
			return make_shared<IsUnpacker>();
			break;
		case Unpacker::UP_INFOANG:
			return make_shared<InfoAngUnpacker>();
			break;
		case Unpacker::UP_INFOPOL:
			return make_shared<InfoPolUnpacker>();
			break;
		case Unpacker::UP_SETUP:
			return make_shared<SetupUnpacker>();
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