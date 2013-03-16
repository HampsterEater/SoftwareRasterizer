// =========================================================================================
//	PointLight.cpp
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#include "StdAfx.h"
#include "Light.h"
#include "PointLight.h"

// Initializes the values of this point light to those given.
PointLight::PointLight(Color color, float atnA, float atnB, float atnC)
{
	_intensity = color;
	_attenuationA = atnA;
	_attenuationB = atnB;
	_attenuationC = atnC;
}

// Accessor methods. Simple get/set code
void PointLight::SetAttenuation(float a, float b, float c)
{
	_attenuationA = a;
	_attenuationB = b;
	_attenuationC = c;
}
void PointLight::GetAttenuation(float& a, float& b, float& c)
{
	a = _attenuationA;
	b = _attenuationB;
	c = _attenuationC;
}