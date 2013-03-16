// =========================================================================================
//	PointLight.h
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#pragma once

#include "stdafx.h"
#include "Vector3D.h"
#include <GdiPlus.h>
#include "Light.h"

using namespace Gdiplus;

// Our directional light class. Casts an light from a given direction with attenuation factored in..
class PointLight : public Light
{
	public:
		PointLight(Color color, float atnA, float atnB, float atnC);

		void SetAttenuation(float a, float b, float c);
		void GetAttenuation(float& a, float& b, float& c);

	protected:
		float _attenuationA, _attenuationB, _attenuationC;
};

