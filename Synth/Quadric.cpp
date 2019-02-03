#include "Quadric.hpp"

double Quadric :: intersect(cyPoint3d eye, cyPoint3d eyeToPix)
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
			/*double maxA, maxA0, maxA1, maxA2;
			maxA = absMax(A);
			maxA0 = absMax(A0);
			maxA1 = absMax(A1);
			maxA2 = absMax(A2);
			double w = maxA0 / maxA;
			s = maxA1 / maxA;
			t = maxA2 / maxA;*/
			s = A1.Length() / A.Length();
			t = A2.Length() / A.Length();
			//if (s + t + w > 0.9999 && s + t + w < 1.0001 && s >= 0 && s <= 1 && t >= 0 && t <= 1 && w >= 0 && w <= 1)
			if (abs(A0.Length() + A1.Length() + A2.Length() - A.Length()) < 0.00001)
				return hitParam;
			else
				return INT_MAX;
		}

	}

	temp1 = (-B + sqrt(D)) / (2 * A);
	temp2 = (-B - sqrt(D)) / (2 * A);

	if (temp1 > 0.0001 && temp2 > 0.0001)
		return temp2;
	else if (temp1 > 0.0001)
		return temp1;
	else
		return INT_MAX;
}

double Quadric :: intersect_length(cyPoint3d& eye, cyPoint3d& eyeToPix, double lightdist)
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

cyPoint3d Quadric :: normalAtHitPoint(cyPoint3d hitPoint)
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

	if (normalImage.data != NULL)
	{
		cyPoint3d color = { 0, 0, 0 };
		double u, v, ratioX, ratioY;
		int pixelX, pixelY;
		int w, h;
		double phi, theta, tempX;

		switch (type)
		{
		case 0:
		{
			w = normalImage.width;
			h = normalImage.height;

			u = (1 - s - t)*texpoints[0][0] + s*texpoints[1][0] + t*texpoints[2][0];
			v = (1 - s - t)*texpoints[0][1] + s*texpoints[1][1] + t*texpoints[2][1];
			u = u * w;
			v = v * h;

			pixelX = (int)(u);
			pixelY = (int)(v);
			ratioX = u - pixelX;
			ratioY = v - pixelY;

			color = normalImage.texture[(pixelY + 0) % h][(pixelX + 0) % w] * (1 - ratioX) * (1 - ratioY) +
				normalImage.texture[(pixelY + 1) % h][(pixelX + 0) % w] * (1 - ratioX) * ratioY +
				normalImage.texture[(pixelY + 0) % h][(pixelX + 1) % w] * ratioX * (1 - ratioY) +
				normalImage.texture[(pixelY + 1) % h][(pixelX + 1) % w] * ratioX * ratioY;

			cyPoint3d t;
			t = (2 * color[0] - 1) * N[0] + (2 * color[1] - 1) * N[1] + (2 * color[2] - 1) * N[2];
			t = color*2.0 - 1.0;
			normalAtHit = t;
			break;
		}
		case 1:
		{
			w = normalImage.width;
			h = normalImage.height;

			phi = N[2].Dot(hitPoint - qc) / si[2];
			phi = acos(phi);
			v = phi / cy::cyPi<double>();

			theta = N[1].Dot(hitPoint - qc) / si[1];
			theta = theta / sin(phi);
			theta = acos(theta);
			tempX = N[0].Dot(hitPoint - qc) / si[0];
			theta = tempX >= 0 ? theta : (cy::cyPi<double>() * 2) - theta;
			u = theta / (cy::cyPi<double>() * 2);

			//handle negative dot product

			v = v * h;
			u = u * w;
			pixelY = (int)(v);
			pixelX = (int)(u);
			ratioY = v - pixelY;
			ratioX = u - pixelX;

			color = normalImage.texture[(pixelY + 0) % h][(pixelX + 0) % w] * (1 - ratioX) * (1 - ratioY) +
				normalImage.texture[(pixelY + 1) % h][(pixelX + 0) % w] * (1 - ratioX) * ratioY +
				normalImage.texture[(pixelY + 0) % h][(pixelX + 1) % w] * ratioX * (1 - ratioY) +
				normalImage.texture[(pixelY + 1) % h][(pixelX + 1) % w] * ratioX * ratioY;

			cyPoint3d t;
			t = (2 * color[0] - 1) * N[0] + (2 * color[1] - 1) * N[1] + (2 * color[2] - 1) * N[2];
			t = color*2.0 - 1.0;
			normalAtHit = t;
			break;
		}
		}
	}

	return normalAtHit.GetNormalized();
}

cyPoint3d Quadric :: computeAmbientColor()
{
	return ambientFact * ambientColor;
}

