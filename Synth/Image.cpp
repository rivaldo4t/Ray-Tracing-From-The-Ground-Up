#include "Image.hpp"

// use #define in one of the cpp files and then include headers without #define in others
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

Image :: Image() 
{ 
	data = nullptr; 
}

Image :: Image(const char* path)
{
	data = stbi_load(path, &width, &height, &bpp, 3);
	texture = vector<vector<cyPoint3d>>(height, vector<cyPoint3d>(width));
	int k = 0;
	for (int j = height - 1; j >= 0; --j)
	{
		for (int i = 0; i < width; ++i)
		{
			texture[j][i][0] = (data[k]) / 255.0;
			texture[j][i][1] = (data[k + 1]) / 255.0;
			texture[j][i][2] = (data[k + 2]) / 255.0;
			k += 3;
		}
	}
	stbi_image_free(data);
}

// fix the hardcoding
void Image ::writeImage(const char* path, float(*frameBuffer)[960][3])
{
	uint8_t* rgb_image = new uint8_t[Xmax * Ymax * 3];
	int k = 0;
	for (int j = Ymax - 1; j >= 0; --j)
	{
		for (int i = 0; i < Xmax; ++i)
		{
			rgb_image[k] = unsigned char(frameBuffer[j][i][0] * 255);
			rgb_image[k + 1] = unsigned char(frameBuffer[j][i][1] * 255);
			rgb_image[k + 2] = unsigned char(frameBuffer[j][i][2] * 255);
			k += 3;
		}
	}
	stbi_write_jpg(path, Xmax, Ymax, 3, rgb_image, Xmax * 3);
	stbi_image_free(rgb_image);
}