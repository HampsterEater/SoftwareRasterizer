// =========================================================================================
//	Matrix3D.h
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#pragma once
#include "Vertex.h"

// This is the matrix class it's responsible for all matrix manipulation and calculations.
class Matrix3D
{
	public:
		Matrix3D(void);
		Matrix3D(float, float, float, float,
				 float, float, float, float,
				 float, float, float, float,
				 float, float, float, float);
		Matrix3D(const Matrix3D&);
		~Matrix3D();

		Matrix3D& operator= (const Matrix3D &rhs);

		const Matrix3D operator*(const Matrix3D &other) const;
		const Vertex operator*(const Vertex &p) const;

		static Matrix3D TranslateMatrix(float x, float y, float z);
		static Matrix3D ScaleMatrix(float x, float y, float z);
		static Matrix3D RotateXMatrix(float angle);
		static Matrix3D RotateYMatrix(float angle);
		static Matrix3D RotateZMatrix(float angle);
		static Matrix3D RotateMatrix(float x, float y, float z);
		static Matrix3D IdentityMatrix();
		static Matrix3D ZeroMatrix();

	private:
		float _elements[4][4];
		
		void Copy(const Matrix3D& mat);
};
