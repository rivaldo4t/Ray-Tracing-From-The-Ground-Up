#include <GL/glut.h>
#include <vector>
#include <iostream>
#include <math.h>
#include <algorithm>
#include "cyPoint.h"
#include "cyCore.h"
using namespace std;


//#define SAVE_PPM
//#define ANTI_ALIASED
#ifndef ANTI_ALIASED
int subPixX = 1, subPixY = 1;
#else
int subPixX = 4, subPixY = 4;
#endif

# define PI 3.14159265358979323846
double rotX = 0.0, rotY = 0.0;
const int Xmax = 600, Ymax = 600;
double farPlane = 50;
float frameBuffer[Ymax][Xmax][3] = { 0 };

cyPoint3d _DC304B(220.0 / 255.0,  48.0 / 255.0,  75.0 / 255.0);
cyPoint3d _F95F62(249.0 / 255.0,  95.0 / 255.0,  98.0 / 255.0);
cyPoint3d _FFFFFF(255.0 / 255.0, 255.0 / 255.0, 255.0 / 255.0);
cyPoint3d _1F2D3D( 31.0 / 255.0,  45.0 / 255.0,  61.0 / 255.0);
cyPoint3d _343F4B( 52.0 / 255.0,  63.0 / 255.0,  75.0 / 255.0);
cyPoint3d _FFEFCA(255.0 / 255.0, 239.0 / 255.0, 202.0 / 255.0);
cyPoint3d _EDA16A(237.0 / 255.0, 161.0 / 255.0, 106.0 / 255.0);
cyPoint3d _C83741(200.0 / 255.0,  55.0 / 255.0,  65.0 / 255.0);

static void keyRot(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		rotY -= 5.0;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		rotY += 5.0;
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
	double radAngle = degAngle * PI / 180;
	double cos_theta = cos(radAngle);
	double sin_theta = sin(radAngle);
	v1 = (v1 * cos_theta) + (v2.Cross(v1) * sin_theta) + (v2 * v2.Dot(v1) * (1 - cos_theta));
	//v1.Normalize();
}

void quit(unsigned char key, int x, int y)
{
	if (key == 27) exit(0);
}

class Quadric
{
public:
	cyPoint3d ai2;
	double a21, a00;
	cyPoint3d qc;
	cyPoint3d si;
	vector<cyPoint3d> N;
	cyPoint3d color;

	Quadric(cyPoint3d _ai2,
			double _a21, 
			double _a00, 
			cyPoint3d _qc, 
			cyPoint3d _si,
			vector<cyPoint3d> _N,
			cyPoint3d _color) :
			ai2(_ai2), a21(_a21), a00(_a00), qc(_qc), si(_si), N(_N), color(_color) {}

	inline pair<bool, double> intersect(cyPoint3d eye, cyPoint3d eyeToPix)
	{
		double A(0), B(0), C(0), temp1, temp2, D;

		B += a21 * N[2].Dot(eyeToPix) / si[2];
		C += (a21 * N[2].Dot(eye - qc) / si[2]) + a00;

		for (int i = 0; i <= 2; i++)
		{
			temp1 = N[i].Dot(eyeToPix) / si[i];
			temp2 = N[i].Dot(eye - qc) / si[i];

			A += ai2[i] * temp1 * temp1;
			B += 2 * ai2[i] * temp1 * temp2;
			C += ai2[i] * temp2 * temp2;
		}

		D = (B * B) - (4 * A * C);
		
		if (D < 0)
			return { false, INT_MAX };

		if (A == 0)
		{
			if ((-C / B) < 0)
				return { false, INT_MAX };
			else
				return { true, (-C / B) };
		}

		temp1 = (-B + sqrt(D)) / (2 * A);
		temp2 = (-B - sqrt(D)) / (2 * A);

		if(temp1 > 0 && temp2 > 0)
			return { true, temp2 };
		else
			return { false, INT_MAX };

		/*
		hitPoint = eye + eyeToPix * hitParam;
		normalAtHit = a21 / si[2] * N[2];
		for (int i = 0; i <= 2; i++)
		{
			temp1 = 2 * ai2[i] * N[i].Dot(hitPoint - qc) / (si[i] * si[i]);
			normalAtHit += temp1 * N[i];
		}
		normalAtHit.Normalize();
		*/
	}
};

