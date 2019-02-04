#pragma once
#include "cyPoint.h"
class Light
{
public:
	cyPoint3d pos;
	cyPoint3d color;
	cyPoint3d dir;

	Light(cyPoint3d _pos, cyPoint3d _color, cyPoint3d _dir = { 0.0, 0.0, 0.0 });
};

class DirectionalLight : public Light {};

class PointLight : public Light {};

class AreaLight
{
public:
	cyPoint3d pos;
	cyPoint3d dir;
	cyPoint3d updir;
	cyPoint3d color;
	cyPoint3d n2, n0, n1;
	double scaleX, scaleY;
	int pxmax, pymax;
	int pxsub, pysub;

	AreaLight(cyPoint3d _pos, cyPoint3d _color, cyPoint3d _dir, cyPoint3d _updir,
		double _scaleX = 10, double _scaleY = 10, int _pxmax = 8, int _pymax = 8,
		int _pxsub = 2, int _pysub = 2);
};