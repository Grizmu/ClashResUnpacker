#pragma once
#include <string>
 
class Color {
public:
	char r;
	char g;
	char b;
	char a;

	Color(char r, char g, char b, char a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	Color(int r, int g, int b, int a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	std::string ToString() {
		return std::string({ (char)r, (char)g, (char)b, (char)a });
	}
};