#include <GL/glut.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>
#include "cyPoint.h"
#include "cyCore.h"
#include "Quadric.h"
#include "Light.h"
#include "color_templates.h"
using namespace std; 

//#define SAVE_PPM
//#define SUBSURFACESSCATTERING
//#define AREALIGHT
//#define ANTI_ALIASED

#ifndef ANTI_ALIASED
int subPixX = 1, subPixY = 1;
#else
int subPixX = 4, subPixY = 4;
#endif

double rotX = 0.0, rotY = 0.0;
const int Xmax = 600, Ymax = 600;
double farPlane = 100;
float frameBuffer[Ymax][Xmax][3] = { 0 };

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
		if (i == objIndex)
			continue;
		double hitParamTemp = quadrics[i].intersect(hitPoint, hitPointToLight).second;
		if (hitParamTemp < pointToLightDist)
			return true;
	}
	return false;
}

struct PPMObject
{
	std::string magicNum;
	int width, height, maxColVal;
	char * m_Ptr;
};

std::istream& operator >>(std::istream &inputStream, PPMObject &obj)
{
	inputStream >> obj.magicNum;
	inputStream >> obj.width >> obj.height >> obj.maxColVal;
	inputStream.get();
	size_t size = obj.width * obj.height * 3;
	obj.m_Ptr = new char[size];
	inputStream.read(obj.m_Ptr, size);
	return inputStream;
}

std::ostream& operator <<(std::ostream &outputStream, const PPMObject &other)
{
	outputStream << "P6" << "\n"
		<< other.width << " "
		<< other.height << "\n"
		<< other.maxColVal << "\n"
		;
	size_t size = other.width * other.height * 3;
	outputStream.write(other.m_Ptr, size);
	return outputStream;
}

