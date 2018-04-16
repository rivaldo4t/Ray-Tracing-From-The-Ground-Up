#pragma once
#include "cyPoint.h"
class Light
{
public:
	cyPoint3d pos;
	cyPoint3d color;
	cyPoint3d dir;

	Light(cyPoint3d _pos, cyPoint3d _color, cyPoint3d _dir = { 0.0, 0.0, 0.0 }) : pos(_pos), color(_color), dir(_dir) {}
};

class DirectionalLight : public Light
{

};

class PointLight : public Light
{

};

class AreaLight
{
public:
	cyPoint3d pos;
	cyPoint3d color;
	cyPoint3d dir;
	cyPoint3d updir;
	cyPoint3d n2;
	cyPoint3d n0;
	cyPoint3d n1;
	double scaleX = 10, scaleY = 10;
	int pxmax = 8, pymax = 8;
	int pxsub = 2, pysub = 2;

	AreaLight(cyPoint3d _pos, cyPoint3d _color, cyPoint3d _dir, cyPoint3d _updir,
	double _scaleX = 10, double _scaleY = 10, 
	int _pxmax = 8, int _pymax = 8,
	int _pxsub = 2, int _pysub = 2) : pos(_pos), color(_color), dir(_dir), updir(_updir), 
	scaleX(_scaleX), scaleY(_scaleY), pxmax(_pxmax), pymax(_pymax), pxsub(_pxsub), pysub(_pysub)
	{
		n2 = dir.GetNormalized();
		n0 = dir.Cross(updir).GetNormalized();
		n1 = n0.Cross(n2).GetNormalized();
	}
};