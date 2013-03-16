// =========================================================================================
//	Vector3D.h
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#pragma once

#include "StdAfx.h"
#include "Point3D.h"

// This class is used to store a vector in 3d space.
class Vector3D : public Point3D
{
	public:
		Vector3D(void);
		Vector3D(float x, float y, float z);
		Vector3D(const Vector3D& v);
		~Vector3D(void);

		float GetLength();
		void Normalize();

		static float DotProduct(Vector3D v1, Vector3D v2);
		static Vector3D CrossProduct(Vector3D v1, Vector3D v2);

	private:

};

