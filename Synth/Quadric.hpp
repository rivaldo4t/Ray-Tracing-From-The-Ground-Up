#pragma once
#include <iostream>
#include <vector>
#include <math.h>
#include "cyPoint.h"
#include "Image.hpp"
#include "Material.hpp"
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
	Material material;
	vector<cyPoint3d> points;
	vector<cyPoint2d> texpoints;
	double s, t;
	
	//Quadric() {}

	Quadric(cyPoint3d _ai2,
		double _a21,
		double _a00,
		cyPoint3d _qc,
		cyPoint3d _si,
		vector<cyPoint3d> _N,
		Material mat,
		vector<cyPoint3d> _points = {},
		vector<cyPoint2d> _texpoints = {});

	double intersect(cyPoint3d eye, cyPoint3d eyeToPix);
	double intersect_length(cyPoint3d& eye, cyPoint3d& eyeToPix, double lightdist);
	cyPoint3d normalAtHitPoint(cyPoint3d hitPoint);
	cyPoint3d computeAmbientColor();
	cyPoint3d computeDiffuseColor(cyPoint3d& normalAtHit, cyPoint3d& hitPointToLight, cyPoint3d& lightColor, double pointToLightDist, double spotLightComp = 1.0, double t = 0.0);
	cyPoint3d computeSpecularColor(cyPoint3d& normalAtHit, cyPoint3d& hitPointToLight, cyPoint3d& lightColor, cyPoint3d& eyeToHitPoint, double spotLightComp = 1.0);
	cyPoint3d computeBorderColor(cyPoint3d& normalAtHit, cyPoint3d& eyeToHitPoint, double spotLightComp = 1.0);
	cyPoint3d computeTextureColor(cyPoint3d hitPoint, cyPoint3d normalAtHit, bool env = false);
};