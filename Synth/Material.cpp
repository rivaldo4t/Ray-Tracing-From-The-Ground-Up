#pragma once
#include "Material.hpp"

Material :: Material(double ambF, cyPoint3d ambC, double difF, cyPoint3d difC, double specF, cyPoint3d specC, 
	double borF, cyPoint3d borC, Image & tex, Image & nor, double refl, double refr) :
	ambientFact(ambF), ambientColor(ambC), diffuseFact(difF), diffuseColor(difC), specularFact(specF), specularColor(specC),
	borderFact(borF), borderColor(borC), 
	textureImage(tex), normalImage(nor),
	reflectivity(refl), refractive_index(refr) {}
