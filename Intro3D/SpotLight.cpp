// =========================================================================================
//	SpotLight.cpp
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#include "StdAfx.h"
#include "Light.h"
#include "SpotLight.h"

// Initializes the values of this spot light to those given.
SpotLight::SpotLight(Color color, float atnA, float atnB, float atnC, float innerRadius, float outerRadius)
{
	_intensity = color;
	_attenuationA = atnA;
	_attenuationB = atnB;
	_attenuationC = atnC;
}

// Accessor methods. Simple get/set code
void SpotLight::SetAttenuation(float a, float b, float c)
{
	_attenuationA = a;
	_attenuationB = b;
	_attenuationC = c;
}
void SpotLight::GetAttenuation(float& a, float& b, float& c)
{
	a = _attenuationA;
	b = _attenuationB;
	c = _attenuationC;
}
		
// Smooth step function, steps between 2 edges.
float SpotLight::SmoothStep(float edge0, float edge1, float x)
{
	float result;
	
	if (x < edge0)
	{
		result = 0.0f;
	}
	else if (x >= edge1) 
	{
		result = 1.0f;
	}
	else 
	{
		float t = (x - edge0) / (edge1 - edge0);
		result = (3.0f-2.0f*t) * (t*t);
	}

	return result;
}
	
void SpotLight::SetAngle(float inner, float outer)
{
	_innerRadius = inner;
	_outerRadius = outer;
}

void SpotLight::GetAngle(float& inner, float& outer)
{
	inner = _innerRadius;
	outer = _outerRadius;
}
