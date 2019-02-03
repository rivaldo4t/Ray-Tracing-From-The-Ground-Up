#pragma once
#include <iostream>
#include <vector>
#include <math.h>
#include "cyPoint.h"
#include "Image.hpp"
using namespace std;

class Quadric
{
public:
	int type;
	cyPoint3d ai2;
	double a21, a00;
	cyPoint3d qc;
	cyPoint3d si;
	vector<cyPoint3d> N;
	cyPoint3d color;
	double ambientFact, diffuseFact, specularFact, borderFact;
	cyPoint3d ambientColor, diffuseColor, specularColor, borderColor;
	Image& textureImage;
	Image& normalImage;
	vector<cyPoint3d> points;
	vector<cyPoint2d> texpoints;
	double reflectivity;
	double refractive_index;
	double s, t;
	//Quadric() {}

	Quadric(cyPoint3d _ai2,
		double _a21,
		double _a00,
		cyPoint3d _qc,
		cyPoint3d _si,
		vector<cyPoint3d> _N,
		vector<pair<double, cyPoint3d>> _colors, 
		Image& _I, Image& _I2,
		vector<cyPoint3d> _points = {},
		vector<cyPoint2d> _texpoints = {}, 
		double _ref = 0.0,
		double _ior = 1.6) :
		ai2(_ai2), a21(_a21), a00(_a00), qc(_qc), si(_si), N(_N),
		ambientFact(_colors[0].first), ambientColor(_colors[0].second),
		diffuseFact(_colors[1].first), diffuseColor(_colors[1].second),
		specularFact(_colors[2].first), specularColor(_colors[2].second),
		borderFact(_colors[3].first), borderColor(_colors[3].second),
		textureImage(_I), normalImage(_I2), points(_points), texpoints(_texpoints),
		reflectivity(_ref), refractive_index(_ior)
		{
			N[0].Normalize();
			N[1].Normalize();
			N[2].Normalize();
			//plane
			if (ai2.IsZero())
				type = 0;
			//sphere; for now
			else
				type = 1;
			s = 0.0;
			t = 0.0;
		}

	double intersect(cyPoint3d eye, cyPoint3d eyeToPix);
	double intersect_length(cyPoint3d& eye, cyPoint3d& eyeToPix, double lightdist);
	cyPoint3d normalAtHitPoint(cyPoint3d hitPoint);
	cyPoint3d computeAmbientColor();
	cyPoint3d computeDiffuseColor(cyPoint3d& normalAtHit, cyPoint3d& hitPointToLight, cyPoint3d& lightColor, double pointToLightDist, double spotLightComp = 1.0, double t = 0.0);
	cyPoint3d computeSpecularColor(cyPoint3d& normalAtHit, cyPoint3d& hitPointToLight, cyPoint3d& lightColor, cyPoint3d& eyeToHitPoint, double spotLightComp = 1.0);
	cyPoint3d computeBorderColor(cyPoint3d& normalAtHit, cyPoint3d& eyeToHitPoint, double spotLightComp = 1.0);
	cyPoint3d computeTextureColor(cyPoint3d hitPoint, cyPoint3d normalAtHit, bool env = false);
};