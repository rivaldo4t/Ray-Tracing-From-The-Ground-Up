#pragma once
#include "cyPoint.h"
#include "Image.h"
#include <iostream>
#include <vector>
#include <math.h>
using namespace std;

class Quadric
{
public:
	int type;
	cyPoint3d ai2;
	double a21, a00;
	cyPoint3d qc;
	cyPoint3d si;
	vector<cyPoint3d> N;
	cyPoint3d color;
	double ambientFact, diffuseFact, specularFact, borderFact;
	cyPoint3d ambientColor, diffuseColor, specularColor, borderColor;
	Image& textureImage;
	Image& normalImage;
	vector<cyPoint3d> points;
	vector<cyPoint2d> texpoints;
	double s, t;
	//Quadric() {}

	Quadric(cyPoint3d _ai2,
		double _a21,
		double _a00,
		cyPoint3d _qc,
		cyPoint3d _si,
		vector<cyPoint3d> _N,
		vector<pair<double, cyPoint3d>> _colors, 
		Image& _I, Image& _I2,
		vector<cyPoint3d> _points,
		vector<cyPoint2d> _texpoints) :
		ai2(_ai2), a21(_a21), a00(_a00), qc(_qc), si(_si), N(_N),
		ambientFact(_colors[0].first), ambientColor(_colors[0].second),
		diffuseFact(_colors[1].first), diffuseColor(_colors[1].second),
		specularFact(_colors[2].first), specularColor(_colors[2].second),
		borderFact(_colors[3].first), borderColor(_colors[3].second),
		textureImage(_I), normalImage(_I2), points(_points), texpoints(_texpoints)
		{
			//plane
			if (ai2.IsZero())
				type = 0;
			//sphere
			else
				type = 1;
			s = 0.0;
			t = 0.0;
		}

	inline double intersect(cyPoint3d eye, cyPoint3d eyeToPix)
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
			return INT_MAX;

		if (A == 0)
		{
			if ((-C / B) < 0.0001)
				return INT_MAX;
			else
				return (-C / B);
		}

		temp1 = (-B + sqrt(D)) / (2 * A);
		temp2 = (-B - sqrt(D)) / (2 * A);

