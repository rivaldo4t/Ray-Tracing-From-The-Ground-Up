#pragma once
#include "cyPoint.h"

class Camera
{
public:
	cyPoint3d pos, viewDir, upDir;
	double scaleX, scaleY, focalLength;
	cyPoint3d n0, n1, n2;
	cyPoint3d viewPortCenter, viewPortBottomLeft;

	Camera(cyPoint3d _camPos, cyPoint3d _viewDir, cyPoint3d _upDir,
		double sx, double sy, double f) :
		pos(_camPos), viewDir(_viewDir), upDir(_upDir),
		scaleX(sx), scaleY(sy), focalLength(f) 
		{
			n2 = viewDir.GetNormalized();
			n0 = viewDir.Cross(upDir).GetNormalized();
			n1 = n0.Cross(n2).GetNormalized();

			viewPortCenter = pos + n2 * focalLength;
			viewPortBottomLeft = viewPortCenter - n0 * (scaleX / 2.0) - n1 * (scaleY / 2.0);
		}
};

class AreaCamera
{
public:
	cyPoint3d pos, dir, updir;
	cyPoint3d n0, n1, n2;
	cyPoint3d eyeBottomLeft;
	double scaleX, scaleY;
	int pxmax, pymax;
	int pxsub, pysub;
	double focalLength;

	AreaCamera(cyPoint3d _pos, cyPoint3d _dir, cyPoint3d _updir,
		double _scaleX = 0, double _scaleY = 0,
		int _pxmax = 1, int _pymax = 1,
		int _pxsub = 1, int _pysub = 1, double _f = 5) : pos(_pos), dir(_dir), updir(_updir),
		scaleX(_scaleX), scaleY(_scaleY), pxmax(_pxmax), pymax(_pymax), pxsub(_pxsub), pysub(_pysub),
		focalLength(_f)
	{
		n2 = dir.GetNormalized();
		n0 = dir.Cross(updir).GetNormalized();
		n1 = n0.Cross(n2).GetNormalized();

		eyeBottomLeft = pos - n0 * (scaleX / 2.0) - n1 * (scaleY / 2.0);
	}
};