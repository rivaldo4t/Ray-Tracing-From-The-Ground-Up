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

//#define PROJTEX
//#define SUBSURFACESSCATTERING
//#define AREALIGHT
//#define GLOSSY
//#define TRANSLUCENT

Image Null_image;
Image Tex_env("textures/AdobeStock_space_3.2.jpeg");
//Image Tex_plane("textures/AdobeStock_stone_2.jpeg");
//Image Tex_plane_2("textures/tess3_sq.png");
Image Tex_sphere("textures/AdobeStock_space_1.1.jpeg");
Image Tex_sphere_2("textures/AdobeStock_space_2.2.jpeg");
Image Tex_sphere_3("textures/nebula.jpg");
//Image Tex_sphere_4("textures/jupiter.jpg");
//Image Norm_sphere("textures/map.jpg");
//Image Norm_plane("textures/stone.jpg");
Image camera_painting("textures/abs9_960x1080.jpg");

double rotX = 0.0, rotY = 0.0;
const int Xmax = 960, Ymax = 1080;
//const int Xmax = 600, Ymax = 600;
double farPlane = 100;
float frameBuffer[Ymax][Xmax][3] = { 0 };
AreaLight areaLight({ 0, 10, -2 }, { 1, 1, 1 }, { 0, -1, 0 }, { 0, 0, 1 });
vector<AreaLight> areaLights = { areaLight };
double animParam = 0, animParam2 = 0;
double cp_x = 0, cp_y = 0, cp_z = 0, cp_r, cp_g, cp_b;

inline void keyRot(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		rotY -= 1.0;
		//rotY -= 0.1;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		rotY += 1.0;
		//rotY += 0.1;
		//lights.push_back({ { 0, 10,  0 },{ 0.5, 0.5, 0.5 },{ 0, -10, 0 } });
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		rotX -= 5.0;
		//rotX -= 0.1;
		//lights.push_back({ { -10, 10,  0 },{ 0.5, 0.5, 0.5 },{ 10, -10, 0 } });
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		rotX += 5.0;
		//rotX += 0.1;
		//lights.push_back({ { 10, 10,  0 },{ 0.5, 0.5, 0.5 },{ -10, -10, 0 } });
		glutPostRedisplay();
		break;
	}
}

cyPoint3d projectPoint(cyPoint3d p, cyPoint3d o, double d)
{
	return p;
	cyPoint3d newP;
	cyPoint3d ray = p - o;
	double dist = ray.Length();
	newP = o + d * dist * ray.GetNormalized();
	return newP;
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
				color = I.texture[(int)y1][(int)x1];
			}
		}
	}
	return color;
}

