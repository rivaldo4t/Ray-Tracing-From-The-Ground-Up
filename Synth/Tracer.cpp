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
//#define OUTPUT_JPG "output/out"
//#define OBJFILE "objects/dodec.obj"
//#define DEPTH_OF_FIELD
//#define MOTION_BLUR
//#define CAM_PAINTING
cyPoint3d eye(0, 0, 4);
cyPoint3d view(0, 0, -1);
cyPoint3d up(0, 1, 0);
void renderScene()
{
	cyPoint3d color, pix, camToPix;
	double X, Y, x, y, rx, ry;
	int subPixX, subPixY;
	
	//cyPoint3d up(0, 1, 0);
	
	//cyPoint3d eye(0, 0, 10);
	// Stereo configuration
	//eye = int(outCount*20) % 2 == 0 ? cyPoint3d{ 0.2, 0, 5 } : cyPoint3d{ -0.2, 0, 5 };
	//eye = { 0.3, 0, 5 };
	
	//cyPoint3d view(0, 0, -1);
	//point view at (0, 0, 0)
	//view = (cyPoint3d(0, 0, 0) - eye).GetNormalized();
	
	//rotVec(view, up, rotX);
	//eye += rotY * view;
	//animParam = animParam > 6.3 ? 0 : animParam;			//2*pie = 6.3

	Camera cam = {eye, view, up, double(Xmax) / double(Ymax) * 10, 10, 6};

	//vector<Light> lights = {	
	//							{ { 0,  10, 0 },{ 0.5, 0.5, 0.5 } },
	//							{ { 0,  0, 5 },{ 0.5, 0.5, 0.5 } },
	//							{ { -10,  0,  5 },{ 0.5, 0.5, 0.5 } },
	//							{{ 10,  0,  5 }, { 0.5, 0.5, 0.5 }},
	//};
	
	vector<Light> lights = {
		{ { 10 * cos(animParam),  10 * sin(animParam), 0 },{ 0.0, 0.5, 0.5 } },
		{ { -10 * cos(animParam),  0,  10 * sin(animParam) },{ 0.5, 0.0, 0.5 } },
		{ { 0,  10 * cos(animParam),  10 * sin(animParam) },{ 0.5, 0.5, 0.0 } },
	};

	/*AreaLight areaLight({ 0, 10, -2 }, { 1, 1, 1 }, { 0, -1, 0 }, { 0, 0, 1 });
	vector<AreaLight> areaLights = { areaLight };*/

	//Camera proj = { { 0, 4, 10 },{ 0, 0, -1 },{ 0, 1, 0 }, 9, 9, 10 };
	Camera proj = { { 0, 20, -4 },{ 0, -1, 0 },{ 0, 0, -1 }, 9, 9, 10 };
	cyPoint3d solidColor;

#ifndef DEPTH_OF_FIELD
	AreaCamera areaCam(eye, view, up);
#else
	AreaCamera areaCam(eye, view, up, 0.5, 0.5, 4, 4, 1, 1, 8);
#endif

	cyPoint3d eyePos;
	double pX, pY, px, py, prx, pry;
	double psubweighted = 1.0 / (areaCam.pxsub * areaCam.pysub);
	double pmaxweighted = 1.0 / (areaCam.pxmax * areaCam.pymax);
	cyPoint3d pointOnFocalPlane;
	cyPoint3d primaryRay, secondaryRay;

	vector<Quadric> quadrics;
	vector<cyPoint3d> N = { { 0, 0, -1 },{ -1, 0, 0 },{ 0, -1, 0 } };
	vector<pair<double, cyPoint3d>> colors = { { 0.05, { 1, 1, 1 } }, { 0.5, _725E9C }, { 0.0, _725E9C }, { 0.0 ,{ 1, 1, 1 } } };
	vector<pair<double, cyPoint3d>> colors2 = { { 0.05,{ 1, 1, 1 } },{ 0.5, palette[(10) % palette.size()] },{ 0.0, _725E9C },{ 0.0 ,{ 1, 1, 1 } } };
	
	Quadric infSphere({ 1, 1, 1 }, 0, -1, { 0, 0, 0 }, { 1, 1, 1 }, N, colors, Tex_env, Null_image);

	// Camera Painting
	//quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 0, 0, 0 }, { 1, 1, 1 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 0.0));

	// Multiple Objects setup; enable dodec OBJ in addition to this
	/*quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { -4, -0.5, 0 }, { 2, 2, 2 }, N, colors, Null_image, Null_image, {}, {}, 1.0, 0.0));
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, {  2.5, -0.5,  -1.5 }, { 1, 1, 1 }, N, colors, Tex_sphere, Null_image, {}, {}, 0.0, 0.0));
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, {  2.5, -0.5,  1}, { 1, 1, 1 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 0.0));
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, {  -1.5, -0.5,  2.5 }, { 1, 1, 1 }, N, colors, Tex_sphere_3, Null_image, {}, {}, 0.0));
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 0.0, 1.5,  -2.0 }, { 1, 1, 1 }, N, colors, Tex_sphere_2, Null_image, {}, {}, 0.0));
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, {  -2.5, 0.8,  -2 }, { 1, 1, 1 }, N, colors, Tex_sphere_4, Null_image, {}, {}, 0.0));*/
	
	// Depth of field Configuration
	//quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { -1.5, -0.5,  2.5 }, { 1, 1, 1 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 0.0));
	//quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, {    0, -0.5,  0 }, { 1, 1, 1 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 0.0));
	//quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, {  2.5, -0.5, -2.5 }, { 1, 1, 1 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 0.0));

	// Motion in a grid
	/*quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 0 + rotX, -0.5,  0 + rotY }, { 1, 1, 1 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 0.0));
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { -1.7, -0.5,  -1.7 }, { 0.6, 0.6, 0.6 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 0.0));
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 1.7, -0.5,  1.7 },  { 0.6, 0.6, 0.6 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 0.0));
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 1.7, -0.5,  -1.7 }, { 0.6, 0.6, 0.6 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 0.0));
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { -1.7, -0.5,  1.7 }, { 0.6, 0.6, 0.6 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 0.0));*/

	// Orbiting motion
	/*quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 0, 0, 0 }, { 1, 1, 1 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 0.0));
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 3 * cos(animParam), 0, 3 * sin(animParam) }, { 0.4, 0.4, 0.4 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 1.5));
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 0, 2 * cos(animParam + 0.5), 2 * sin(animParam + 0.5) }, { 0.4, 0.4, 0.4 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 1.5));
	cyPoint3d a = cyPoint3d{ 0, 0, -1 }.GetNormalized();
	cyPoint3d b = cyPoint3d{ 1, 1, 0 }.GetNormalized();
	cyPoint3d c = {0, 0, 0};
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { c[0] + 2 * (cos(-animParam + 1) * a[0] + sin(-animParam + 1) * b[0]),
													 c[1] + 2 * (cos(-animParam + 1) * a[1] + sin(-animParam + 1) * b[1]),
													 c[2] + 2 * (cos(-animParam + 1) * a[2] + sin(-animParam + 1) * b[2]) },
													{ 0.4, 0.4, 0.4 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 1.6));*/

	// Stereo
	//quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { -2.5, -0.5,  -2.5 }, { 1, 1, 1 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 0.0));
	//quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 0, -0.5,  0 }, { 1, 1, 1 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 0.0));
	//quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 2.5, -0.5, -2.5 }, { 1, 1, 1 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 0.0));

	/*quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 0, 0, 0 }, { 2, 2, 2 }, N, colors2, Tex_sphere, Null_image, {}, {}, 0.0, 0.0));
	cyPoint3d a = cyPoint3d{ 0, 0, -1 }.GetNormalized();
	cyPoint3d b = cyPoint3d{ 1, 1, 0 }.GetNormalized();
	cyPoint3d center = { 0, 0, 0 };
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { center[0] + 3 * (cos(animParam + 1) * a[0] + sin(animParam + 1) * b[0]),
		center[1] + 3 * (cos(animParam + 1) * a[1] + sin(animParam + 1) * b[1]),
		center[2] + 3 * (cos(animParam + 1) * a[2] + sin(animParam + 1) * b[2]) },
		{ 0.4, 0.4, 0.4 }, N, colors2, Tex_sphere_2, Null_image, {}, {}, 0.0, 0.0));*/

	// Planes
	//quadrics.push_back(planeFromPoints({ -10, -1.5, 10 }, { -10, -1.5, -10 }, { 10, -1.5, -10 }, palette[(10) % palette.size()], { 0, 0 }, { 0, 1 }, { 1, 1 }, Tex_plane, Null_image, 0.0));
	//quadrics.push_back(planeFromPoints({ -10, -1.5, 10 }, { 10, -1.5, -10 }, { 10, -1.5, 10 }, palette[(10) % palette.size()], { 0, 0 }, { 1, 1 }, { 1, 0 }, Tex_plane, Null_image, 0.0));
	//quadrics.push_back(planeFromPoints({ -10, -10, -5 }, { -10, 10, -5 }, { 10, 10, -5 }, palette[(11) % palette.size()], { 0, 0 }, { 0, 1 }, { 1, 1 }, Tex_plane, Null_image, 0.0));
	//quadrics.push_back(planeFromPoints({ -10, -10, -5 }, { 10, 10, -5 }, { 10, -10, -5 }, palette[(11) % palette.size()], { 0, 0 }, { 1, 1 }, { 1, 0 }, Tex_plane, Null_image, 0.0));
	//quadrics[0].refractive_index = 1 + rotY / 10.0;

	// Animation
	//cyPoint3d a, b, c, d;
	//a = projectPoint({ -4, 0, 0 }, eye, 1);
	//b = projectPoint({ -4, 2.5, 0 }, eye, 1);
	//c = projectPoint({ 0, 2.5, 0 }, eye, 1);
	//d = (a + b + c) / 3;
	//rotVec(a, { 0, 0, d[0] }, animParam2 * 5);
	//rotVec(b, { 0, 0, d[0] }, animParam2 * 5);
	//rotVec(c, { 0, 0, d[0] }, animParam2 * 5);
	////rotVec(a, up, rotY * 1);
	////rotVec(b, up, rotY * 1);
	////rotVec(c, up, rotY * 1);
	////cout << rotY << endl;
	//quadrics.push_back(planeFromPoints(a, b, c, palette[(11) % palette.size()], { 0, 0 }, { 0, 1 }, { 1, 1 }, Tex_plane, Null_image, 0.0));
	//
	//a = projectPoint({ -4, 0, 0 }, eye, 1.7);
	//b = projectPoint({ 0, 2.5, 0 }, eye, 1.7);
	//c = projectPoint({ 0, 0, 0 }, eye, 1.7);
	//d = (a + b + c) / 3;
	//rotVec(a, { 0, d[0], 0 }, -animParam2 * 5);
	//rotVec(b, { 0, d[0], 0 }, -animParam2 * 5);
	//rotVec(c, { 0, d[0], 0 }, -animParam2 * 5);
	//quadrics.push_back(planeFromPoints(a, b, c, palette[(11) % palette.size()], { 0, 0 }, { 1, 1 }, { 1, 0 }, Tex_plane, Null_image, 0.0));

	//a = projectPoint({ 0, 0, 0 }, eye, 1.2);
	//b = projectPoint({ 0, 2.5, 0 }, eye, 1.2);
	//c = projectPoint({ 4, 2.5, 0 }, eye, 1.2);
	//d = (a + b + c) / 3;
	//rotVec(a, { d[0], d[0], 0 }, animParam2 * 5);
	//rotVec(b, { d[0], d[0], 0 }, animParam2 * 5);
	//rotVec(c, { d[0], d[0], 0 }, animParam2 * 5);
	//quadrics.push_back(planeFromPoints(a, b, c, palette[(11) % palette.size()], { 0, 0 }, { 0, 1 }, { 1, 1 }, Tex_plane, Null_image, 0.0));

	//a = projectPoint({ 0, 0, 0 }, eye, 0.9);
	//b = projectPoint({ 4, 2.5, 0 }, eye, 0.9);
	//c = projectPoint({ 4, 0, 0 }, eye, 0.9);
	//d = (a + b + c) / 3;
	//rotVec(a, { d[0], 0, 0 }, -animParam2 * 5);
	//rotVec(b, { d[0], 0, 0 }, -animParam2 * 5);
	//rotVec(c, { d[0], 0, 0 }, -animParam2 * 5);
	//quadrics.push_back(planeFromPoints(a, b, c, palette[(11) % palette.size()], { 0, 0 }, { 1, 1 }, { 1, 0 }, Tex_plane, Null_image, 0.0));

	//a = projectPoint({ -1.25, -5, 0 }, eye, 1.8);
	//b = projectPoint({ -1.25, 0, 0 }, eye, 1.8);
	//c = projectPoint({ 1.25, 0, 0 }, eye, 1.8);
	//d = (a + b + c) / 3;
	//rotVec(a, { 0, d[0], d[0] }, animParam2 * 5);
	//rotVec(b, { 0, d[0], d[0] }, animParam2 * 5);
	//rotVec(c, { 0, d[0], d[0] }, animParam2 * 5);
	//quadrics.push_back(planeFromPoints(a, b, c, palette[(11) % palette.size()], { 0, 0 }, { 0, 1 }, { 1, 1 }, Tex_plane, Null_image, 0.0));
	//
	//a = projectPoint({ -1.25, -5, 0 }, eye, 1.4);
	//b = projectPoint({ 1.25, 0, 0 }, eye, 1.4);
	//c = projectPoint({ 1.25, -5, 0 }, eye, 1.4);
	//d = (a + b + c) / 3;
	//rotVec(a, { 0, d[1], 0 }, -animParam2 * 5);
	//rotVec(b, { 0, d[1], 0 }, -animParam2 * 5);
	//rotVec(c, { 0, d[1], 0 }, -animParam2 * 5);
	//quadrics.push_back(planeFromPoints(a, b, c, palette[(11) % palette.size()], { 0, 0 }, { 1, 1 }, { 1, 0 }, Tex_plane, Null_image, 0.0));

	//quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 0, 0, 2 }, { 1, 1, 1 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 0.0));
	//quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 4 * cos(animParam), 0, 4 * sin(animParam) }, { 0.4, 0.4, 0.4 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 1.3));
	//quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 0, 4 * cos(animParam + 0.5), 4 * sin(animParam + 0.5) }, { 0.4, 0.4, 0.4 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 1.6));
	//a = cyPoint3d{ 0, 0, -1 }.GetNormalized();
	//b = cyPoint3d{ 1, 1, 0 }.GetNormalized();
	//c = { 0, 0, 0 };
	//quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { c[0] + 4 * (cos(-animParam + 1) * a[0] + sin(-animParam + 1) * b[0]),
	//	c[1] + 4 * (cos(-animParam + 1) * a[1] + sin(-animParam + 1) * b[1]),
	//	c[2] + 4 * (cos(-animParam + 1) * a[2] + sin(-animParam + 1) * b[2]) },
	//	{ 0.4, 0.4, 0.4 }, N, colors2, Null_image, Null_image, {}, {}, 1.0, 1.9));

	vector<pair<double, cyPoint3d>> colors3 = { { 0.1,{ 1, 1, 1 } },{ 0.5, {1, 1, 1} },{ 0.5, {1,1,1} },{ 0.0 ,{ 1, 1, 1 } } };
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 0, 0, 2 }, { 1, 1, 1 }, N, colors3, Null_image, Null_image, {}, {}, 0.0, 0.0));

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
				c = c.IsZero() ? palette[(10) % palette.size()] : c;

				quadrics.push_back(planeFromPoints(q1 / 0.5, q2 / 0.5, q3 / 0.5, c, t1, t2, t3, Tex_plane_2, Null_image, 1.0));
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
			double timeVal = 0;
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
					//pix = areaCam.eyeBottomLeft + areaCam.scaleX * areaCam.n0 * x + areaCam.scaleY * areaCam.n1 * y;

					// depth of field
					primaryRay = (pix - areaCam.pos).GetNormalized();
					pointOnFocalPlane = areaCam.pos + areaCam.focalLength * primaryRay;
					for (int pi = 0; pi < areaCam.pxmax; pi++)
					{
						for (int pj = 0; pj < areaCam.pymax; pj++)
						{
							prx = (rand() % 10) / 10.0;
							pry = (rand() % 10) / 10.0;
							for (int psub = 0; psub < areaCam.pxsub; psub++)
							{
								for (int qsub = 0; qsub < areaCam.pysub; qsub++)
								{
									pX = pi + (psub + prx) / areaCam.pxsub;
									pY = pj + (qsub + pry) / areaCam.pysub;
									px = pX / areaCam.pxmax;
									py = pY / areaCam.pymax;
									eyePos = areaCam.eyeBottomLeft + areaCam.scaleX * px * areaCam.n0 + areaCam.scaleY * py * areaCam.n1;

#ifdef MOTION_BLUR
									timeVal += weighted / 2.0;
									quadrics[1].qc.x = timeVal / 3.0;
									quadrics[2].qc.y = timeVal / 3.0;
									timeVal += weighted;
#endif

									secondaryRay = (pointOnFocalPlane - eyePos).GetNormalized();
									color += castRays(eyePos, secondaryRay, quadrics, lights, infSphere, 1) * weighted * psubweighted * pmaxweighted;
								}
							}
						}
					}
					//

					// camera painting
#ifdef CAM_PAINTING
					cp_r = camera_painting.texture[i][j][0];
					cp_g = camera_painting.texture[i][j][1];
					cp_b = camera_painting.texture[i][j][2];

					cp_x = outCount;
					cp_y = outCount;
					cp_z = outCount;

					eye = cyPoint3d{ 0, 2, 5 } + cp_x * cp_r * cam.n0 + cp_y * cp_g * cam.n1 + cp_z * cp_b * cam.n2;
					areaCam = { eye, view, up };
#endif

					//camToPix = (pix - cam.pos).GetNormalized();
					//color += castRays(cam.pos, camToPix, quadrics, lights, infSphere, 1) * weighted;
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
	string fname = OUTPUT_JPG + to_string(animParam) + ".jpg";
	Image :: writeImage(fname.c_str(), frameBuffer);
#endif
}

void update()
{
	animParam += 0.05;
	if (animParam > 2.5)
		animParam2 += 0.1;
	/*else
		eye = cyPoint3d(0, 0, 4) - animParam * view;*/
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(Xmax, Ymax);
	glutCreateWindow("----Synthesized----");
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(quit);
	glutSpecialFunc(keyRot);
	glutIdleFunc(update);
	glutMainLoop();

	return 0;
}