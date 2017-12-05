#pragma once

class FormatUnpacker {

protected: 
	bool shouldConvertFiles = true;

public: 
	void SetConvertFiles(bool value) {
		shouldConvertFiles = true;
	}
};