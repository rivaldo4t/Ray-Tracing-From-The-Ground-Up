#include <GL/glut.h>
#include <vector>
#include <iostream>
#include <math.h>
#include <algorithm>
#include "cyPoint.h"
#include "cyCore.h"
using namespace std;


//#define SAVE_PPM
#define SUBSURFACESSCATTERING
//#define AREALIGHT
//#define ANTI_ALIASED
#ifndef ANTI_ALIASED
int subPixX = 1, subPixY = 1;
#else
int subPixX = 4, subPixY = 4;
#endif

# define PI 3.14159265358979323846
double rotX = 0.0, rotY = 0.0;
const int Xmax = 600, Ymax = 600;
double farPlane = 100;
float frameBuffer[Ymax][Xmax][3] = { 0 };

cyPoint3d _DC304B(220.0 / 255.0,  48.0 / 255.0,  75.0 / 255.0);
cyPoint3d _F95F62(249.0 / 255.0,  95.0 / 255.0,  98.0 / 255.0);
cyPoint3d _FFFFFF(255.0 / 255.0, 255.0 / 255.0, 255.0 / 255.0);
cyPoint3d _1F2D3D( 31.0 / 255.0,  45.0 / 255.0,  61.0 / 255.0);
cyPoint3d _343F4B( 52.0 / 255.0,  63.0 / 255.0,  75.0 / 255.0);
cyPoint3d _FFEFCA(255.0 / 255.0, 239.0 / 255.0, 202.0 / 255.0);
cyPoint3d _EDA16A(237.0 / 255.0, 161.0 / 255.0, 106.0 / 255.0);
cyPoint3d _C83741(200.0 / 255.0,  55.0 / 255.0,  65.0 / 255.0);
cyPoint3d _CA4679(202.0 / 255.0, 70.0 / 255.0, 121.0 / 255.0);
cyPoint3d _725E9C(14.0 / 255.0, 94.0 / 255.0, 156.0 / 255.0);
cyPoint3d _823066(130.0 / 255.0, 48.0 / 255.0, 102.0 / 255.0);
cyPoint3d _F4EAC8(244.0 / 255.0, 234.0 / 255.0, 200.0 / 255.0);
cyPoint3d _72828F(114.0 / 255.0, 130.0 / 255.0, 143.0 / 255.0);

static void keyRot(int key, int x, int y)
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

inline double clamp(double val, double low = -1, double high = 1)
{
	if (val < low)
		return 0;
	else if (val > high)
		return 1;
	else
		return (val - low) / (high - low);
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
	double ambientFact, diffuseFact, specularFact, borderFact;
	cyPoint3d ambientColor, diffuseColor, specularColor, borderColor;

	Quadric();

	Quadric(cyPoint3d _ai2,
			double _a21, 
			double _a00, 
			cyPoint3d _qc, 
			cyPoint3d _si,
			vector<cyPoint3d> _N,
			vector<pair<double, cyPoint3d>> _colors) :
			ai2(_ai2), a21(_a21), a00(_a00), qc(_qc), si(_si), N(_N),
			ambientFact(_colors[0].first), ambientColor(_colors[0].second),
			diffuseFact(_colors[1].first), diffuseColor(_colors[1].second),
			specularFact(_colors[2].first), specularColor(_colors[2].second),
			borderFact(_colors[3].first), borderColor(_colors[3].second) {}

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

		if (temp1 > 0 && temp2 > 0)
			return { true, temp2 };
		else if (temp1 > 0)
			return { true, temp1 };
		else if (temp2 > 0)
			return { true, temp2 };
		else
			return { false, INT_MAX };
	}

	inline double intersect_length(cyPoint3d& eye, cyPoint3d& eyeToPix, double lightdist)
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
			return 0;

		if (A == 0)
		{
			if ((-C / B) < 0)
				return 0;
			else
				return (-C / B);
		}

		temp1 = (-B + sqrt(D)) / (2 * A);
		temp2 = (-B - sqrt(D)) / (2 * A);

		if (temp1 > 0 && temp2 > 0)
		{
			if (lightdist > temp1)
				return temp1 - temp2;
			else if (lightdist > temp2)
				return lightdist - temp2;
			else
				return 0;
		}
		else if (temp1 > 0)
		{
			if (lightdist > temp1)
				return temp1;
			else
				return lightdist;
		}
		else
			return 0;
	}

	inline cyPoint3d normalAtHitPoint(cyPoint3d hitPoint)
	{
		double temp;
		cyPoint3d normalAtHit;
		normalAtHit = a21 / si[2] * N[2];
		for (int r = 0; r <= 2; r++)
		{
			temp = 2 * ai2[r] * N[r].Dot(hitPoint - qc) / (si[r] * si[r]);
			normalAtHit += temp * N[r];
		}
		normalAtHit.Normalize();
		return normalAtHit;
	}
};

