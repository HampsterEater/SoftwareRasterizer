// =========================================================================================
//	Vertex.cpp
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#include "StdAfx.h"
#include "Vertex.h"

// Constructors.
Vertex::Vertex(void)
{
	Init(0.0f, 0.0f, 0.0f, 1.0f);
	_normal = Vector3D(0,0,0);
	_normalCount = 0;
	_color = Gdiplus::Color::Black;
}
Vertex::Vertex(float x, float y, float z, float w, Gdiplus::Color color, Vector3D normal, int normalCount)
{
	Init(x, y, z, w);
	_normal = normal;
	_normalCount = normalCount;
	_color = color;
}
Vertex::Vertex(const Vertex& v)
{
	Copy(v);
}

// Destructor.
Vertex::~Vertex(void)
{
}

// Accessor methods. Simple get/set code.
void Vertex::SetUVCoordinate(UVCoordinate value)
{
	_uvCoordinate = value;
}
const UVCoordinate Vertex::GetUVCoordinate()
{
	return _uvCoordinate;
}	
void Vertex::SetNormal(Vector3D normal)
{
	_normal = normal;
}
const Vector3D Vertex::GetNormal()
{
	return _normal;
}		
void Vertex::SetColor(Gdiplus::Color color)
{
	_color = color;
}
const Gdiplus::Color Vertex::GetColor()
{
	return _color;
}
void Vertex::SetNormalCount(int count)
{
	_normalCount = count;
}
const int Vertex::GetNormalCount()
{
	return _normalCount;
}

// Gets the vector between 2 vertexs.
Vector3D Vertex::GetVector(Vertex v1, Vertex v2)
{
	return Vector3D(v2.GetX() - v1.GetX(), 
					v2.GetY() - v1.GetY(), 
					v2.GetZ() - v1.GetZ());
}

// Copys the value of this vertex to another.
void Vertex::Copy(const Vertex& v)
{	
	_x = v._x;
	_y = v._y;
	_z = v._z;
	_w = v._w;
	_preTransformZ = v._preTransformZ;
	_color = v._color;
	_normal = v._normal;
	_normalCount = v._normalCount;
	_uvCoordinate = v._uvCoordinate;
}
