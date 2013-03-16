// =========================================================================================
//	Polygon3D.cpp
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#include "StdAfx.h"
#include "Polygon3D.h"

// Contructors.
Polygon3D::Polygon3D(void)
{
	_avgDepth = 0.0f;
}
Polygon3D::Polygon3D(int i1, int i2, int i3)
{
	_vertexIndexes[0] = i1;
	_vertexIndexes[1] = i2;
	_vertexIndexes[2] = i3;
	_avgDepth = 0.0f;
}

// Copy constructor.
Polygon3D::Polygon3D(const Polygon3D& poly)
{
	_avgDepth = 0.0f;
	Copy(poly);
}

// Destructor.
Polygon3D::~Polygon3D()
{

}

// Assignment operator. Copys the value of one polygon to another.
Polygon3D& Polygon3D::operator= (const Polygon3D &rhs)
{
	if (this != &rhs)
	{
		// Only copy if we are not assigning to ourselves. 
		// (remember that a = b is the same as a.operator=(b))
		Copy(rhs);
	}
	return *this;
}

// Accessor methods. Simple get/set code.
int Polygon3D::GetVertexIndex(int index)
{
	return _vertexIndexes[index];
}
void Polygon3D::SetVertexIndex(int index, int value)
{
	_vertexIndexes[index] = value;
}
int Polygon3D::GetUVIndex(int index)
{
	return _uvIndexes[index];
}
void Polygon3D::SetUVIndex(int index, int value)
{
	_uvIndexes[index] = value;
}
void Polygon3D::SetBackfacing(bool value)
{
	_backfacing = value;
}
bool Polygon3D::GetBackfacing()
{
	return _backfacing;
}
void Polygon3D::SetAvgDepth(float value)
{
	_avgDepth = value;
}
float Polygon3D::GetAvgDepth() const 
{
	return _avgDepth;
}
void Polygon3D::SetColor(Color value)
{
	_color = value;
}
Color Polygon3D::GetColor()
{
	return _color;
}

// Copys the value of this polygon to another polygon.
void Polygon3D::Copy(const Polygon3D& poly)
{
	_vertexIndexes[0] = poly._vertexIndexes[0];
	_vertexIndexes[1] = poly._vertexIndexes[1];
	_vertexIndexes[2] = poly._vertexIndexes[2];
	_uvIndexes[0] = poly._uvIndexes[0];
	_uvIndexes[1] = poly._uvIndexes[1];
	_uvIndexes[2] = poly._uvIndexes[2];
	_backfacing = poly._backfacing;
	_avgDepth = poly._avgDepth;
	_color = poly._color;
}
