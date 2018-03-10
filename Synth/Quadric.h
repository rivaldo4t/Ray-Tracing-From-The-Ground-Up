#pragma once
#include "cyPoint.h"
#include "Image.h"
#include <iostream>
#include <vector>
using namespace std;

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

	Quadric() {};

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

	inline cyPoint3d computeDiffuseColor(cyPoint3d& normalAtHit, cyPoint3d& hitPointToLight, cyPoint3d& lightColor, double& pointToLightDist, double& spotLightComp)
	{
		cyPoint3d color = { 0, 0, 0 };
		double diffuseComp = clamp(normalAtHit.Dot(hitPointToLight), 0, 0.5);
		diffuseComp *= spotLightComp;
		color = diffuseComp * diffuseColor * diffuseFact * lightColor / (pow(pointToLightDist, 2));
		return color;
	}

	inline cyPoint3d computeDiffuseColor2(cyPoint3d& normalAtHit, cyPoint3d& hitPointToLight, cyPoint3d& lightColor, double& pointToLightDist, double t, double& spotLightComp)
	{
		cyPoint3d color = { 0, 0, 0 };
		double diffuseComp = clamp(t, 0, 1);
		diffuseComp *= spotLightComp;
		color = diffuseComp * diffuseColor * diffuseFact * lightColor / (pow(pointToLightDist, 2));
		return color;
	}

	inline cyPoint3d computeSpecularColor(cyPoint3d& normalAtHit, cyPoint3d& hitPointToLight, cyPoint3d& lightColor, cyPoint3d& eyeToHitPoint, double& spotLightComp)
	{
		cyPoint3d color = { 0, 0, 0 };
		cyPoint3d lightReflect = -hitPointToLight + 2 * (normalAtHit.Dot(hitPointToLight)) * normalAtHit;
		lightReflect.Normalize();
		double specularComp = clamp((-lightReflect).Dot(eyeToHitPoint), 0.94, 0.96);
		specularComp *= spotLightComp;
		color = specularComp * specularColor * lightColor * specularFact;
		return color;
	}

	inline cyPoint3d computeBorderColor(cyPoint3d& normalAtHit, cyPoint3d& eyeToHitPoint, double& spotLightComp)
	{
		cyPoint3d color = { 0, 0, 0 };
		double borderComp = eyeToHitPoint.Dot(normalAtHit) + 1;
		borderComp *= spotLightComp;
		color = clamp(borderComp, 0.84, 0.85) * borderColor * borderFact;
		return color;
	}

	inline cyPoint3d computeTextureColor(cyPoint3d hitPoint, cyPoint3d normalAtHit, Image& image)
	{
		cyPoint3d color = {0, 0, 0};
		double u, v, ratioX, ratioY;
		int pixelX, pixelY;
		int w = image.width;
		int h = image.height;

		//plane
		if (ai2[0] == 0)
		{
			u = N[0].Dot(hitPoint - qc);
			v = N[1].Dot(hitPoint - qc);
			if (u < 0)
			u = - u;
			if (v < 0)
			v = - v;
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
			color = image.texture[(pixelY) % h][(pixelX) % w] * (1 - ratioX) * (1 - ratioY) +
					image.texture[(pixelY + 1) % h][pixelX		% w] * (1 - ratioX) * ratioY +
					image.texture[pixelY       % h][(pixelX + 1) % w] * ratioX * (1 - ratioY) +
					image.texture[(pixelY + 1) % h][(pixelX + 1) % w] * ratioX * ratioY;
		}

		//sphere
		else
		{
			double phi, theta, tempX;
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
			color = image.texture[pixelY       % h][pixelX       % w] * (1 - ratioX) * (1 - ratioY) +
					image.texture[(pixelY + 1) % h][pixelX       % w] * (1 - ratioX) * ratioY +
					image.texture[pixelY       % h][(pixelX + 1) % w] * ratioX * (1 - ratioY) +
					image.texture[(pixelY + 1) % h][(pixelX + 1) % w] * ratioX * ratioY;
		}
		return color;
	}
};