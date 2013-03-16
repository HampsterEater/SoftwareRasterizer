// =========================================================================================
//	Light.h
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#pragma once

#include "stdafx.h"
#include "Vector3D.h"
#include <GdiPlus.h>

using namespace Gdiplus;

// Our base light class. All light objects are extended from this one.
class Light
{
	public:
		Light();
		~Light(void);

		void SetPosition(Vertex value);
		Vertex GetPosition();
		
		void SetIntensity(Color color);
		Color GetIntensity();

		void SetEnabled(bool enabled);
		bool GetEnabled();

	protected:
		Vertex _position;
		Color _intensity;
		bool _enabled;
};

