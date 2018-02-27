#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include "cyPoint.h"
#include "cyCore.h"
using namespace std;

#define SAVE_PPM

// enable or disable this to see the aliased or anti-aliased image
// anti-aliased takes around 5 seconds to render; please wait for the image to show
#define ANTI_ALIASED


// image dimensions
const int Xmax = 700, Ymax = 700;

// image data
float frameBuffer[Ymax][Xmax][3] = { 0 };

// colors used
cyPoint3d _DC304B(220.0 / 255.0,  48.0 / 255.0,  75.0 / 255.0);
cyPoint3d _F95F62(249.0 / 255.0,  95.0 / 255.0,  98.0 / 255.0);
cyPoint3d _FFFFFF(255.0 / 255.0, 255.0 / 255.0, 255.0 / 255.0);
cyPoint3d _1F2D3D( 31.0 / 255.0,  45.0 / 255.0,  61.0 / 255.0);
cyPoint3d _343F4B( 52.0 / 255.0,  63.0 / 255.0,  75.0 / 255.0);

// sub pixel sampling parameters
#ifndef ANTI_ALIASED
int m = 1, n = 1;
#else
int m = 4, n = 4;
#endif

// sphere functions
bool _sphere1(cyPoint3d P)
{
	return (P.x - 3)*(P.x - 3) + (P.y - 1.5)*(P.y - 1.5) + (P.z + 3)*(P.z + 3) < 20;
}

bool _sphere2(cyPoint3d P)
{
	return (P.x - 10)*(P.x - 10) + (P.y - 5)*(P.y - 5) + (P.z + 3)*(P.z + 3) < 15;
}

// plane function
bool _plane1(cyPoint3d P)
{
	return (P.x * 3) - (P.y * 4) + (P.z * 5) < 5;
}

// implicit function of choice - prism
bool _prism(cyPoint3d P)
{
	return fabs(fabs(fabs(P.x) + 2 * P.y) + fabs(P.x) - 2 * P.z) + fabs(fabs(fabs(P.x) + 2 * P.y) + fabs(P.x) + 2 * P.z) < 40;
}

// another random implicit function
bool _func(cyPoint3d P)
{
	return P.x*P.x + P.y*P.y - P.z*P.z*P.z < 10;
}

void renderScene()
{
	float X, Y;
	float x, y;

	// default points and vectors
	cyPoint3d P0(0, 0, 0), P;
	cyPoint3d upVec(0, 1, 0), lookAtVec(0, 0, 1);
	cyPoint3d n0 = (upVec ^ lookAtVec).GetNormalized();
	cyPoint3d n1 = upVec.GetNormalized();
	cyPoint3d n2 = lookAtVec.GetNormalized();
	cyPoint3d color;

	float weighted = float(1.0 / (m*n));

	// scaling parameters; size of the viewing plane
	float s0 = 10, s1 = 10;
	
	// random variables for sampling
	float rx, ry;

#ifndef ANTI_ALIASED
	cout << "Aliased Image ...\n";
#else
	cout << "Anti - Aliased Image ...\n";
#endif

	// render loop
	for (int i = 0; i < Xmax; i++)
	{
		if (i % 100 == 0)
			cout << "Rendering..." << int((float(i)/Xmax) * 100) << "%\n";

		for (int j = 0; j < Ymax; j++)
		{

#ifndef ANTI_ALIASED
			rx = 0.5;
			ry = 0.5;
#else
			rx = float((rand() % 10) / 10.0);
			ry = float((rand() % 10) / 10.0);
#endif

			float rx = 0, ry = 0;
			for (int p = 0; p < m; p++)
			{
				for (int q = 0; q < n; q++)
				{
					X = i + (p + rx) / m;
					Y = j + (q + ry) / n;
					x = X / Xmax;
					y = Y / Ymax;
					P = P0 + s0*x*n0 + s1*y*n1;
					color = {frameBuffer[j][i][0], frameBuffer[j][i][1], frameBuffer[j][i][2]};

					if(_sphere1(P))
						color += _DC304B * weighted;
					else if(_sphere2(P))
						color += _F95F62 * weighted;
					else if(_plane1(P))
						color += _1F2D3D * weighted;						 
					else if(_prism(P))
						color += _343F4B * weighted;
					else
						color += _FFFFFF * weighted;

					frameBuffer[j][i][0] = float(color[0]);
					frameBuffer[j][i][1] = float(color[1]);
					frameBuffer[j][i][2] = float(color[2]);
				}
			}
		}
	}

	// display image data
	glDrawPixels(Xmax, Ymax, GL_RGB, GL_FLOAT, frameBuffer);
	glFlush();

// save image data to PPM format
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

void keyPress(unsigned char key, int x, int y)
{
	if (key == 'q') exit(0);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(400, 50);
	glutInitWindowSize(Xmax, Ymax);
	glutCreateWindow("----Synthesized----");
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(keyPress);
	glutMainLoop();
	return 0;
}