void renderScene()
{
	cyPoint3d eye(0, 0, 0);
	cyPoint3d view(0, 0, -1);
	cyPoint3d up(0, 1, 0);

	rotVec(view, up, rotX);
	eye += rotY*view;

	cyPoint3d n2 = view.GetNormalized();
	cyPoint3d n0 = view.Cross(up).GetNormalized();
	cyPoint3d n1 = n0.Cross(n2).GetNormalized();

	float focalLength			= 8;
	cyPoint3d viewPortCenter	= eye + n2 * focalLength;
	float scaleX				= 10;
	float scaleY				= 10;
	cyPoint3d vpBottomLeft		= viewPortCenter - n0 * (scaleX / 2.0) - n1 * (scaleY / 2.0);

	cyPoint3d color, pix, eyeToPix, hitPoint, normalAtHit;
	double X, Y, x, y, rx, ry, hitParam, hitParamTemp;
	double weighted				= 1.0 / (subPixX * subPixY);

	vector<Quadric> quadrics;
	int objIndex;

	cyPoint3d _qc (0, 0, -10);
	cyPoint3d _ai2(1, 1, 1 );
	cyPoint3d _si (2, 2, 2);
	double _a21 = 0, _a00 = -1;

	vector<cyPoint3d> N = { {0, 1, 0}, { 1, 0, 0 }, { 0, 0, -1 } };
	quadrics.push_back(Quadric(_ai2, _a21, _a00, _qc, _si, N, _DC304B));
	/*quadrics.push_back(Quadric(_ai2, _a21, _a00, { 1, 1, -10 }, _si*0.6, N, _F95F62));
	quadrics.push_back(Quadric(_ai2, _a21, _a00, { -1, 1, -10 }, _si*0.6, N, _F95F62));
	quadrics.push_back(Quadric(_ai2, _a21, _a00, { 0, -0.8, -8 }, _si*0.2, N, _F95F62));
	quadrics.push_back(Quadric(_ai2, _a21, _a00, { 0.6, 0.6, -8 }, _si*0.15, N, _1F2D3D));
	quadrics.push_back(Quadric(_ai2, _a21, _a00, { -0.6, 0.6, -8 }, _si*0.15, N, _1F2D3D));*/
	N[2] = { 0, 1, 0 };
	quadrics.push_back(Quadric({ 0, 0, 0 }, 1, 0, { 0, -1, -10 }, _si, N, _FFFFFF));
	N[2] = { 0, 0, 1 };
	quadrics.push_back(Quadric({ 0, 0, 0 }, 1, 0, { 0, 0, -14 }, _si, N, _343F4B));

#ifndef ANTI_ALIASED
	cout << "Aliased Image ...\n";
#else
	cout << "Anti - Aliased Image ...\n";
#endif

	for (int i = 0; i < Xmax; i++)
	{
		if (i % 100 == 0)
			cout << "Rendering..." << int((float(i)/Xmax) * 100) << "%\n";

		for (int j = 0; j < Ymax; j++)
		{
			color = { 0,0,0 };
#ifndef ANTI_ALIASED
			rx = 0.5;
			ry = 0.5;
			//color = _1F2D3D;
#else
			rx = (rand() % 10) / 10.0;
			ry = (rand() % 10) / 10.0;
			//color = { frameBuffer[j][i][0], frameBuffer[j][i][1], frameBuffer[j][i][2] };
#endif
			for (int p = 0; p < subPixX; p++)
			{
				for (int q = 0; q < subPixY; q++)
				{
					hitParam	= farPlane;
					X			= i + (p + rx) / subPixX;
					Y			= j + (q + ry) / subPixY;
					x			= X / Xmax;
					y			= Y / Ymax;
					pix			= vpBottomLeft + scaleX * x * n0 + scaleY * y * n1;
					eyeToPix	= (pix - eye).GetNormalized();
					objIndex = -1;

					for (int index = 0; index < quadrics.size(); index++)
					{
						hitParamTemp = quadrics[index].intersect(eye, eyeToPix).second;
						if (hitParamTemp < hitParam)
						{
							hitParam = hitParamTemp;
							objIndex = index;
						}
					}

					if (objIndex == -1)
						color += _1F2D3D * weighted;
					else
						color += quadrics[objIndex].color * weighted;
				}
			}

			frameBuffer[j][i][0] = (float)color[0];
			frameBuffer[j][i][1] = (float)color[1];
			frameBuffer[j][i][2] = (float)color[2];

#if 0
			for (int k = 0; k <= 2; k++)
			{
				if (frameBuffer[j][i][k] < 0.0)
					frameBuffer[j][i][k] = 0.0;
				else if (frameBuffer[j][i][k] > 1.0)
					frameBuffer[j][i][k] = 1.0;
			}
#endif
		}
	}

	glDrawPixels(Xmax, Ymax, GL_RGB, GL_FLOAT, frameBuffer);
	glFlush();
	cout << "Rendering...100%.\nDone.\n\n";

#ifdef SAVE_PPM
#ifndef ANTI_ALIASED
	const char* filename = "aliased.ppm";
#else
	const char* filename = "anti-aliased.ppm";
#endif

	int i, j;
	FILE *fp = fopen(filename, "wb");
	fprintf(fp, "P6\n%d %d\n255\n", Xmax, Ymax);
	for (j = Ymax-1; j >= 0; --j)
	{
		for (i = 0; i < Xmax; ++i)
		{
			static unsigned char color[3];
			color[0] = unsigned char(frameBuffer[j][i][0] * 255);
			color[1] = unsigned char(frameBuffer[j][i][1] * 255);
			color[2] = unsigned char(frameBuffer[j][i][2] * 255);
			fwrite(color, 1, 3, fp);
		}
	}
	fclose(fp);
	cout << "\nFile saved as " << filename << endl;
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