inline cyPoint3d castRays(cyPoint3d pos, cyPoint3d dir, vector<Quadric>& quadrics, vector<Light>& lights, Quadric& infSphere, int bounce)
{
	cyPoint3d hitPoint, normalAtHit, reflectedAtHit;
	cyPoint3d lightPos, lightColor, spotLightDir;
	cyPoint3d hitPointToLight, lightReflect, camToHitPoint;
	cyPoint3d colorTemp;
	cyPoint3d subSurfacePoint, subTolight;
	int objIndex;
	bool isInShadow = false;
	double spotLightComp, pointToLightDist, subToLightDist;
	double hitParam, hitParamTemp;
	double d, r;
	int maxBounces = 5;
	cyPoint3d refractedAtHit, recurRay;

	objIndex = -1;
	hitParam = farPlane;
	cyPoint3d color = {0, 0, 0};

	for (unsigned int index = 0; index < quadrics.size(); index++)
	{
		hitParamTemp = quadrics[index].intersect(pos, dir);
		if (hitParamTemp < hitParam)
		{
			hitParam = hitParamTemp;
			objIndex = index;
		}
	}

	if (objIndex == -1)
	{
		// take care of hitpoint
		//color = { 0.4, 0.4, 0.4 };
		color = infSphere.computeTextureColor(hitPoint, dir, true);
	}
	else
	{
		colorTemp = { 0,0,0 };
		Quadric q = quadrics[objIndex];
		hitPoint = pos + dir * hitParam;
		camToHitPoint = dir;
		
		normalAtHit = q.normalAtHitPoint(hitPoint);
		if ((-dir).Dot(normalAtHit) < 0)
			normalAtHit = -normalAtHit;
		
		//q.computeTextureColor(hitPoint, normalAtHit);

		double cosTheta = dir.Dot(normalAtHit);
		reflectedAtHit = (dir - 2 * (cosTheta * normalAtHit)).GetNormalized();
		double nu = q.refractive_index;

#ifdef GLOSSY
		cyPoint3d randomVec(rand() % 10 / 10.0, rand() % 10 / 10.0, rand() % 10 / 10.0);
		reflectedAtHit += randomVec*0.36;
		reflectedAtHit.Normalize();
#endif

		// handle better
		if (nu == 0.0)
			recurRay = reflectedAtHit;
		else
		{
			double refracTerm = ((cosTheta * cosTheta) - 1) / (nu * nu) + 1;
			if (refracTerm > 0)
			{
				refractedAtHit = (dir / nu - (cosTheta / nu + sqrt(refracTerm)) * normalAtHit).GetNormalized();
#ifdef TRANSLUCENT
				cyPoint3d randomVec2(rand() % 10 / 10.0, rand() % 10 / 10.0, rand() % 10 / 10.0);
				refractedAtHit += randomVec*0.16;
				refractedAtHit.Normalize();
#endif
				recurRay = refractedAtHit;
			}
			else
				recurRay = reflectedAtHit;
		}
		
		q.computeTextureColor(hitPoint, normalAtHit);

#ifdef AREALIGHT
		for (unsigned int a = 0; a < areaLights.size(); a++)
			colorTemp += computeColorFromAreaLight(hitPoint, dir, areaLights[a], objIndex, q, quadrics);
#else
		for (unsigned int lightIndex = 0; lightIndex < lights.size(); lightIndex++)
		{
			lightPos = lights[lightIndex].pos;
			lightColor = lights[lightIndex].color;
			spotLightDir = lights[lightIndex].dir;
			hitPointToLight = lightPos - hitPoint;
			pointToLightDist = hitPointToLight.Length();
			hitPointToLight.Normalize();

			spotLightComp = spotLightDir.IsZero() ? 1.0 : clamp(spotLightDir.GetNormalized().Dot(-hitPointToLight), 0.9, 0.91);
			colorTemp += q.computeAmbientColor();

#ifndef SUBSURFACESSCATTERING
			isInShadow = shadowRay(objIndex, hitPoint, hitPointToLight, pointToLightDist, quadrics);
			if (isInShadow == false)
			{
				colorTemp += q.computeDiffuseColor(normalAtHit, hitPointToLight, lightColor, pointToLightDist, spotLightComp);
				colorTemp += q.computeSpecularColor(normalAtHit, hitPointToLight, lightColor, camToHitPoint, spotLightComp);
				colorTemp += q.computeBorderColor(normalAtHit, camToHitPoint, spotLightComp);
			}
#ifdef PROJTEX
			// type(second last parameter) = 0 - parallel
			// type = 1 - perspective
			// solid(last parameter) = 0 - solid texturing
			// solid = 1 - light shading
			solidColor = computeSolidTexture(hitPoint, proj, objIndex, quadrics, I5, 0, 0);
			colorTemp = solidColor.IsZero() ? colorTemp : solidColor;
#endif
#else
			d = 0.1;
			r = 0;
			subSurfacePoint = hitPoint - (d)*normalAtHit;
			subTolight = lightPos - subSurfacePoint;
			subToLightDist = subTolight.Length();
			subTolight.Normalize();
			isInShadow = subTolight.Dot(normalAtHit) < d ? true : false;

			//if (isInShadow == false)
			//{
			for (unsigned int qi = 0; qi < quadrics.size(); qi++)
			{
				r += quadrics[qi].intersect_length(subSurfacePoint, subTolight, subToLightDist);
			}

			colorTemp += q.computeDiffuseColor(normalAtHit, hitPointToLight, lightColor, pointToLightDist, spotLightComp, d / (r));
			colorTemp += q.computeSpecularColor(normalAtHit, hitPointToLight, lightColor, camToHitPoint, spotLightComp);
			colorTemp += q.computeBorderColor(normalAtHit, camToHitPoint, spotLightComp);
			//}
#endif
		}
#endif
		if (bounce < maxBounces)
		{
			color += (1.0 - q.reflectivity) * colorTemp;
			color += q.reflectivity * castRays(hitPoint, recurRay, quadrics, lights, infSphere, bounce + 1);
		}
		else
			color += colorTemp;
	}
	return color;
}

inline Quadric planeFromPoints(cyPoint3d p0, cyPoint3d p1, cyPoint3d p2, cyPoint3d c, cyPoint2d t1, cyPoint2d t2, cyPoint2d t3, Image& I, Image& N, double refl = 0.0, double refr = 0.0)
{
	cyPoint3d planarVec1 = p1 - p0;
	cyPoint3d planarVec2 = p2 - p0;
	cyPoint3d n2 = planarVec1.Cross(planarVec2).GetNormalized();
	cyPoint3d n0 = planarVec1.GetNormalized();
	cyPoint3d n1 = n0.Cross(n2).GetNormalized();
	vector<pair<double, cyPoint3d>> colors = { { 0.01, c },{ 0.5, c },{ 0.0, c },{ 0.0,{ 1, 1, 1 } } };
	return Quadric({ 0, 0, 0 }, 1, 0, (p0 + p1 + p2) / 3, { 1, 1, 1 }, { n0, n1, n2 }, colors, I, N, { p0, p1, p2 }, { t1, t2, t3 }, refl, refr);
}