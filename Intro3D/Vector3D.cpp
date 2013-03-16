// =========================================================================================
//	Vector3D.cpp
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#include "StdAfx.h"
#include "Vector3D.h"

// Constructors.
Vector3D::Vector3D(void)
{
	Init(0.0f, 0.0f, 0.0f, 0.0f);
}
Vector3D::Vector3D(float x, float y, float z)
{
	Init(x, y, z, 0.0f);
}
Vector3D::Vector3D(const Vector3D& v)
{
	Copy(v);
}

// Destructor.
Vector3D::~Vector3D(void)
{
}

// Returns the length of the vector.
float Vector3D::GetLength()
{
	return sqrt((_x * _x) + (_y * _y) + (_z * _z));
}

// Normalizes the value of the vector.
void Vector3D::Normalize()
{
	float length = GetLength();
	_x = _x / length;
	_y = _y / length;
	_z = _z / length;
}

// Work out the dot product of 2 vectors.
float Vector3D::DotProduct(Vector3D v1, Vector3D v2)
{
	float val = (v1._x * v2._x) + (v1._y * v2._y) + (v1._z * v2._z);
	return val;
}

// Works out the cross product of 2 vectors.
Vector3D Vector3D::CrossProduct(Vector3D v1, Vector3D v2)
{
	return Vector3D((v1._y * v2._z) - (v1._z * v2._y),
					(v1._z * v2._x) - (v1._x * v2._z),
					(v1._x * v2._y) - (v1._y * v2._x));
}
