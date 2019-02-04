#pragma once
#include <iostream>
#include <vector>
#include "Image.hpp"
#include "cyPoint.h"

class Material
{
public:
	double ambientFact, diffuseFact, specularFact, borderFact;
	cyPoint3d ambientColor, diffuseColor, specularColor, borderColor;
	Image& textureImage;
	Image& normalImage;
	double reflectivity;
	double refractive_index;
	
	// default material colors
	Material(double ambF, cyPoint3d ambC, double difF, cyPoint3d difC, double specF, cyPoint3d specC,
		double borF, cyPoint3d borC, Image & tex, Image & nor, double refl = 0.0, double refr = 1.6);
};
