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

//class DirectionalLight : public Light
//{
//
//};
//
//class PointLight : public Light
//{
//
//};
//
//class AreaLight : public Light
//{
//
//};