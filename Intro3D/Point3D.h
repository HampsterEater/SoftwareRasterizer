// =========================================================================================
//	Point3D.h
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#pragma once

// This class represents a point in 3d space.
class Point3D
{
	public:
		Point3D(void);
		Point3D(float x, float y, float z, float w);
		Point3D(const Point3D& v);
		~Point3D(void);

		float GetX(void) const;
		void SetX(float x);
		float GetY(void) const;
		void SetY(float y);
		float GetZ(void) const;
		void SetZ(float z);
		float GetW(void) const;
		void SetW(float w);
		void SetPreTransformZ(float value);
		float GetPreTransformZ() const;

		void Dehomogenize();

		Point3D& operator= (const Point3D &rhs);

	protected:
		float _x;
		float _y;
		float _z;
		float _w;
		float _preTransformZ;

		void Init(float x, float y, float z, float w);
		virtual void Copy(const Point3D& v);
};

