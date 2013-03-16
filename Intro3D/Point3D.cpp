// =========================================================================================
//	Point3D.cpp
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#include "StdAfx.h"
#include "Point3D.h"

// Constructors
Point3D::Point3D(void)
{
	Init(0.0f, 0.0f, 0.0f, 1.0f);
}
Point3D::Point3D(float x, float y, float z, float w)
{
	Init(x, y, z, w);
}
Point3D::Point3D(const Point3D& v)
{
	Copy(v);
}
Point3D::~Point3D(void)
{
}

// Accessors and mutators
float Point3D::GetX(void) const
{
	return _x;
}
void Point3D::SetX(float x)
{
	_x = x;
}
float Point3D::GetY(void) const
{
	return _y;
}
void Point3D::SetY(float y)
{
	_y = y;
}
float Point3D::GetZ(void) const
{
	return _z;
}
void Point3D::SetZ(float z)
{
	_z = z;
}
float Point3D::GetW(void) const
{
	return _w;
}
void Point3D::SetW(float w)
{
	_w = w;
}
void Point3D::SetPreTransformZ(float value)
{
	_preTransformZ = value;
}
float Point3D::GetPreTransformZ() const
{
	return _preTransformZ;
}

// Dehomogenizes the coordinates of the point by dividing all axis's by w.
void Point3D::Dehomogenize()
{
	_preTransformZ = _w;
	_x /= _w;
	_y /= _w;
	_z /= _w;
	_w /= _w;
}

// Operator overloads
Point3D& Point3D::operator= (const Point3D &rhs)
{
	if (this != &rhs)
	{
		// Only copy if we are not assigning to ourselves. 
		// (remember that a = b is the same as a.operator=(b))
		Copy(rhs);
	}
	return *this;
}

// Initializes the value of this point to the given values.
void Point3D::Init(float x, float y, float z, float w)
{
	_x = x;
	_y = y;
	_z = z;
	_w = w;
	_preTransformZ = z;
}

// Copys the value of this point to another.
void Point3D::Copy(const Point3D& v)
{
	_x = v._x;
	_y = v._y;
	_z = v._z;
	_w = v._w;
	_preTransformZ = v._preTransformZ;
}


