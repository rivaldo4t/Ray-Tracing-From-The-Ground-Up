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