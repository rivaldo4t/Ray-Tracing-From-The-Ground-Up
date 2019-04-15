#include "Main.hpp"
#include "LoadObj.hpp"
using namespace std;

//#define OUTPUT_JPG "output/out"
//#define OBJFILE "objects/cube.obj"
//#define ANTI_ALIASED
//#define DEPTH_OF_FIELD
//#define MOTION_BLUR
//#define CAM_PAINTING

void renderScene()
{
	cyPoint3d eye(0.2, 0, 6);
	cyPoint3d view(0, 0, -1);
	cyPoint3d up(0, 1, 0);

	cyPoint3d color, pix, camToPix;
	double X, Y, x, y, rx, ry;
	int subPixX, subPixY;

	Camera cam = {eye, view, up, double(Xmax) / double(Ymax) * 10, 10, 6};

	vector<Light> lights = { {{ 0, 10, 0 },{ 0.6, 0.6, 0.6 }},
							 {{ -6, 0, 6 },{ 0.2, 0.2, 0.2 }},
							 {{ 6, 0, 6 }, { 0.6, 0.6, 0.6 }} };
	
	Camera proj = { { 0, 20, -4 },{ 0, -1, 0 },{ 0, 0, -1 }, 9, 9, 10 };
	cyPoint3d solidColor;

#ifndef DEPTH_OF_FIELD
	AreaCamera areaCam(eye, view, up);
#else
	AreaCamera areaCam(eye, view, up, 0.06, 0.06, 4, 4, 1, 1, 5);
#endif

	cyPoint3d eyePos;
	double pX, pY, px, py, prx, pry;
	double psubweighted = 1.0 / (areaCam.pxsub * areaCam.pysub);
	double pmaxweighted = 1.0 / (areaCam.pxmax * areaCam.pymax);
	cyPoint3d pointOnFocalPlane;
	cyPoint3d primaryRay, secondaryRay;

	vector<Quadric> quadrics;
	vector<cyPoint3d> axes = { { 0, 0, 1 }, { 1, 0, 0 }, { 0, 1, 0 } };
	
	Material mt1(0.05, { 1, 1, 1 }, 0.5, _725E9C , 0.0, _725E9C, 0.0, { 1, 1, 1 }, Tex_env, Null_image);
	Quadric infSphere({ 1, 1, 1 }, 0, -1, { 0, 0, 0 }, { 1, 1, 1 }, axes, mt1);

	Material mt2(0.05, { 1, 1, 1 }, 0.5, { 0.7, 0.7, 0.7 }, 0.0, { 1, 1, 1 }, 0.0, { 1, 1, 1 }, Tex_sphere, Null_image);
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 0, 0, 0 }, { 1, 1, 1 }, axes, mt2));

#ifdef OBJFILE
	LoadObjFile(OBJFILE, quadrics);
#endif

	for (int i = 0; i < Xmax; i++)
	{
		if (i % 50 == 0)
			cout << ".";
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
			subPixX = 3;
			subPixY = 3;
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

#if DEPTH_OF_FIELD
					//pix = areaCam.eyeBottomLeft + areaCam.scaleX * areaCam.n0 * x + areaCam.scaleY * areaCam.n1 * y;
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

#ifdef CAM_PAINTING
					cp_r = camera_painting.texture[j][i][0];
					cp_g = camera_painting.texture[j][i][1];
					cp_b = camera_painting.texture[j][i][2];

					cp_x = animParam2;
					cp_y = animParam2;
					cp_z = animParam2;

					eye = cyPoint3d{ 0.2, 0, 6 } + cp_x * cp_r * cam.n0 + cp_y * cp_g * cam.n1 + cp_z * cp_b * cam.n2;
					areaCam = { eye, view, up };
#endif
#else
					camToPix = (pix - cam.pos).GetNormalized();
					color += castRays(cam.pos, camToPix, quadrics, lights, infSphere, 1) * weighted;
#endif
				}
			}

			for (int k = 0; k < 3; ++k)
				frameBuffer[(Ymax * i + j) * 3 + k] = (float)clamp(color[k], 0.0, 1.0);
		}
	}

	glDrawPixels(Xmax, Ymax, GL_RGB, GL_FLOAT, frameBuffer.data());
	glFlush();
	cout << "!\n\n";

#ifdef OUTPUT_JPG
	string fname = OUTPUT_JPG + to_string(animParam) + ".jpg";
	Image :: writeImage(fname.c_str(), frameBuffer);
#endif
}

void keyRot(int key, int x, int y)
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

void quit(unsigned char key, int x, int y)
{
	if (key == 27) exit(0);
}

void update()
{
	animParam += 0.05;
	if (animParam > 1)
		animParam2 += 0.075;
	/*else
		eye = cyPoint3d(0, 0, 4) - animParam * view;*/
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(500, 100);
	glutInitWindowSize(Xmax, Ymax);
	glutCreateWindow("----Synthesized----");
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(quit);
	glutSpecialFunc(keyRot);
	//glutIdleFunc(update);
	glutMainLoop();

	return 0;
}