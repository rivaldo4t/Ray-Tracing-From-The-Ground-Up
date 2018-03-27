#include <GL/glut.h>
#include <vector>
#include <string>
#include <iostream>
#include <math.h>
#include <algorithm>
#include "Tracer.h"
#include "OBJ_Loader.h"
using namespace std; 

//#define PROJTEX
//#define SUBSURFACESSCATTERING
//#define AREALIGHT
//#define ANTI_ALIASED

//#define OBJFILE "box_stack.obj"
//#define OBJFILE "earth.obj" // unexpected behaiviour with this file; Try re-running if does not load the first time; Works; checked.
#define OBJFILE "cube.obj"

#ifndef ANTI_ALIASED
int subPixX = 1, subPixY = 1;
#else
int subPixX = 4, subPixY = 4;
#endif

Quadric planeFromPoints(cyPoint3d p0, cyPoint3d p1, cyPoint3d p2, cyPoint3d c, cyPoint2d t1, cyPoint2d t2, cyPoint2d t3, Image& I)
{
	cyPoint3d planarVec1 = p1 - p0;
	cyPoint3d planarVec2 = p2 - p0;
	cyPoint3d n2 = planarVec1.Cross(planarVec2).GetNormalized();
	cyPoint3d n0 = planarVec1.GetNormalized();
	cyPoint3d n1 = n0.Cross(n2).GetNormalized();
	Image temp;
	vector<pair<double, cyPoint3d>> colors = { { 0.01, c },{ 50, c },{ 0.0, c },{ 0.0,{ 1, 1, 1 } } };
	return Quadric({ 0, 0, 0 }, 1, 0, p0, { 1, 1, 1 }, { n0, n1, n2 }, colors, I, temp, { p0, p1, p2 }, {t1, t2, t3});
}