class Light
{
public:
	cyPoint3d pos;
	cyPoint3d color;
	cyPoint3d dir;

	Light(cyPoint3d _pos, cyPoint3d _color, cyPoint3d _dir = {0,0,0}) : pos(_pos), color(_color), dir(_dir) {}
};

inline cyPoint3d computeAmbientColor(Quadric& q)
{
	return (q.ambientFact * q.ambientColor);
}

inline cyPoint3d computeDiffuseColor(cyPoint3d& normalAtHit, cyPoint3d& hitPointToLight, cyPoint3d& lightColor, double& spotLightComp, double& pointToLightDist, Quadric& q)
{
	cyPoint3d color = { 0, 0, 0 };
	double diffuseComp = clamp(normalAtHit.Dot(hitPointToLight), 0, 0.5);
	diffuseComp *= spotLightComp;
	color = diffuseComp * q.diffuseColor * q.diffuseFact * lightColor / (pow(pointToLightDist, 2));
	return color;
}
inline cyPoint3d computeDiffuseColor2(cyPoint3d& normalAtHit, cyPoint3d& hitPointToLight, cyPoint3d& lightColor, double& spotLightComp, double& pointToLightDist, Quadric& q, double t)
{
	cyPoint3d color = { 0, 0, 0 };
	double diffuseComp = clamp(t, 0, 1);
	diffuseComp *= spotLightComp;
	color = diffuseComp * q.diffuseColor * q.diffuseFact * lightColor / (pow(pointToLightDist, 2));
	return color;
}

inline cyPoint3d computeSpecularColor(cyPoint3d& normalAtHit, cyPoint3d& hitPointToLight, cyPoint3d& lightColor, cyPoint3d& eyeToHitPoint, double& spotLightComp, Quadric& q)
{
	cyPoint3d color = { 0, 0, 0 };
	cyPoint3d lightReflect = -hitPointToLight + 2 * (normalAtHit.Dot(hitPointToLight)) * normalAtHit;
	lightReflect.Normalize();
	double specularComp = clamp((-lightReflect).Dot(eyeToHitPoint), 0.94, 0.96);
	specularComp *= spotLightComp;
	color = specularComp * q.specularColor * lightColor * q.specularFact;
	return color;
}

inline cyPoint3d computeBorderColor(cyPoint3d& normalAtHit, cyPoint3d& eyeToHitPoint, double& spotLightComp, Quadric& q)
{
	cyPoint3d color = { 0, 0, 0 };
	double borderComp = eyeToHitPoint.Dot(normalAtHit) + 1;
	borderComp *= spotLightComp;
	color = clamp(borderComp, 0.84, 0.85) * q.borderColor * q.borderFact;
	return color;
}

inline bool shadowRay(int& objIndex, cyPoint3d& hitPoint, cyPoint3d& hitPointToLight, double& pointToLightDist, vector<Quadric>& quadrics)
{
	for (int i = 0; i < quadrics.size(); i++)
	{
		if (i == objIndex)
			continue;
		double hitParamTemp = quadrics[i].intersect(hitPoint, hitPointToLight).second;
		if (hitParamTemp < pointToLightDist)
			return true;
	}
	return false;
}