cyPoint3d Quadric :: computeDiffuseColor(cyPoint3d& normalAtHit, cyPoint3d& hitPointToLight, cyPoint3d& lightColor, double pointToLightDist, double spotLightComp, double t)
{
	cyPoint3d color = { 0, 0, 0 };
	double diffuseComp = t == 0.0 ? clamp(normalAtHit.Dot(hitPointToLight), 0, 0.5) : clamp(t, 0, 1);
	diffuseComp *= spotLightComp;
	color = diffuseComp * diffuseColor * (diffuseFact * 100) * lightColor / (pow(pointToLightDist, 2));
	return color;
}

cyPoint3d Quadric :: computeSpecularColor(cyPoint3d& normalAtHit, cyPoint3d& hitPointToLight, cyPoint3d& lightColor, cyPoint3d& eyeToHitPoint, double spotLightComp)
{
	cyPoint3d color = { 0, 0, 0 };
	cyPoint3d lightReflect = -hitPointToLight + 2 * (normalAtHit.Dot(hitPointToLight)) * normalAtHit;
	lightReflect.Normalize();
	double specularComp = clamp((-lightReflect).Dot(eyeToHitPoint), 0.94, 0.96);
	specularComp *= spotLightComp;
	color = specularComp * specularColor * lightColor * specularFact;
	return color;
}

cyPoint3d Quadric :: computeBorderColor(cyPoint3d& normalAtHit, cyPoint3d& eyeToHitPoint, double spotLightComp)
{
	cyPoint3d color = { 0, 0, 0 };
	double borderComp = eyeToHitPoint.Dot(normalAtHit) + 1;
	borderComp *= spotLightComp;
	color = clamp(borderComp, 0.84, 0.85) * borderColor * borderFact;
	return color;
}

cyPoint3d Quadric :: computeTextureColor(cyPoint3d hitPoint, cyPoint3d normalAtHit, bool env)
{
	if (textureImage.data != NULL)
	{
		cyPoint3d color = { 0, 0, 0 };
		double u, v, ratioX, ratioY;
		int pixelX, pixelY;
		int w, h;

		switch (type)
		{
			//plane
		case 0:
		{
			w = textureImage.width;
			h = textureImage.height;

			u = (1 - s - t)*texpoints[0][0] + s*texpoints[1][0] + t*texpoints[2][0];
			v = (1 - s - t)*texpoints[0][1] + s*texpoints[1][1] + t*texpoints[2][1];
			u = u * w;
			v = v * h;
			pixelX = (int)(u);
			pixelY = (int)(v);

			ratioX = u - pixelX;
			ratioY = v - pixelY;

			//color = textureImage.texture[(pixelY + 0) % h][(pixelX + 0) % w];
			color = textureImage.texture[(pixelY + 0) % h][(pixelX + 0) % w] * (1 - ratioX) * (1 - ratioY) +
				textureImage.texture[(pixelY + 1) % h][(pixelX + 0) % w] * (1 - ratioX) * ratioY +
				textureImage.texture[(pixelY + 0) % h][(pixelX + 1) % w] * ratioX * (1 - ratioY) +
				textureImage.texture[(pixelY + 1) % h][(pixelX + 1) % w] * ratioX * ratioY;

			break;
		}
		//sphere
		case 1:
		{
			double phi, theta, tempX;

			w = textureImage.width;
			h = textureImage.height;

			if (env)
				phi = N[2].Dot(normalAtHit) / si[2];
			else
				phi = N[2].Dot(hitPoint - qc) / si[2];
			phi = acos(phi);
			v = phi / cy::cyPi<double>();

			if (env)
				theta = N[1].Dot(normalAtHit) / si[1];
			else
				theta = N[1].Dot(hitPoint - qc) / si[1];
			theta = theta / sin(phi);
			theta = acos(theta);

			if (env)
				tempX = N[0].Dot(normalAtHit) / si[0];
			else
				tempX = N[0].Dot(hitPoint - qc) / si[0];
			theta = tempX >= 0 ? theta : (cy::cyPi<double>() * 2) - theta;
			u = theta / (cy::cyPi<double>() * 2);

			//handle negative dot product

			v = v * h;
			u = u * w;
			pixelY = (int)(v);
			pixelX = (int)(u);
			ratioY = v - pixelY;
			ratioX = u - pixelX;

			color = textureImage.texture[(pixelY + 0) % h][(pixelX + 0) % w] * (1 - ratioX) * (1 - ratioY) +
				textureImage.texture[(pixelY + 1) % h][(pixelX + 0) % w] * (1 - ratioX) * ratioY +
				textureImage.texture[(pixelY + 0) % h][(pixelX + 1) % w] * ratioX * (1 - ratioY) +
				textureImage.texture[(pixelY + 1) % h][(pixelX + 1) % w] * ratioX * ratioY;

			break;
		}
		}

		diffuseColor = color;
		specularColor = color;
		refractive_index = color[0] > 0.5 ? 1.5 : 2.0;

		return color;
	}
}