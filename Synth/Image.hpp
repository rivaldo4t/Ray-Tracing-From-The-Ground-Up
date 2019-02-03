#pragma once
#include <iostream>
#include "Utility.hpp"
using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

class Image
{
public:
	int width, height, bpp;
	uint8_t* data;
	vector<vector<cyPoint3d>> texture;

	Image();
	Image(const char* path);
	static void writeImage(const char* path, float(*frameBuffer)[960][3]);
};
