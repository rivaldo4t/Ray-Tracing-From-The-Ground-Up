﻿#include <GL/glut.h>
#include <vector>
#include <string>
#include <iostream>
#include <math.h>
#include <algorithm>
#include "cyPoint.h"
#include "cyCore.h"
#include "Quadric.h"
#include "Light.h"
#include "Colors.h"
#include "Image.h"
#include "Common.h"
#include "Camera.h"
#include "Tracer.h"
using namespace std; 

//#define SUBSURFACESSCATTERING
//#define AREALIGHT
//#define ANTI_ALIASED

#ifndef ANTI_ALIASED
int subPixX = 1, subPixY = 1;
#else
int subPixX = 4, subPixY = 4;
#endif

void renderScene()
{
	Image I1("tex.jpg");
	Image I2("tex2.png");
	Image I3("tex7.jpg");
	Image I4("tex9.jpg");

#if 1
	cyPoint3d eye(0, 4, 8);
	cyPoint3d view(0, 0, -1);
	cyPoint3d up(0, 1, 0);
#else
	cyPoint3d eye(0, 4, 0);
	cyPoint3d view(0, -1, -1);
	cyPoint3d up(0, 1, -1);
#endif
	rotVec(view, up, rotX);
	eye += rotY*view;

	Camera cam = {eye, view, up, 10, 10, 8};

	cyPoint3d color, pix, camToPix, hitPoint, normalAtHit;
	double X, Y, x, y, rx, ry, hitParam, hitParamTemp;
	double weighted = 1.0 / (subPixX * subPixY);
	
	vector<Quadric> quadrics;
	vector<cyPoint3d> N = { { 0, 0, -1 },{ -1, 0, 0 },{ 0, -1, 0 } };
	vector<pair<double, cyPoint3d>> colors = { { 0.05, { 1, 1, 1 } }, { 50, _725E9C }, { 3, _725E9C }, { 0.0 ,{ 1, 1, 1 } } };
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 0, 4, -6 }, { 2, 2, 2 }, N, colors, I1));

	N = { { 1, 0 ,0 },{ 0, 0, 1 },{ 0, 1, 0 } };
	colors = { { 0.05, { 1, 1, 1 } }, { 50, _72828F }, { 0.0, _72828F }, { 0.0, { 1, 1, 1 } } };
	quadrics.push_back(Quadric({ 0, 0, 0 }, 1, 0, { 0, -0.5, 0 }, { 0.20, 0.20, 0.20 }, N, colors, I2));

	N = { {1, 0 ,0}, {0, 1, 0}, { 0, 0, 1 } };
	quadrics.push_back(Quadric({ 0, 0, 0 }, 1, 0, { 0, 0, -16 }, {2, 2, 2}, N, colors, I2));

	N = { { 0, 0, 1 },{ -1, 0, 0 },{ 0, -1, 0 } };
	Quadric infSphere({ 1, 1, 1 }, 0, -1, { 0, 0, 0 }, { 1, 1, 1 }, N, colors, I4);

	cyPoint3d lightPos(-8, 8, 0);
	cyPoint3d lightColor(0.8, 0.8, 0.8);
	cyPoint3d spotLightDir(0, -1, -1);
	spotLightDir.Normalize();
	vector<Light> lights = { { lightPos, lightColor, spotLightDir }, { { 0, 8, 0 },{ 0.7, 0.7, 0.7 } } };
	
	AreaLight areaLight({ 0, 10, -2 }, { 1, 1, 1 }, { 0, -1, 0 }, { 0, 0, 1 });
	vector<AreaLight> areaLights = { areaLight };

	cyPoint3d hitPointToLight, lightReflect, camToHitPoint;
	int objIndex;
	double temp;
	cyPoint3d colorTemp;
	double spotLightComp;
	double pointToLightDist;
	bool isInShadow;
	double d = 0.1;
	double r;
	cyPoint3d subSurfacePoint, subTolight;
	double subToLightDist;

	cout << "Navigate using ARROW KEYS ...\n\n";

	for (int i = 0; i < Xmax; i++)
	{
		if (i % 100 == 0)
			cout << "Rendering..." << int((float(i) / Xmax) * 100) << "%\n";

		for (int j = 0; j < Ymax; j++)
		{
			color = { 0,0,0 };
#ifndef ANTI_ALIASED
			rx = 0.5;
			ry = 0.5;
#else
			rx = (rand() % 10) / 10.0;
			ry = (rand() % 10) / 10.0;
#endif
			for (int p = 0; p < subPixX; p++)
			{
				for (int q = 0; q < subPixY; q++)
				{
					X = i + (p + rx) / subPixX;
					Y = j + (q + ry) / subPixY;
					x = X / Xmax;
					y = Y / Ymax;
					pix = cam.viewPortBottomLeft + cam.scaleX * cam.n0 * x + cam.scaleY * cam.n1 * y;
					camToPix = (pix - cam.pos).GetNormalized();
					objIndex = -1;
					hitParam = farPlane;

					for (unsigned int index = 0; index < quadrics.size(); index++)
					{
						hitParamTemp = quadrics[index].intersect(cam.pos, camToPix);
						if (hitParamTemp < hitParam)
						{
							hitParam = hitParamTemp;
							objIndex = index;
						}
					}

					if (objIndex == -1)
					{
						color = infSphere.computeTextureColor(hitPoint, camToPix);
					}
					else
					{
						colorTemp = { 0,0,0 };
						Quadric q = quadrics[objIndex];
						hitPoint = cam.pos + camToPix * hitParam;

#ifdef AREALIGHT
						for(unsigned int a = 0; a < areaLights.size(); a++)
							colorTemp += computeColorFromAreaLight(hitPoint, camToPix, areaLights[a], objIndex, q, quadrics);
#else
#ifndef SUBSURFACESSCATTERING
						// normal shadow ray color computation
						for (unsigned int lightIndex = 0; lightIndex < lights.size(); lightIndex++)
						{
							lightPos = lights[lightIndex].pos;
							lightColor = lights[lightIndex].color;
							spotLightDir = lights[lightIndex].dir;
							hitPointToLight = lightPos - hitPoint;
							pointToLightDist = hitPointToLight.Length();
							hitPointToLight.Normalize();
							camToHitPoint = camToPix;
							normalAtHit = q.normalAtHitPoint(hitPoint);

							isInShadow = shadowRay(objIndex, hitPoint, hitPointToLight, pointToLightDist, quadrics);
							spotLightComp = (spotLightDir.x == 0 && spotLightDir.y == 0 && spotLightDir.z == 0) ? 1.0 :
								clamp(spotLightDir.Dot(-hitPointToLight), 0.5, 0.51);

							colorTemp += q.computeAmbientColor();

							if (isInShadow == false)
							{
								colorTemp += q.computeDiffuseColor(normalAtHit, hitPointToLight, lightColor, pointToLightDist, spotLightComp);
								colorTemp += q.computeSpecularColor(normalAtHit, hitPointToLight, lightColor, camToHitPoint, spotLightComp);
								colorTemp += q.computeBorderColor(normalAtHit, camToHitPoint, spotLightComp);
								//colorTemp = q.computeTextureColor(hitPoint, normalAtHit);
							}
						}
#else
						// sub surface scattering
						for (unsigned int lightIndex = 0; lightIndex < lights.size(); lightIndex++)
						{
							lightPos = lights[lightIndex].pos;
							lightColor = lights[lightIndex].color;
							spotLightDir = lights[lightIndex].dir;
							hitPointToLight = lightPos - hitPoint;
							pointToLightDist = hitPointToLight.Length();
							hitPointToLight.Normalize();
							camToHitPoint = camToPix;
							normalAtHit = q.normalAtHitPoint(hitPoint);

							spotLightComp = (spotLightDir.x == 0 && spotLightDir.y == 0 && spotLightDir.z == 0) ? 1.0 :
								clamp(spotLightDir.Dot(-hitPointToLight), 0.5, 0.51);

							d = 0.1;
							r = 0;
							subSurfacePoint = hitPoint - (d)*normalAtHit;
							subTolight = lightPos - subSurfacePoint;
							subToLightDist = subTolight.Length();
							subTolight.Normalize();
							if (subTolight.Dot(normalAtHit) < d)
								isInShadow = true;
							else
								isInShadow = false;

							colorTemp += q.computeAmbientColor();
							//if (isInShadow == false)
							{
								for (unsigned int qi = 0; qi < quadrics.size(); qi++)
								{
									r += quadrics[qi].intersect_length(subSurfacePoint, subTolight, subToLightDist);
								}

								colorTemp += q.computeDiffuseColor(normalAtHit, hitPointToLight, lightColor, pointToLightDist, spotLightComp, d / r);
								colorTemp += q.computeSpecularColor(normalAtHit, hitPointToLight, lightColor, camToHitPoint, spotLightComp);
								colorTemp += q.computeBorderColor(normalAtHit, camToHitPoint, spotLightComp);
							}
						}
#endif
#endif
						color += colorTemp * weighted;
					}
				}
			}

			frameBuffer[j][i][0] = (float)color[0];
			frameBuffer[j][i][1] = (float)color[1];
			frameBuffer[j][i][2] = (float)color[2];

			for (int k = 0; k <= 2; k++)
			{
				if (frameBuffer[j][i][k] < 0.0)
					frameBuffer[j][i][k] = 0.0;
				else if (frameBuffer[j][i][k] > 1.0)
					frameBuffer[j][i][k] = 1.0;
			}
		}
	}

	glDrawPixels(Xmax, Ymax, GL_RGB, GL_FLOAT, frameBuffer);
	glFlush();
	cout << "Rendering...100%.\nDone.\n\n";

	//Image :: writeImage("image_file.jpg", frameBuffer);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(400, 50);
	glutInitWindowSize(Xmax, Ymax);
	glutCreateWindow("----Synthesized----");
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(quit);
	glutSpecialFunc(keyRot);
	glutMainLoop();

	return 0;
}