void renderScene()
{
	double weighted = 1.0 / (subPixX * subPixY);

#if 1
	/*cyPoint3d eye(0, 0, 6);
	cyPoint3d view(0, 0, -1);
	cyPoint3d up(0, 1, 0);*/
	cyPoint3d eye(3, 1, 3);
	cyPoint3d view(-1, 0, -1);
	cyPoint3d up(0, 1, 0);
#else
	cyPoint3d eye(0, 4, 4);
	cyPoint3d view(0, -1, -1);
	cyPoint3d up(0, 1, -1);
#endif
	rotVec(view, up, rotX);
	eye += rotY*view;

	Camera cam = {eye, view, up, 10, 10, 8};
	Image temp;
	vector<Quadric> quadrics;
	vector<cyPoint3d> N = { { 0, 0, -1 },{ -1, 0, 0 },{ 0, -1, 0 } };
	vector<pair<double, cyPoint3d>> colors = { { 0.05, { 1, 1, 1 } }, { 50, _725E9C }, { 0.0, _725E9C }, { 0.0 ,{ 1, 1, 1 } } };
	//quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 0, 2, -6 }, { 2, 2, 2 }, N, colors, I1, I3));

	N = { { 1, 0 ,0 },{ 0, 0, 1 },{ 0, 1, 0 } };
	colors = { { 0.05, { 1, 1, 1 } }, { 50, _72828F }, { 0.0, _72828F }, { 0.0, { 1, 1, 1 } } };
	//quadrics.push_back(Quadric({ 0, 0, 0 }, 1, 0, { 0, -0.1, 0 }, { 0.20, 0.20, 0.20 }, N, colors, I2, temp));

	N = { {1, 0 ,0}, {0, 1, 0}, { 0, 0, 1 } };
	//quadrics.push_back(Quadric({ 0, 0, 0 }, 1, 0, { 0, 0, -10 }, {2, 2, 2}, N, colors, I2, temp));

	N = { { 0, 0, 1 },{ -1, 0, 0 },{ 0, -1, 0 } };
	//Quadric infSphere({ 1, 1, 1 }, 0, -1, { 0, 0, 0 }, { 1, 1, 1 }, N, colors, I4, temp);

	vector<Light> lights = {	{{ -8, 8, -8 }, { 0.8, 0.8, 0.8 }}, 
								{{  8, 8, -8 }, { 0.8, 0.8, 0.8 }},
								{ { 0, 8, -8 },{ 0.8, 0.8, 0.8 } },
								{ { 0, -8, 0 },{ 0.8, 0.8, 0.8 } },
								{{  0, 8,  0 }, { 0.8, 0.8, 0.8 }} ,
								{ { -8, 8,  0 },{ 0.8, 0.8, 0.8 } } ,
								{ { 8, 8,  0 },{ 0.8, 0.8, 0.8 } } ,
								{ { -8, -8, -8 },{ 0.8, 0.8, 0.8 } },
								{ { 0, 0,  8 },{ 0.5, 0.5, 0.5 } } ,
								{ { -8, 0,  0 },{ 0.5, 0.5, 0.5 } } ,
	};
	
	AreaLight areaLight({ 0, 10, -2 }, { 1, 1, 1 }, { 0, -1, 0 }, { 0, 0, 1 });
	vector<AreaLight> areaLights = { areaLight };

	Camera proj = { { 0, 20, -4 },{ 0, -1, 0 },{ 0, 0, -1 }, 9, 9, 10 };
	//Camera proj = { { 0, 4, 10 },{ 0, 0, -1 },{ 0, 1, 0 }, 9, 9, 10 };
	cyPoint3d solidColor;

	//tetrahedron
	/*quadrics.push_back(planeFromPoints({ 0, 6, -8 }, { 0, 2, -8 }, { 0, 2, -4 }, palette[(9)%palette.size()], {0, 0}, { 0, 0 }, { 0, 0 }, I6));
	quadrics.push_back(planeFromPoints({ 0, 2, -8 }, { 0, 6, -8 }, { 4, 2, -8 }, palette[(9)%palette.size()], { 0, 0 }, { 0, 0 }, { 0, 0 }, I6));
	quadrics.push_back(planeFromPoints({ 0, 2, -4 }, { 4, 2, -8 }, { 0, 6, -8 }, palette[(9)%palette.size()], { 0, 0 }, { 0, 0 }, { 0, 0 }, I6));
	quadrics.push_back(planeFromPoints({ 4, 2, -8 }, { 0, 2, -4 }, { 0, 2, -8 }, palette[(9)%palette.size()], { 0, 0 }, { 0, 0 }, { 0, 0 }, I6));*/

	//
	quadrics.push_back(planeFromPoints({ -50, -0.45, 50 }, { -50, -0.45, -50 }, { 50, -0.45, -50 }, palette[(2) % palette.size()], { 0, 0 }, {1, 0}, {0, 1}, I7));
	quadrics.push_back(planeFromPoints({ -50, -0.45, 50 }, { 50, -0.45, -50 }, { 50, -0.45, 50 }, palette[(2) % palette.size()], { 0, 0 }, { 0, 1 }, { 1, 1 }, I7));

	objl::Loader Loader;
	bool loadout = Loader.LoadFile(OBJFILE);
	if (loadout)
	{
		for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = Loader.LoadedMeshes[i];

			/*cout << "Mesh " << i << ": " << curMesh.MeshName << "\n";
			cout << "Vertices:\n";

			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{
				cout << "V" << j << ": " <<
					"P(" << curMesh.Vertices[j].Position.X << ", " << curMesh.Vertices[j].Position.Y << ", " << curMesh.Vertices[j].Position.Z << ") " <<
					"N(" << curMesh.Vertices[j].Normal.X << ", " << curMesh.Vertices[j].Normal.Y << ", " << curMesh.Vertices[j].Normal.Z << ") " <<
					"TC(" << curMesh.Vertices[j].TextureCoordinate.X << ", " << curMesh.Vertices[j].TextureCoordinate.Y << ")\n";
			}
			cout << "Indices:\n";*/
			for (int j = 0; j < curMesh.Indices.size(); j += 3)
			{
				//cout << "T" << j / 3 << ": " << curMesh.Indices[j] << ", " << curMesh.Indices[j + 1] << ", " << curMesh.Indices[j + 2] << "\n";
				cyPoint3d q1 = { curMesh.Vertices[curMesh.Indices[j]].Position.X, curMesh.Vertices[curMesh.Indices[j]].Position.Y, curMesh.Vertices[curMesh.Indices[j]].Position.Z };
				cyPoint3d q2 = { curMesh.Vertices[curMesh.Indices[j + 1]].Position.X, curMesh.Vertices[curMesh.Indices[j + 1]].Position.Y, curMesh.Vertices[curMesh.Indices[j + 1]].Position.Z };
				cyPoint3d q3 = { curMesh.Vertices[curMesh.Indices[j + 2]].Position.X, curMesh.Vertices[curMesh.Indices[j + 2]].Position.Y, curMesh.Vertices[curMesh.Indices[j + 2]].Position.Z };
				cyPoint2d t1 = { curMesh.Vertices[curMesh.Indices[j]].TextureCoordinate.X, curMesh.Vertices[curMesh.Indices[j]].TextureCoordinate.Y };
				cyPoint2d t2 = { curMesh.Vertices[curMesh.Indices[j + 1]].TextureCoordinate.X, curMesh.Vertices[curMesh.Indices[j + 1]].TextureCoordinate.Y };
				cyPoint2d t3 = { curMesh.Vertices[curMesh.Indices[j + 2]].TextureCoordinate.X, curMesh.Vertices[curMesh.Indices[j + 2]].TextureCoordinate.Y };
				quadrics.push_back(planeFromPoints(q1, q2, q3, palette[(9)%palette.size()], t1, t2, t3, I6));
			}
			//cout << "\n";
		}
	}
	//

	cout << "Navigate using ARROW KEYS ...\n";
	for (int space = 0; space < Xmax / 50; space++)
		cout << " ";
	cout << "|\n";

	for (int i = 0; i < Xmax; i++)
	{
		if (i % 50 == 0)
			cout << "#";

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
						hitParamTemp = quadrics[index].intersect2(cam.pos, camToPix);
						if (hitParamTemp < hitParam)
						{
							hitParam = hitParamTemp;
							objIndex = index;
						}
					}

					if (objIndex == -1)
					{
						//color = infSphere.computeTextureColor(hitPoint, camToPix);
					}
					else
					{
						colorTemp = { 0,0,0 };
						Quadric q = quadrics[objIndex];
						hitPoint = cam.pos + camToPix * hitParam;
						camToHitPoint = camToPix;
						normalAtHit = q.normalAtHitPoint(hitPoint);
						q.computeTextureColor(hitPoint, normalAtHit);

#ifdef AREALIGHT
					for(unsigned int a = 0; a < areaLights.size(); a++)
						colorTemp += computeColorFromAreaLight(hitPoint, camToPix, areaLights[a], objIndex, q, quadrics);
#else
						// normal shadow ray color computation
						for (unsigned int lightIndex = 0; lightIndex < lights.size(); lightIndex++)
						{
							lightPos = lights[lightIndex].pos;
							lightColor = lights[lightIndex].color;
							spotLightDir = lights[lightIndex].dir;
							hitPointToLight = lightPos - hitPoint;
							pointToLightDist = hitPointToLight.Length();
							/*hitPointToLight = { 1, 1, 1 };
							pointToLightDist = 10;*/
							hitPointToLight.Normalize();

							spotLightComp = spotLightDir.IsZero() ? 1.0 : clamp(spotLightDir.GetNormalized().Dot(-hitPointToLight), 0.5, 0.51);
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

							colorTemp += q.computeDiffuseColor(normalAtHit, hitPointToLight, lightColor, pointToLightDist, spotLightComp, d / r);
							colorTemp += q.computeSpecularColor(normalAtHit, hitPointToLight, lightColor, camToHitPoint, spotLightComp);
							colorTemp += q.computeBorderColor(normalAtHit, camToHitPoint, spotLightComp);
							//}
#endif
						}
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
	cout << "\nRendering Complete\n\n";

	//Image :: writeImage("texturedBoxes.jpg", frameBuffer);
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