void renderScene()
{
#if 1
	cyPoint3d eye(0, 0, 8);
	cyPoint3d view(0, 0, -1);
	cyPoint3d up(0, 1, 0);
#else
	cyPoint3d eye(0, 4, 0);
	cyPoint3d view(0, -1, -1);
	cyPoint3d up(0, 1, -1);
#endif
	rotVec(view, up, rotX);
	eye += rotY*view;

	cyPoint3d n2 = view.GetNormalized();
	cyPoint3d n0 = view.Cross(up).GetNormalized();
	cyPoint3d n1 = n0.Cross(n2).GetNormalized();

	double focalLength			= 8;
	cyPoint3d viewPortCenter	= eye + n2 * focalLength;
	double scaleX				= 10;
	double scaleY				= 10;
	cyPoint3d vpBottomLeft		= viewPortCenter - n0 * (scaleX / 2.0) - n1 * (scaleY / 2.0);

#ifdef AREALIGHT
	cyPoint3d pl(0, 10, -6);
	cyPoint3d ldir(0, -1, 0);
	cyPoint3d lup(0, 0, 1);
	cyPoint3d plcolor(1, 1, 1);
	cyPoint3d n22 = ldir.GetNormalized();
	cyPoint3d n00 = ldir.Cross(lup).GetNormalized();
	cyPoint3d n11 = n00.Cross(n22).GetNormalized();

	int pxmax = 4, pymax = 4;
	int pxsub = 1, pysub = 1;
	double pX, pY, px, py, prx, pry;
	cyPoint3d plpix;
	cyPoint3d colortemptemp;
	double pweighted = 1.0 / (pxsub * pysub);
#endif

	cyPoint3d color, pix, eyeToPix, hitPoint, normalAtHit;
	double X, Y, x, y, rx, ry, hitParam, hitParamTemp;
	double weighted				= 1.0 / (subPixX * subPixY);

	vector<Quadric> quadrics;
	cyPoint3d _qc (0, 0, -10);
	cyPoint3d _ai2(1, 1, 1 );
	cyPoint3d _si (2, 2, 2);
	double _a21 = 0, _a00 = -1;
	vector<cyPoint3d> N = { { 0, 1, 0 }, { 1, 0, 0 }, { 0, 0, -1 } };
	vector<pair<double, cyPoint3d>> colors = { { 0.05, { 1, 1, 1 } }, { 50, _DC304B }, { 1, _DC304B }, { 0.3 ,{ 1, 1, 1 } } };
	quadrics.push_back(Quadric(_ai2, _a21, _a00, { 0, 2, -6 }, { 2, 2, 2 }, N, colors));

	N[2] = { 0, 1, 0 };
	vector<pair<double, cyPoint3d>> colors2 = { { 0.05, { 1, 1, 1 } }, { 50, _72828F },{ 0.0, _72828F },{ 0.0, { 1,1,1 } } };
	quadrics.push_back(Quadric({ 0, 0, 0 }, 1, 0, { 0, -2, 0 }, _si, N, colors2));

	N[2] = { 0, 0, 1 };
	quadrics.push_back(Quadric({ 0, 0, 0 }, 1, 0, { 0, 0, -12 }, _si, N, colors2));

	//Quadric q;

	cyPoint3d lightPos(-8, 8, 0);
	cyPoint3d lightColor(0.8, 0.8, 0.8);
	cyPoint3d spotLightDir(1, -1, -1);
	spotLightDir.Normalize();
	vector<Light> lights = { {lightPos, lightColor, spotLightDir }, { { 0, 8, 0 },{ 0.7, 0.7, 0.7 } } };
	cyPoint3d hitPointToLight, lightReflect, eyeToHitPoint;
	int objIndex;
	double temp;
	cyPoint3d colorTemp;
	double diffuseComp, specularComp, borderComp, spotLightComp;
	double pointToLightDist;
	bool isInShadow;

	double d = 0.1, r;
	cyPoint3d subSurfacePoint, subTolight;

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
#else
			rx = (rand() % 10) / 10.0;
			ry = (rand() % 10) / 10.0;
#endif
			for (int p = 0; p < subPixX; p++)
			{
				for (int q = 0; q < subPixY; q++)
				{
					X			= i + (p + rx) / subPixX;
					Y			= j + (q + ry) / subPixY;
					x			= X / Xmax;
					y			= Y / Ymax;
					pix			= vpBottomLeft + scaleX * x * n0 + scaleY * y * n1;
					eyeToPix	= (pix - eye).GetNormalized();
					objIndex	= -1;
					hitParam	= farPlane;
					
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
					{
#ifndef AREALIGHT
						colorTemp = { 0,0,0 };
#endif
						Quadric q = quadrics[objIndex];
						hitPoint = eye + eyeToPix * hitParam;

#ifdef AREALIGHT
						for (int pi = 0; pi < pxmax; pi++)
						{
							for (int pj = 0; pj < pymax; pj++)
							{
								prx = 0.5;
								pry = 0.5;
								colorTemp = { 0,0,0 };
								for (int psub = 0; psub < pxsub; psub++)
								{
									for (int qsub = 0; qsub < pysub; qsub++)
									{
										colortemptemp = { 0,0,0 };
										pX = pi + (psub + prx) / pxsub;
										pY = pj + (qsub + pry) / pysub;
										px = pX / pxmax;
										py = pY / pymax;
										plpix = pl + scaleX * 0.1 * px * n00 + scaleY * 0.1 * py * n11;

										lightPos = plpix;
										lightColor = plcolor;
										//spotLightDir = {0, 0, 0};
										hitPointToLight = lightPos - hitPoint;
										pointToLightDist = hitPointToLight.Length();
										hitPointToLight.Normalize();
										eyeToHitPoint = eyeToPix;
										normalAtHit = q.normalAtHitPoint(hitPoint);

										isInShadow = shadowRay(objIndex, hitPoint, hitPointToLight, pointToLightDist, quadrics);
										//spotLightComp = (spotLightDir.x == 0 && spotLightDir.y == 0 && spotLightDir.z == 0) ? 1.0 :
											//clamp(spotLightDir.Dot(-hitPointToLight), 0.6, 0.61);
										spotLightComp = 1;

										colortemptemp += computeAmbientColor(q);

										if (isInShadow == false)
										{
											colortemptemp += computeDiffuseColor(normalAtHit, hitPointToLight, lightColor, spotLightComp, pointToLightDist, q);
											colortemptemp += computeSpecularColor(normalAtHit, hitPointToLight, lightColor, eyeToHitPoint, spotLightComp, q);
											colortemptemp += computeBorderColor(normalAtHit, eyeToHitPoint, spotLightComp, q);
										}

										colorTemp += colortemptemp * pweighted;
									}
								}

							}
						}
#else
#ifndef SUBSURFACESSCATTERING
						// normal shadow ray color computation
						for (int lightIndex = 0; lightIndex < lights.size(); lightIndex++)
						{
							lightPos = lights[lightIndex].pos;
							lightColor = lights[lightIndex].color;
							spotLightDir = lights[lightIndex].dir;
							hitPointToLight = lightPos - hitPoint;
							pointToLightDist = hitPointToLight.Length();
							hitPointToLight.Normalize();
							eyeToHitPoint = eyeToPix;
							normalAtHit = q.normalAtHitPoint(hitPoint);

							isInShadow = shadowRay(objIndex, hitPoint, hitPointToLight, pointToLightDist, quadrics);
							spotLightComp = (spotLightDir.x == 0 && spotLightDir.y == 0 && spotLightDir.z == 0) ? 1.0 :
											clamp(spotLightDir.Dot(-hitPointToLight), 0.6, 0.61);

							colorTemp += computeAmbientColor(q);

							if (isInShadow == false)
							{
								colorTemp += computeDiffuseColor(normalAtHit, hitPointToLight, lightColor, spotLightComp, pointToLightDist, q);
								colorTemp += computeSpecularColor(normalAtHit, hitPointToLight, lightColor, eyeToHitPoint, spotLightComp, q);
								colorTemp += computeBorderColor(normalAtHit, eyeToHitPoint, spotLightComp, q);
							}
						}
#else
						// sub surface scattering
						for (int lightIndex = 0; lightIndex < lights.size(); lightIndex++)
						{
							lightPos = lights[lightIndex].pos;
							lightColor = lights[lightIndex].color;
							spotLightDir = lights[lightIndex].dir;
							hitPointToLight = lightPos - hitPoint;
							pointToLightDist = hitPointToLight.Length();
							hitPointToLight.Normalize();
							eyeToHitPoint = eyeToPix;
							normalAtHit = q.normalAtHitPoint(hitPoint);

							spotLightComp = (spotLightDir.x == 0 && spotLightDir.y == 0 && spotLightDir.z == 0) ? 1.0 :
								clamp(spotLightDir.Dot(-hitPointToLight), 0.6, 0.61);

							d = 0.1;
							r = 0;
							subSurfacePoint = hitPoint - (d)*normalAtHit;
							subTolight = (lightPos - subSurfacePoint).GetNormalized();
							if (subTolight.Dot(normalAtHit) < d)
								isInShadow = true;
							else
								isInShadow = false;
							
							colorTemp += computeAmbientColor(q);
							//if (isInShadow == false)
							{
								for (int qi = 0; qi < quadrics.size(); qi++)
								{
									r += quadrics[qi].intersect_length(hitPoint, hitPointToLight, pointToLightDist);
								}

								colorTemp += computeDiffuseColor2(normalAtHit, hitPointToLight, lightColor, spotLightComp, pointToLightDist, q, d / r);
								colorTemp += computeSpecularColor(normalAtHit, hitPointToLight, lightColor, eyeToHitPoint, spotLightComp, q);
								colorTemp += computeBorderColor(normalAtHit, eyeToHitPoint, spotLightComp, q);
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

#ifdef SAVE_PPM 
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