#pragma once
#include "cyPoint.h"
class Light
{
public:
	cyPoint3d pos;
	cyPoint3d color;
	cyPoint3d dir;

	Light(cyPoint3d _pos, cyPoint3d _color, cyPoint3d _dir = { 0,0,0 }) : pos(_pos), color(_color), dir(_dir) {}
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
	int pxsub = 1, pysub = 1;

	AreaLight(cyPoint3d _pos, cyPoint3d _color, cyPoint3d _dir, cyPoint3d _updir) : pos(_pos), color(_color), dir(_dir), updir(_updir) 
	{
		n2 = dir.GetNormalized();
		n0 = dir.Cross(updir).GetNormalized();
		n1 = n0.Cross(n2).GetNormalized();
	}
};