void renderScene()
{
	auto ifs = ifstream("tex.ppm", ios::binary);
	PPMObject ppmObject = PPMObject();
	ifs >> ppmObject;
	ifs.close();
	int k = 0;
	auto inputTexture = vector<vector<cyPoint3d>>(ppmObject.height, vector<cyPoint3d>(ppmObject.width));
	for (int j = ppmObject.height - 1; j >= 0; --j)
	{
		for (int i = 0; i < ppmObject.width; ++i)
		{
			inputTexture[j][i][0] = unsigned char(ppmObject.m_Ptr[k]) / 255.0;
			inputTexture[j][i][1] = unsigned char(ppmObject.m_Ptr[k + 1]) / 255.0;
			inputTexture[j][i][2] = unsigned char(ppmObject.m_Ptr[k + 2]) / 255.0;
			k += 3;
		}
	}
	
	ifs = ifstream("tex2.ppm", ios::binary);
	PPMObject ppmObject2 = PPMObject();
	ifs >> ppmObject2;
	ifs.close();
	k = 0;
	auto inputTexture2 = vector<vector<cyPoint3d>>(ppmObject2.height, vector<cyPoint3d>(ppmObject2.width));
	for (int j = ppmObject2.height - 1; j >= 0; --j)
	{
		for (int i = 0; i < ppmObject2.width; ++i)
		{
			inputTexture2[j][i][0] = unsigned char(ppmObject2.m_Ptr[k]) / 255.0;
			inputTexture2[j][i][1] = unsigned char(ppmObject2.m_Ptr[k + 1]) / 255.0;
			inputTexture2[j][i][2] = unsigned char(ppmObject2.m_Ptr[k + 2]) / 255.0;
			k += 3;
		}
	}

	ifs = ifstream("tex7.ppm", ios::binary);
	PPMObject ppmObject3 = PPMObject();
	ifs >> ppmObject3;
	ifs.close();
	k = 0;
	auto inputTexture3 = vector<vector<cyPoint3d>>(ppmObject3.height, vector<cyPoint3d>(ppmObject3.width));
	for (int j = ppmObject3.height - 1; j >= 0; --j)
	{
		for (int i = 0; i < ppmObject3.width; ++i)
		{
			inputTexture3[j][i][0] = unsigned char(ppmObject3.m_Ptr[k]) / 255.0;
			inputTexture3[j][i][1] = unsigned char(ppmObject3.m_Ptr[k + 1]) / 255.0;
			inputTexture3[j][i][2] = unsigned char(ppmObject3.m_Ptr[k + 2]) / 255.0;
			k += 3;
		}
	}

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

	double focalLength = 8;
	cyPoint3d viewPortCenter = eye + n2 * focalLength;
	double scaleX = 10;
	double scaleY = 10;
	cyPoint3d vpBottomLeft = viewPortCenter - n0 * (scaleX / 2.0) - n1 * (scaleY / 2.0);

#ifdef AREALIGHT
	cyPoint3d pl(0, 10, -2);
	cyPoint3d ldir(0, -1, 0);
	cyPoint3d lup(0, 0, 1);
	cyPoint3d plcolor(1, 1, 1);
	cyPoint3d n22 = ldir.GetNormalized();
	cyPoint3d n00 = ldir.Cross(lup).GetNormalized();
	cyPoint3d n11 = n00.Cross(n22).GetNormalized();

	int pxmax = 8, pymax = 8;
#ifndef ANTI_ALIASED
	int pxsub = 1, pysub = 1;
#else
	int pxsub = 4, pysub = 4;
#endif
	double pX, pY, px, py, prx, pry;
	cyPoint3d plpix;
	cyPoint3d colortemptemp;
	double pweighted = 1.0 / (pxsub * pysub);
#endif

	cyPoint3d color, pix, eyeToPix, hitPoint, normalAtHit;
	double X, Y, x, y, rx, ry, hitParam, hitParamTemp;
	double weighted = 1.0 / (subPixX * subPixY);
	
	vector<Quadric> quadrics;
	vector<cyPoint3d> N = { { 0, 0, -1 },{ -1, 0, 0 },{ 0, -1, 0 } };
	//vector<cyPoint3d> N = { { 0, 1, 0 },{ 0, 0, 1 },{ 1, 0, 0 } };
	vector<pair<double, cyPoint3d>> colors = { { 0.05, { 1, 1, 1 } }, { 50, _DC304B }, { 3, _DC304B }, { 0.0 ,{ 1, 1, 1 } } };
	quadrics.push_back(Quadric({ 1, 1, 1 }, 0, -1, { 2, 2, 0 }, { 2, 2, 2 }, N, colors));

	N[2] = { 0, 1, 0 };
	vector<pair<double, cyPoint3d>> colors2 = { { 0.05, { 1, 1, 1 } }, { 50, _72828F }, { 0.0, _72828F }, { 0.0, { 1, 1, 1 } } };
	quadrics.push_back(Quadric({ 0, 0, 0 }, 1, 0, { 0, -4, 0 }, { 0.20, 0.20, 0.20 }, N, colors2));

	N = { {1, 0 ,0}, {0, 1, 0}, { 0, 0, 1 } };
	//quadrics.push_back(Quadric({ 0, 0, 0 }, 1, 0, { 0, 0, -6 }, {2, 2, 2}, N, colors2));

	cyPoint3d lightPos(-8, 8, 0);
	cyPoint3d lightColor(0.8, 0.8, 0.8);
	cyPoint3d spotLightDir(0, -1, -1);
	spotLightDir.Normalize();
	//vector<Light> lights = { { lightPos, lightColor, spotLightDir }, { { 0, 8, 0 },{ 0.7, 0.7, 0.7 } } };
	vector<Light> lights = { { { 0, 8, 0 },{ 0.7, 0.7, 0.7 } } };
	cyPoint3d hitPointToLight, lightReflect, eyeToHitPoint;
	int objIndex;
	double temp;
	cyPoint3d colorTemp;
	double spotLightComp;
	double pointToLightDist;
	bool isInShadow;

	double d = 0.1, r;
	cyPoint3d subSurfacePoint, subTolight;

	double u, v;
	double phi, theta;
	double ratioX, ratioY;
	int pixelX, pixelY;
	double tempX;
	vector<cyPoint3d> N2 = { { 0, 0, 1 },{ -1, 0, 0 },{ 0, -1, 0 } };
	cout << "Navigate using ARROW KEYS ...\n\n";
#ifndef ANTI_ALIASED
	cout << "Aliased Image ...\n";
#else
	cout << "Anti - Aliased Image ...\n";
#endif

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
					pix = vpBottomLeft + scaleX * x * n0 + scaleY * y * n1;
					eyeToPix = (pix - eye).GetNormalized();
					objIndex = -1;
					hitParam = farPlane;

					for (unsigned int index = 0; index < quadrics.size(); index++)
					{
						hitParamTemp = quadrics[index].intersect(eye, eyeToPix).second;
						if (hitParamTemp < hitParam)
						{
							hitParam = hitParamTemp;
							objIndex = index;
						}
					}

					if (objIndex == -1)
					{
						//color += _1F2D3D * weighted;
						phi = N2[2].Dot(eyeToPix);
						phi = acos(phi);
						v = phi / cy::cyPi<double>();

						theta = N2[1].Dot(eyeToPix);
						theta = theta / sin(phi);
						theta = acos(theta);
						tempX = N2[0].Dot(eyeToPix);
						theta = tempX >= 0 ? theta : (cy::cyPi<double>() * 2) - theta;
						u = theta / (cy::cyPi<double>() * 2);

						v = v * ppmObject.height;
						u = u * ppmObject.width;
						pixelY = (int)(v);
						pixelX = (int)(u);
						ratioY = v - pixelY;
						ratioX = u - pixelX;
						color = inputTexture[pixelY][pixelX] * (1 - ratioX) * (1 - ratioY) +
								inputTexture[pixelY + 1][pixelX] * (1 - ratioX) * ratioY +
								inputTexture[pixelY][pixelX + 1] * ratioX * (1 - ratioY) +
								inputTexture[pixelY + 1][pixelX + 1] * ratioX * ratioY;
					}
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
								prx = (rand() % 10) / 10.0;
								pry = (rand() % 10) / 10.0;
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

										colortemptemp += q.computeAmbientColor();

										if (isInShadow == false)
										{
											colortemptemp += q.computeDiffuseColor(normalAtHit, hitPointToLight, lightColor, pointToLightDist, spotLightComp);
											colortemptemp += q.computeSpecularColor(normalAtHit, hitPointToLight, lightColor, eyeToHitPoint, spotLightComp);
											colortemptemp += q.computeBorderColor(normalAtHit, eyeToHitPoint, spotLightComp);
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

							colorTemp += q.computeAmbientColor();

							if (isInShadow == false)
							{
								//colorTemp += q.computeDiffuseColor(normalAtHit, hitPointToLight, lightColor, pointToLightDist, spotLightComp);
								//colorTemp += q.computeSpecularColor(normalAtHit, hitPointToLight, lightColor, eyeToHitPoint, spotLightComp);
								//colorTemp += q.computeBorderColor(normalAtHit, eyeToHitPoint, spotLightComp);
								if(q.ai2[0] == 0)
									colorTemp = q.computeTextureColor(hitPoint, normalAtHit, ppmObject2.width, ppmObject2.height, inputTexture2, inputTexture2);
								else
									colorTemp = q.computeTextureColor(hitPoint, normalAtHit, ppmObject.width, ppmObject.height, inputTexture, inputTexture3);
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
								for (unsigned int qi = 0; qi < quadrics.size(); qi++)
								{
									r += quadrics[qi].intersect_length(hitPoint, hitPointToLight, pointToLightDist);
								}

								colorTemp += q.computeDiffuseColor2(normalAtHit, hitPointToLight, lightColor, pointToLightDist, d / r, spotLightComp);
								colorTemp += q.computeSpecularColor(normalAtHit, hitPointToLight, lightColor, eyeToHitPoint, spotLightComp);
								colorTemp += q.computeBorderColor(normalAtHit, eyeToHitPoint, spotLightComp);
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

#ifdef SAVE_PPM
#ifndef ANTI_ALIASED
	const char* filename = "aliased.ppm";
#else
	const char* filename = "anti-aliased.ppm";
#endif
#if 0
	int i, j;
	FILE *fp = fopen(filename, "wb");
	fprintf(fp, "P6\n%d %d\n255\n", Xmax, Ymax);
	for (j = Ymax - 1; j >= 0; --j)
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
#else
	auto ofs = ofstream(filename, ios::binary);
	PPMObject ppmObject4 = PPMObject();
	ppmObject4.width = Xmax;
	ppmObject4.height = Ymax;
	ppmObject4.maxColVal = 255;
	ppmObject4.m_Ptr = new char[ppmObject4.width * ppmObject4.height * 3];
	int k2 = 0;
	for (int j = Ymax - 1; j >= 0; --j)
	{
		for (int i = 0; i < Xmax; ++i)
		{
			ppmObject4.m_Ptr[k2] = (frameBuffer[j][i][0] * 255);
			ppmObject4.m_Ptr[k2 + 1] = (frameBuffer[j][i][1] * 255);
			ppmObject4.m_Ptr[k2 + 2] = (frameBuffer[j][i][2] * 255);
			k2 += 3;
		}
	}
	ofs << ppmObject4;
	ofs.close();
#endif
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