		if (temp1 > 0 && temp2 > 0)
			return temp2;
		else if (temp1 > 0.0001)
			return temp1;
		else
			return INT_MAX;
	}

	inline double absMax(cyPoint3d a)
	{
		double absmax = abs(a[0]);
		bool minus = a[0] < 0 ? true : false;
		for (int i = 1; i < 3; i++)
		{
			if (abs(a[i]) > absmax)
			{
				absmax = abs(a[i]);
				minus = a[i] < 0 ? true : false;
			}
		}
		if (minus == true)
			absmax *= -1;
		return absmax;
	}

	inline double intersect2(cyPoint3d eye, cyPoint3d eyeToPix)
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
			return INT_MAX;

		if (A == 0)
		{
			if ((-C / B) < 0.0001)
				return INT_MAX;
			else
			{
				double hitParam = -C / B;
				cyPoint3d ph = eye + eyeToPix * hitParam;
				cyPoint3d A, A0, A1, A2;
				cyPoint3d p0, p1, p2;
				p0 = points[0];
				p1 = points[1];
				p2 = points[2];
				A = (p1 - p0).Cross(p2 - p0);
				A0 = (ph - p1).Cross(ph - p2);
				A1 = (ph - p2).Cross(ph - p0);
				A2 = (ph - p0).Cross(ph - p1);
				double maxA, maxA0, maxA1, maxA2;
				maxA = absMax(A);
				maxA0 = absMax(A0);
				maxA1 = absMax(A1);
				maxA2 = absMax(A2);
				double w = maxA0 / maxA;
				s = maxA1 / maxA;
				t = maxA2 / maxA;
				if (s + t + w > 0.9999 && s + t + w < 1.0001 && s >= 0 && s <= 1 && t >= 0 && t <= 1 && w >= 0 && w <= 1)
					return hitParam;
				else 
					return INT_MAX;
			}
				
		}

		temp1 = (-B + sqrt(D)) / (2 * A);
		temp2 = (-B - sqrt(D)) / (2 * A);

		if (temp1 > 0 && temp2 > 0)
			return temp2;
		else if (temp1 > 0.0001)
			return temp1;
		else
			return INT_MAX;
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
			temp1 = (-C / B);
			if (temp1 < 0)
				return 0;
			else if (temp1 < lightdist)
				return temp1;
			else
				return 0;
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

	inline cyPoint3d computeAmbientColor()
	{
		return ambientFact * ambientColor;
	}

	inline cyPoint3d computeDiffuseColor(cyPoint3d& normalAtHit, cyPoint3d& hitPointToLight, cyPoint3d& lightColor, double pointToLightDist, double spotLightComp = 1.0, double t = 0.0)
	{
		cyPoint3d color = { 0, 0, 0 };
		double diffuseComp = t == 0.0 ? clamp(normalAtHit.Dot(hitPointToLight), 0, 0.5) : clamp(t, 0, 1);
		diffuseComp *= spotLightComp;
		color = diffuseComp * diffuseColor * diffuseFact * lightColor / (pow(pointToLightDist, 2));
		return color;
	}

	inline cyPoint3d computeSpecularColor(cyPoint3d& normalAtHit, cyPoint3d& hitPointToLight, cyPoint3d& lightColor, cyPoint3d& eyeToHitPoint, double spotLightComp = 1.0)
	{
		cyPoint3d color = { 0, 0, 0 };
		cyPoint3d lightReflect = -hitPointToLight + 2 * (normalAtHit.Dot(hitPointToLight)) * normalAtHit;
		lightReflect.Normalize();
		double specularComp = clamp((-lightReflect).Dot(eyeToHitPoint), 0.94, 0.96);
		specularComp *= spotLightComp;
		color = specularComp * specularColor * lightColor * specularFact;
		return color;
	}

	inline cyPoint3d computeBorderColor(cyPoint3d& normalAtHit, cyPoint3d& eyeToHitPoint, double spotLightComp = 1.0)
	{
		cyPoint3d color = { 0, 0, 0 };
		double borderComp = eyeToHitPoint.Dot(normalAtHit) + 1;
		borderComp *= spotLightComp;
		color = clamp(borderComp, 0.84, 0.85) * borderColor * borderFact;
		return color;
	}

	inline cyPoint3d computeTextureColor(cyPoint3d hitPoint, cyPoint3d normalAtHit)
	{
		cyPoint3d color = {0, 0, 0};
		double u, v, ratioX, ratioY;
		int pixelX, pixelY;
		int w, h;

		switch(type)
		{
			//plane
			case 0:
			{
				w = textureImage.width;
				h = textureImage.height;
				/*u = N[0].Dot(hitPoint - qc);
				v = N[1].Dot(hitPoint - qc);
				if (u < 0)
					u = -u;
				if (v < 0)
					v = -v;*/
				u = (1 - s - t)*texpoints[0][0] + s*texpoints[1][0] + t*texpoints[2][0];
				v = (1 - s - t)*texpoints[0][1] + s*texpoints[1][1] + t*texpoints[2][1];
				u = u * w;
				v = v * h;
				pixelX = (int)(u);
				pixelY = (int)(v);
				/*if (pixelX < 0)
				pixelX = w - (abs(pixelX) % w);
				if (pixelY < 0)
				pixelY = h - (abs(pixelY) % h);*/
				ratioX = u - pixelX;
				ratioY = v - pixelY;
				color = textureImage.texture[(pixelY) % h][(pixelX) % w] * (1 - ratioX) * (1 - ratioY) +
						textureImage.texture[(pixelY + 1) % h][pixelX		% w] * (1 - ratioX) * ratioY +
						textureImage.texture[pixelY       % h][(pixelX + 1) % w] * ratioX * (1 - ratioY) +
						textureImage.texture[(pixelY + 1) % h][(pixelX + 1) % w] * ratioX * ratioY;
				break;
			}
			//sphere
			case 1:
			{
				double phi, theta, tempX;

				if (normalImage.width != 0)
				{
					w = normalImage.width;
					h = normalImage.height;
					
					phi = N[2].Dot(normalAtHit) / si[2];
					phi = acos(phi);
					v = phi / cy::cyPi<double>();

					theta = N[1].Dot(normalAtHit) / si[1];
					theta = theta / sin(phi);
					theta = acos(theta);
					tempX = N[0].Dot(normalAtHit) / si[0];
					theta = tempX >= 0 ? theta : (cy::cyPi<double>() * 2) - theta;
					u = theta / (cy::cyPi<double>() * 2);
					
					//handle negative dot product
					
					v = v * h;
					u = u * w;
					pixelY = (int)(v);
					pixelX = (int)(u);
					ratioY = v - pixelY;
					ratioX = u - pixelX;

					color = normalImage.texture[pixelY       % h][pixelX       % w] * (1 - ratioX) * (1 - ratioY) +
							normalImage.texture[(pixelY + 1) % h][pixelX       % w] * (1 - ratioX) * ratioY +
							normalImage.texture[pixelY       % h][(pixelX + 1) % w] * ratioX * (1 - ratioY) +
							normalImage.texture[(pixelY + 1) % h][(pixelX + 1) % w] * ratioX * ratioY;
					cyPoint3d t;
					t = (2 * color[0] - 1) * N[0] + (2 * color[1] - 1) * N[1] + (2 * color[2] - 1) * N[2];
					//t = color*2.0 - 1.0;
					//t.Normalize();
					//normalAtHit += t;
					//normalAtHit.Normalize();
				}

				w = textureImage.width;
				h = textureImage.height;

				phi = N[2].Dot(normalAtHit) / si[2];
				phi = acos(phi);
				v = phi / cy::cyPi<double>();

				theta = N[1].Dot(normalAtHit) / si[1];
				theta = theta / sin(phi);
				theta = acos(theta);
				tempX = N[0].Dot(normalAtHit) / si[0];
				theta = tempX >= 0 ? theta : (cy::cyPi<double>() * 2) - theta;
				u = theta / (cy::cyPi<double>() * 2);
				
				//handle negative dot product
				
				v = v * h;
				u = u * w;
				pixelY = (int)(v);
				pixelX = (int)(u);
				ratioY = v - pixelY;
				ratioX = u - pixelX;
				color = textureImage.texture[pixelY       % h][pixelX       % w] * (1 - ratioX) * (1 - ratioY) +
						textureImage.texture[(pixelY + 1) % h][pixelX       % w] * (1 - ratioX) * ratioY +
						textureImage.texture[pixelY       % h][(pixelX + 1) % w] * ratioX * (1 - ratioY) +
						textureImage.texture[(pixelY + 1) % h][(pixelX + 1) % w] * ratioX * ratioY;

				break;
			}
		}

		diffuseColor = color;
		specularColor = color;
		return color;
	}
};