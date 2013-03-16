// =========================================================================================
//	Matrix3D.cpp
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#include "StdAfx.h"
#include "Matrix3D.h"

// Constructor. Initializes the matrix to an identity matrix.
Matrix3D::Matrix3D(void)
{
	// Set the matrix to an identity matrix.
	_elements[0][0] = 1.0f;
	_elements[0][1] = 0.0f;
	_elements[0][2] = 0.0f;
	_elements[0][3] = 0.0f;

	_elements[1][0] = 0.0f;
	_elements[1][1] = 1.0f;
	_elements[1][2] = 0.0f;
	_elements[1][3] = 0.0f;

	_elements[2][0] = 0.0f;
	_elements[2][1] = 0.0f;
	_elements[2][2] = 1.0f;
	_elements[2][3] = 0.0f;

	_elements[3][0] = 0.0f;
	_elements[3][1] = 0.0f;
	_elements[3][2] = 0.0f;
	_elements[3][3] = 1.0f;
}

// Constructor. Initializes the matrix to the values given.
Matrix3D::Matrix3D(float a0, float a1, float a2, float a3,
				 float b0, float b1, float b2, float b3,
				 float c0, float c1, float c2, float c3,
				 float d0, float d1, float d2, float d3)
{
	_elements[0][0] = a0;
	_elements[1][0] = a1;
	_elements[2][0] = a2;
	_elements[3][0] = a3;

	_elements[0][1] = b0;
	_elements[1][1] = b1;
	_elements[2][1] = b2;
	_elements[3][1] = b3;

	_elements[0][2] = c0;
	_elements[1][2] = c1;
	_elements[2][2] = c2;
	_elements[3][2] = c3;

	_elements[0][3] = d0;
	_elements[1][3] = d1;
	_elements[2][3] = d2;
	_elements[3][3] = d3;
}

// Copy constructor. Copys the matrix from another matrix.
Matrix3D::Matrix3D(const Matrix3D& mat)
{
	Copy(mat);
}

// Destructor.
Matrix3D::~Matrix3D()
{

}

// Assignment operator. Copys this matrix to another matrix.
Matrix3D& Matrix3D::operator= (const Matrix3D &rhs)
{
	if (this != &rhs)
	{
		// Only copy if we are not assigning to ourselves. 
		// (remember that a = b is the same as a.operator=(b))
		Copy(rhs);
	}
	return *this;
}

// Multiplication operator. Multiplys this matrix with another matrix and returns the result.
const Matrix3D Matrix3D::operator*(const Matrix3D &other) const
{
	Matrix3D newMatrix;

	for (int column = 0; column < 4; column++)
	{
		for (int row = 0; row < 4; row++)
		{
			float total = 0;

			for (int row2 = 0; row2 < 4; row2++)
				total += _elements[column][row2] * other._elements[row2][row];
		
			newMatrix._elements[column][row] = total;			
		}
	}

	return newMatrix;
}

// Multiplication operator. Multiplies this matrix with a vertex and returns the Vertex result.
const Vertex Matrix3D::operator*(const Vertex &p) const
{
	Gdiplus::Color color = p._color;
	Vector3D normal = p._normal;
	int normalCount = p._normalCount;

	float x = (_elements[0][0] * p.GetX()) + (_elements[1][0] * p.GetY()) + (_elements[2][0] * p.GetZ()) + (_elements[3][0] * p.GetW());
	float y = (_elements[0][1] * p.GetX()) + (_elements[1][1] * p.GetY()) + (_elements[2][1] * p.GetZ()) + (_elements[3][1] * p.GetW());
	float z = (_elements[0][2] * p.GetX()) + (_elements[1][2] * p.GetY()) + (_elements[2][2] * p.GetZ()) + (_elements[3][2] * p.GetW());
	float w = (_elements[0][3] * p.GetX()) + (_elements[1][3] * p.GetY()) + (_elements[2][3] * p.GetZ()) + (_elements[3][3] * p.GetW());
	float preZ = p.GetPreTransformZ();

	Vertex vert =  Vertex(x, y, z, w, color, normal, normalCount);
	vert.SetPreTransformZ(preZ);

	return vert;
}

// Copys the values from one matrix to another.
void Matrix3D::Copy(const Matrix3D& mat)
{
	_elements[0][0] = mat._elements[0][0];
	_elements[1][0] = mat._elements[1][0];
	_elements[2][0] = mat._elements[2][0];
	_elements[3][0] = mat._elements[3][0];

	_elements[0][1] = mat._elements[0][1];
	_elements[1][1] = mat._elements[1][1];
	_elements[2][1] = mat._elements[2][1];
	_elements[3][1] = mat._elements[3][1];

	_elements[0][2] = mat._elements[0][2];
	_elements[1][2] = mat._elements[1][2];
	_elements[2][2] = mat._elements[2][2];
	_elements[3][2] = mat._elements[3][2];

	_elements[0][3] = mat._elements[0][3];
	_elements[1][3] = mat._elements[1][3];
	_elements[2][3] = mat._elements[2][3];
	_elements[3][3] = mat._elements[3][3];
}

// Creates a translation matrix given the axis coordinates.
Matrix3D Matrix3D::TranslateMatrix(float x, float y, float z)
{
	return Matrix3D
		(
			1.0f, 0.0f, 0.0f, x,
			0.0f, 1.0f, 0.0f, y,
			0.0f, 0.0f, 1.0f, z,
			0.0f, 0.0f, 0.0f, 1
		);
}

// Creates a scale matrix given the axis coordinates.
Matrix3D Matrix3D::ScaleMatrix(float x, float y, float z)
{
	return Matrix3D
		(
			x, 0.0f, 0.0f, 0.0f,
			0.0f, y, 0.0f, 0.0f,
			0.0f, 0.0f, z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
}

// Creates a rotation matrix on the x axis with the given rotation angle.
Matrix3D Matrix3D::RotateXMatrix(float angle)
{
	return Matrix3D
		(
			1, 0, 0, 0,
			0, cos(angle), -sin(angle), 0,
			0, sin(angle), cos(angle), 0,
			0, 0, 0, 1
		);
}

// Creates a rotation matrix on the y axis with the given rotation angle.
Matrix3D Matrix3D::RotateYMatrix(float angle)
{
	return Matrix3D
		(
			cos(angle), 0, sin(angle), 0,
			0, 1, 0, 0,
			-sin(angle), 0, cos(angle), 0,
			0, 0, 0, 1
		);
}

// Creates a rotation matrix on the z axis with the given rotation angle.
Matrix3D Matrix3D::RotateZMatrix(float angle)
{
	return Matrix3D
		(
			cos(angle), -sin(angle), 0, 0,
			sin(angle), cos(angle), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
}

// Creates a compound rotation matrix with the angles for each axis.
Matrix3D Matrix3D::RotateMatrix(float x, float y, float z)
{
	return RotateXMatrix(x) * RotateYMatrix(y) * RotateZMatrix(z);
}

// Creates and returns a identity matrix.
Matrix3D Matrix3D::IdentityMatrix()
{
	return Matrix3D
		(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
}

// Creates and returns a zero matrix.
Matrix3D Matrix3D::ZeroMatrix()
{
	return Matrix3D
		(
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
		);
}
