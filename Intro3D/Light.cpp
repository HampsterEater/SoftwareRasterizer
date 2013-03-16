// =========================================================================================
//	Light.cpp
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#include "StdAfx.h"
#include "Light.h"

// Constructor.
Light::Light()
{
	_intensity = Color(255, 255, 255);
}

// Destructor.
Light::~Light(void)
{

}

// Accessor methods. Simple get/set code.
void Light::SetIntensity(Color value)
{
	_intensity = value;
}
Color Light::GetIntensity()
{
	return _intensity;
}
void Light::SetPosition(Vertex value)
{
	_position = value;
}
Vertex Light::GetPosition()
{
	return _position;
}
void Light::SetEnabled(bool enabled)
{
	_enabled = enabled;
}
bool Light::GetEnabled()
{
	return _enabled;
}