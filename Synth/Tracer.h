#pragma once
#include <iostream>
#include "cyPoint.h"
#include "cyCore.h"
#include "Quadric.h"
#include "Light.h"
#include "Colors.h"
#include "Image.h"
#include "Common.h"
#include "Camera.h"
using namespace std;

Image I1("tex.jpg");
Image I2("tex13.jpg");
Image I3("tex7.jpg");
Image I4("tex9.jpg");
Image I5("tex6.jpg");

double rotX = 0.0, rotY = 0.0;
const int Xmax = 600, Ymax = 600;
double farPlane = 100;
float frameBuffer[Ymax][Xmax][3] = { 0 };

cyPoint3d color, pix, camToPix, hitPoint, normalAtHit;
double X, Y, x, y, rx, ry;
double hitParam, hitParamTemp;

cyPoint3d lightPos, lightColor, spotLightDir;
cyPoint3d hitPointToLight, lightReflect, camToHitPoint;
cyPoint3d colorTemp;
cyPoint3d subSurfacePoint, subTolight;
int objIndex;
bool isInShadow;
double spotLightComp, pointToLightDist, subToLightDist;
double d, r;

inline void keyRot(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		rotY -= 1.0;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		rotY += 1.0;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		rotX -= 5.0;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		rotX += 5.0;
		glutPostRedisplay();
		break;
	}
}

inline void rotVec(cyPoint3d& v1, cyPoint3d v2, double degAngle)
{
	v2.Normalize();
	double radAngle = degAngle * cy::cyPi<double>() / 180;
	double cos_theta = cos(radAngle);
	double sin_theta = sin(radAngle);
	v1 = (v1 * cos_theta) + (v2.Cross(v1) * sin_theta) + (v2 * v2.Dot(v1) * (1 - cos_theta));
	//v1.Normalize();
}

void quit(unsigned char key, int x, int y)
{
	if (key == 27) exit(0);
}

inline bool shadowRay(int& objIndex, cyPoint3d& hitPoint, cyPoint3d& hitPointToLight, double& pointToLightDist, vector<Quadric>& quadrics)
{
	for (unsigned int i = 0; i < quadrics.size(); i++)
	{
		/*if (i == objIndex)
			continue;*/
		double hitParamTemp = quadrics[i].intersect(hitPoint, hitPointToLight);
		if (hitParamTemp < pointToLightDist)
			return true;
	}
	return false;
}

inline cyPoint3d computeColorFromAreaLight(cyPoint3d& hitPoint, cyPoint3d& camToPix, AreaLight& a, int& objIndex, Quadric& q, vector<Quadric>& quadrics)
{
	double pX, pY, px, py, prx, pry;
	cyPoint3d plpix;
	cyPoint3d color, colortemp;
	double pweighted = 1.0 / (a.pxsub * a.pysub);

	cyPoint3d lightPos, lightColor, hitPointToLight, camToHitPoint, normalAtHit;
	double pointToLightDist;
	bool isInShadow;

	color = { 0, 0, 0 };

	for (int pi = 0; pi < a.pxmax; pi++)
	{
		for (int pj = 0; pj < a.pymax; pj++)
		{
			prx = (rand() % 10) / 10.0;
			pry = (rand() % 10) / 10.0;
			for (int psub = 0; psub < a.pxsub; psub++)
			{
				for (int qsub = 0; qsub < a.pysub; qsub++)
				{
					colortemp = { 0, 0, 0 };
					pX = pi + (psub + prx) / a.pxsub;
					pY = pj + (qsub + pry) / a.pysub;
					px = pX / a.pxmax;
					py = pY / a.pymax;
					plpix = a.pos + a.scaleX * px * a.n0 + a.scaleY * py * a.n1;

					lightPos = plpix;
					lightColor = a.color;
					hitPointToLight = lightPos - hitPoint;
					pointToLightDist = hitPointToLight.Length();
					hitPointToLight.Normalize();
					camToHitPoint = camToPix;
					normalAtHit = q.normalAtHitPoint(hitPoint);

					isInShadow = shadowRay(objIndex, hitPoint, hitPointToLight, pointToLightDist, quadrics);

					colortemp += q.computeAmbientColor();

					if (isInShadow == false)
					{
						colortemp += q.computeDiffuseColor(normalAtHit, hitPointToLight, lightColor, pointToLightDist);
						colortemp += q.computeSpecularColor(normalAtHit, hitPointToLight, lightColor, camToHitPoint);
						colortemp += q.computeBorderColor(normalAtHit, camToHitPoint);
					}

					color += colortemp * pweighted * 0.05;
				}
			}
		}
	}

	return color;
}

inline cyPoint3d computeSolidTexture(cyPoint3d& hitPoint, Camera proj, int& objIndex, vector<Quadric>& quadrics, Image& I, int type, int solid)
{
	cyPoint3d nth = type == 0 ? -proj.n2 : proj.pos - hitPoint;
	double nthlength = type == 0 ? INT_MAX : nth.Length();
	nth.Normalize();
	cyPoint3d ph1;
	double dist, distTemp;
	double x1, y1;
	cyPoint3d color(0,0,0);
	bool isInShadow = solid == 0 ? false : shadowRay(objIndex, hitPoint, nth, nthlength, quadrics);
	if (isInShadow == false)
	{
		dist = (proj.viewPortBottomLeft - hitPoint).Dot(proj.n2);
		distTemp = nth.Dot(proj.n2);
		if (distTemp != 0 && dist / distTemp > 0)
		{
			dist /= distTemp;
			ph1 = hitPoint + dist*nth;
			x1 = (ph1 - proj.viewPortBottomLeft).Dot(proj.n0) / proj.scaleX;
			y1 = (ph1 - proj.viewPortBottomLeft).Dot(proj.n1) / proj.scaleY;
			if (x1 > 0 && x1 < 1 && y1 > 0 && y1 < 1)
			{
				x1 -= floor(x1);
				y1 -= floor(y1);
				/*if (x1 < 0)
					x1 = 1 - x1;
				if (y1 < 0)
					y1 = 1 - y1;*/
				x1 *= I.width;
				y1 *= I.height;
				x1 = (int)x1 % I.width;
				y1 = (int)y1 % I.height;
				color = I.texture[y1][x1];
			}
		}
	}
	return color;
}