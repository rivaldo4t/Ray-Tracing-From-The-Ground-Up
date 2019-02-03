#include "Utility.hpp"

cyPoint3d _DC304B(220.0 / 255.0, 48.0 / 255.0, 75.0 / 255.0);
cyPoint3d _F95F62(249.0 / 255.0, 95.0 / 255.0, 98.0 / 255.0);
cyPoint3d _FFFFFF(255.0 / 255.0, 255.0 / 255.0, 255.0 / 255.0);
cyPoint3d _1F2D3D(31.0 / 255.0, 45.0 / 255.0, 61.0 / 255.0);
cyPoint3d _343F4B(52.0 / 255.0, 63.0 / 255.0, 75.0 / 255.0);
cyPoint3d _FFEFCA(255.0 / 255.0, 239.0 / 255.0, 202.0 / 255.0);
cyPoint3d _EDA16A(237.0 / 255.0, 161.0 / 255.0, 106.0 / 255.0);
cyPoint3d _C83741(200.0 / 255.0, 55.0 / 255.0, 65.0 / 255.0);
cyPoint3d _CA4679(202.0 / 255.0, 70.0 / 255.0, 121.0 / 255.0);
cyPoint3d _725E9C(14.0 / 255.0, 94.0 / 255.0, 156.0 / 255.0);
cyPoint3d _823066(130.0 / 255.0, 48.0 / 255.0, 102.0 / 255.0);
cyPoint3d _F4EAC8(244.0 / 255.0, 234.0 / 255.0, 200.0 / 255.0);
cyPoint3d _72828F(114.0 / 255.0, 130.0 / 255.0, 143.0 / 255.0);
std::vector<cyPoint3d> palette = { _DC304B, _F95F62, _FFFFFF, _1F2D3D, _343F4B, _FFEFCA, _EDA16A, _C83741, _CA4679, _725E9C, _823066, _F4EAC8, _72828F };
double farPlane = 100;

cyPoint3d projectPoint(cyPoint3d p, cyPoint3d o, double d)
{
	return p;
	cyPoint3d newP;
	cyPoint3d ray = p - o;
	double dist = ray.Length();
	newP = o + d * dist * ray.GetNormalized();
	return newP;
}

void rotVec(cyPoint3d& v1, cyPoint3d v2, double degAngle)
{
	v2.Normalize();
	double radAngle = degAngle * cy::cyPi<double>() / 180;
	double cos_theta = cos(radAngle);
	double sin_theta = sin(radAngle);
	v1 = (v1 * cos_theta) + (v2.Cross(v1) * sin_theta) + (v2 * v2.Dot(v1) * (1 - cos_theta));
	//v1.Normalize();
}