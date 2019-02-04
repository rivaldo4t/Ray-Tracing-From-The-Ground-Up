#pragma once
#include <iostream>
#include "Utility.hpp"
using namespace std;

class Image
{
public:
	int width, height, bpp;
	uint8_t* data;
	vector<vector<cyPoint3d>> texture;

	Image();
	Image(const char* path);
	static void flip(vector<float>& data);
	static void writeImage(const char* path, vector<float>& frameBuffer);
};
