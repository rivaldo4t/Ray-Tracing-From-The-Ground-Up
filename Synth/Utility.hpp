#pragma once
#include <iostream>
#include <vector>
#include "cyPoint.h"

extern const int Xmax, Ymax;
extern double farPlane;
extern cyPoint3d _DC304B, _F95F62, _FFFFFF, _1F2D3D, _343F4B, _FFEFCA, _EDA16A, _C83741, _CA4679, _725E9C, _823066, _F4EAC8, _72828F;
extern std::vector<cyPoint3d> palette;

cyPoint3d projectPoint(cyPoint3d p, cyPoint3d o, double d);
void rotVec(cyPoint3d& v1, cyPoint3d v2, double degAngle);