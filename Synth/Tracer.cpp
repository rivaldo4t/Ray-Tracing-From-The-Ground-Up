#include <GL/glut.h>
#include <vector>
#include <string>
#include <iostream>
#include <math.h>
#include <algorithm>
#include "Tracer.h"
#include "OBJ_Loader.h"
using namespace std;

//#define ANTI_ALIASED
//#define OUTPUT_JPG "output/out.jpg"
//#define OBJFILE "objects/cube.obj"

void renderScene()
{
	cyPoint3d color, pix, camToPix;
	double X, Y, x, y, rx, ry;
	int subPixX, subPixY;
#if 1
	cyPoint3d eye(0, 0, 6);
	cyPoint3d view(0, 0, -1);
	cyPoint3d up(0, 1, 0);
#else
	cyPoint3d eye(-3, 1.5, 4);
	cyPoint3d view(1, 0, -1);
	cyPoint3d up(0, 1, 0);
	/*cyPoint3d eye(0, 4, 4);
	cyPoint3d view(0, -1, -1);
	cyPoint3d up(0, 1, -1);*/
#endif
	rotVec(view, up, rotX);
	eye += rotY*view;

	Camera cam = {eye, view, up, 10, 10, 8};

	vector<Light> lights = {	{{ -10,  10, -10 }, { 0.5, 0.5, 0.5 }}, 
								{{  10,  10, -10 }, { 0.5, 0.5, 0.5 }},
								{{  0,  10, -10 }, { 0.5, 0.5, 0.5 }},
								{{  0, -10,  0 }, { 0.5, 0.5, 0.5 }},
								{{  0,  10,  0 }, { 0.5, 0.5, 0.5 }},
								{{ -10,  10,  0 }, { 0.5, 0.5, 0.5 }},
								{{ 10,  10,  0 }, { 0.5, 0.5, 0.5 }},
								{{ -10, -10, -10 }, { 0.5, 0.5, 0.5 }},
								{{  0,  0,  10 }, { 0.5, 0.5, 0.5 }},
								{{ -10,  0,  0 }, { 0.5, 0.5, 0.5 }},
	};
	
	AreaLight areaLight({ 0, 10, -2 }, { 1, 1, 1 }, { 0, -1, 0 }, { 0, 0, 1 });
	vector<AreaLight> areaLights = { areaLight };

	//Camera proj = { { 0, 4, 10 },{ 0, 0, -1 },{ 0, 1, 0 }, 9, 9, 10 };
	Camera proj = { { 0, 20, -4 },{ 0, -1, 0 },{ 0, 0, -1 }, 9, 9, 10 };
	cyPoint3d solidColor;

	vector<Quadric> quadrics;
	vector<cyPoint3d> N = { { 0, 0, -1 },{ -1, 0, 0 },{ 0, -1, 0 } };
	vector<pair<double, cyPoint3d>> colors = { { 0.05, { 1, 1, 1 } }, { 0.5, _725E9C }, { 0.0, _725E9C }, { 0.0 ,{ 1, 1, 1 } } };
	Quadric infSphere({ 1, 1, 1 }, 0, -1, { 0, 0, 0 }, { 1, 1, 1 }, N, colors, Tex_env, Null_image);
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 0, 0, -1 }, { 2, 2, 2 }, N, colors, Tex_sphere, Null_image, {}, {}, 1.0));
	//quadrics.push_back(planeFromPoints({ -10, -1, 10 }, { -10, -1, -10 }, { 10, -1, -10 }, palette[(10) % palette.size()], { 0, 0 }, { 0, 1 }, { 1, 1 }, Tex_plane_2, 0.0));
	//quadrics.push_back(planeFromPoints({ -10, -1, 10 }, { 10, -1, -10 }, { 10, -1, 10 }, palette[(10) % palette.size()], { 0, 0 }, { 1, 1 }, { 1, 0 }, Tex_plane_2, 0.0));
	//quadrics.push_back(planeFromPoints({ -10, -10, -7 }, { -10, 10, -7 }, { 10, 10, -7 }, palette[(11) % palette.size()], { 0, 0 }, { 0, 1 }, { 1, 1 }, Tex_plane, 1.0));
	//quadrics.push_back(planeFromPoints({ -10, -10, -7 }, { 10, 10, -7 }, { 10, -10, -7 }, palette[(11) % palette.size()], { 0, 0 }, { 1, 1 }, { 1, 0 }, Tex_plane, 1.0));
	//quadrics[0].refractive_index = 1 + rotY / 10.0;

#ifdef OBJFILE
	objl::Loader Loader;
	bool loadout = Loader.LoadFile(OBJFILE);
	if (loadout)
	{
		for (unsigned int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = Loader.LoadedMeshes[i];
			
			for (unsigned int j = 0; j < curMesh.Indices.size(); j += 3)
			{
				cyPoint3d q1 = { curMesh.Vertices[curMesh.Indices[j]].Position.X, curMesh.Vertices[curMesh.Indices[j]].Position.Y, curMesh.Vertices[curMesh.Indices[j]].Position.Z };
				cyPoint3d q2 = { curMesh.Vertices[curMesh.Indices[j + 1]].Position.X, curMesh.Vertices[curMesh.Indices[j + 1]].Position.Y, curMesh.Vertices[curMesh.Indices[j + 1]].Position.Z };
				cyPoint3d q3 = { curMesh.Vertices[curMesh.Indices[j + 2]].Position.X, curMesh.Vertices[curMesh.Indices[j + 2]].Position.Y, curMesh.Vertices[curMesh.Indices[j + 2]].Position.Z };
				cyPoint2d t1 = { curMesh.Vertices[curMesh.Indices[j]].TextureCoordinate.X, curMesh.Vertices[curMesh.Indices[j]].TextureCoordinate.Y };
				cyPoint2d t2 = { curMesh.Vertices[curMesh.Indices[j + 1]].TextureCoordinate.X, curMesh.Vertices[curMesh.Indices[j + 1]].TextureCoordinate.Y };
				cyPoint2d t3 = { curMesh.Vertices[curMesh.Indices[j + 2]].TextureCoordinate.X, curMesh.Vertices[curMesh.Indices[j + 2]].TextureCoordinate.Y };
				cyPoint3d c = { curMesh.MeshMaterial.Kd.X, curMesh.MeshMaterial.Kd.Y, curMesh.MeshMaterial.Kd.Z };
				c = c.IsZero() ? palette[(j) % palette.size()] : c;

				quadrics.push_back(planeFromPoints(q1 / 0.5, q2 / 0.5, q3 / 0.5, c, t1, t2, t3, Tex_plane_2, 1.0));
			}
		}
	}
#endif

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

#ifndef ANTI_ALIASED
			rx = 0.5;
			ry = 0.5;
			subPixX = 1;
			subPixY = 1;
#else
			rx = (rand() % 10) / 10.0;
			ry = (rand() % 10) / 10.0;
			subPixX = 4;
			subPixY = 4;
#endif

			double weighted = 1.0 / (subPixX * subPixY);
			color = { 0, 0, 0 };

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
					color += castRays(cam.pos, camToPix, quadrics, lights, infSphere, 1) * weighted;
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

#ifdef OUTPUT_JPG
	Image :: writeImage(OUTPUT_JPG, frameBuffer);
#endif
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