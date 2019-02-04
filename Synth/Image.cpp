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

void Image :: flip(vector<float>& data)
{
	std::vector<float> flippedData;
	int row = Xmax * 3;
	for (size_t i = 0; i < data.size(); i += row)
		flippedData.insert(flippedData.begin(), data.begin() + i, data.begin() + i + row);
	data = std::move(flippedData);
}

void Image :: writeImage(const char* path, vector<float>& frameBuffer)
{
	uint8_t* rgb_image = new uint8_t[Ymax * Xmax * 3];
	flip(frameBuffer);
	for (int j = 0; j < Ymax; ++j)
		for (int i = 0; i < Xmax; ++i)
			for (int k = 0; k < 3; ++k)
				rgb_image[(j * Xmax + i) * 3 + k] = uint8_t(frameBuffer[(j * Xmax + i) * 3 + k] * 255);
			
	stbi_write_jpg(path, Xmax, Ymax, 3, rgb_image, Xmax * 3);
	stbi_image_free(rgb_image);
}