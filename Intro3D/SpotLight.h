// =========================================================================================
//	SpotLight.h
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

// Our spotlight class. Same as pointlight but only affects a certain radius.
class SpotLight : public Light
{
	public:
		SpotLight(Color color, float atnA, float atnB, float atnC, float innerRadius, float outerRadius);

		void SetAttenuation(float a, float b, float c);
		void GetAttenuation(float& a, float& b, float& c);
		
		float SmoothStep(float edge0, float edge1, float x);
	
		void SetAngle(float inner, float outer);
		void GetAngle(float& inner, float& outer);

	protected:
		float _attenuationA, _attenuationB, _attenuationC;
		float _innerRadius, _outerRadius;
};

