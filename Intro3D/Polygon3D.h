// =========================================================================================
//	Polygon3D.h
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#pragma once
#include "Vertex.h"
#include <GdiPlus.h>

using namespace Gdiplus;

// This is the polygon class, it defines all the values needed to 
// render a polygon to the screen.
class Polygon3D
{
	public:
		Polygon3D(void);
		Polygon3D(int, int, int);
		Polygon3D(const Polygon3D&);
		~Polygon3D();

		Polygon3D& operator= (const Polygon3D &rhs);

		int GetVertexIndex(int index);
		void SetVertexIndex(int index, int value);
		
		int GetUVIndex(int index);
		void SetUVIndex(int index, int value);

		void SetBackfacing(bool value);
		bool GetBackfacing();

		void SetAvgDepth(float value);
		float GetAvgDepth() const;
		
		void SetColor(Color value);
		Color GetColor();

	private:
		int _vertexIndexes[3];
		int _uvIndexes[3];
		bool _backfacing;
		float _avgDepth;
		Color _color;
		
		void Copy(const Polygon3D& poly);
};
