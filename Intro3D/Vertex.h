// =========================================================================================
//	Vertex.h
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#pragma once

#include "StdAfx.h"
#include "Point3D.h"
#include "Vector3D.h"
#include "UVCoordinate.h"

// This class is used to define a vertex of a model in 3d space.
class Vertex : public Point3D
{
	public:

		Vector3D _normal;
		int _normalCount;
		Gdiplus::Color _color;
		UVCoordinate _uvCoordinate;

		Vertex(void);
		Vertex(float x, float y, float z, float w, Gdiplus::Color color, Vector3D normal, int normalCount);
		Vertex(const Vertex& v);
		~Vertex(void);
		
		void SetUVCoordinate(UVCoordinate coord);
		const UVCoordinate GetUVCoordinate();
		
		void SetNormal(Vector3D normal);
		const Vector3D GetNormal();
		
		void SetColor(Gdiplus::Color color);
		const Gdiplus::Color GetColor();

		void SetNormalCount(int count);
		const int GetNormalCount();

		static Vector3D GetVector(Vertex v1, Vertex v2);

	private:

		void Copy(const Vertex& v);
};

