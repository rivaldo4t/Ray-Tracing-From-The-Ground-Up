#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <algorithm>
# if 0
#include "cyPoint.h"
#include "Quadric.hpp"
#include "Light.hpp"
#include "Image.hpp"
#include "Camera.hpp"
#include "Utility.hpp"
using namespace std;

bool shadowRay(int& objIndex, cyPoint3d& hitPoint, cyPoint3d& hitPointToLight, double& pointToLightDist, vector<Quadric>& quadrics);
cyPoint3d computeColorFromAreaLight(cyPoint3d& hitPoint, cyPoint3d& camToPix, AreaLight& a, int& objIndex, Quadric& q, vector<Quadric>& quadrics);
cyPoint3d computeSolidTexture(cyPoint3d& hitPoint, Camera proj, int& objIndex, vector<Quadric>& quadrics, Image& I, int type, int solid);
Quadric planeFromPoints(cyPoint3d p0, cyPoint3d p1, cyPoint3d p2, cyPoint3d c, cyPoint2d t1, cyPoint2d t2, cyPoint2d t3, Image& I, Image& N, double refl = 0.0, double refr = 0.0);
cyPoint3d castRays(cyPoint3d pos, cyPoint3d dir, vector<Quadric>& quadrics, vector<Light>& lights, Quadric& infSphere, int bounce);
#endif