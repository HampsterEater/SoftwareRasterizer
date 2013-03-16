// =========================================================================================
//	Camera.cpp
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#include "StdAfx.h"
#include "Camera.h"
#include "Matrix3D.h"

// Constructor.
Camera::Camera(float xRotation, float yRotation, float zRotation,
			   const Vertex& position,
			   unsigned int viewWidth, unsigned int viewHeight)
{
	_xRotation = xRotation;
	_yRotation = yRotation;
	_zRotation = zRotation;
	_position = position;
	_viewWidth = viewWidth;
	_viewHeight = viewHeight;
	RebuildMatrices();
}

// Destructor.
Camera::~Camera()
{
}

// Accessor methods. Just a lot of simple get/set functions.
Matrix3D Camera::GetViewportMatrix()
{
	return _viewportMatrix;
}
Matrix3D Camera::GetPerspectiveMatrix()
{
	return _perspectiveMatrix;
}
Matrix3D Camera::GetScreenMatrix()
{
	return _screenMatrix;
}
void Camera::SetRotation(float xRotation, float yRotation, float zRotation)
{
	_xRotation = xRotation;
	_yRotation = yRotation;
	_zRotation = zRotation;
	RebuildMatrices();
}
void Camera::GetRotation(float& xRotation, float& yRotation, float& zRotation)
{
	xRotation = _xRotation;
	yRotation = _yRotation;
	zRotation = _zRotation;
}
void Camera::SetPosition(Vertex& position)
{
	_position = position;
	RebuildMatrices();
}
Vertex Camera::GetPosition()
{
	return _position;
}
void Camera::SetViewSize(unsigned int viewWidth, unsigned int viewHeight)
{
	_viewWidth = viewWidth;
	_viewHeight = viewHeight;
	RebuildMatrices();
}
void Camera::GetViewSize(unsigned int& viewWidth, unsigned int& viewHeight)
{
	viewWidth = _viewWidth;
	viewHeight = _viewHeight;
}

// This method rebuilds the viewport, perspective and screen matrices, ready 
// to be used during rendering.
void Camera::RebuildMatrices()
{
	// Work out the rotation and transformation matrices.
	Matrix3D rotateX = Matrix3D
		(
			1, 0, 0, 0,
			0, cos(_xRotation), sin(_xRotation), 0,
			0, -sin(_xRotation), cos(_xRotation), 0,
			0, 0, 0, 1
		);

	Matrix3D rotateY = Matrix3D
		(
			cos(_yRotation), 0, -sin(_yRotation), 0,
			0, 1, 0, 0,
			sin(_yRotation), 0, cos(_yRotation), 0,
			0, 0, 0, 1
		);

	Matrix3D rotateZ = Matrix3D
		(
			cos(_zRotation), sin(_zRotation), 0, 0,
			-sin(_zRotation), cos(_zRotation), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	Matrix3D transform = Matrix3D
		(
			1, 0, 0, -_position.GetX(),
			0, 1, 0, -_position.GetY(),
			0, 0, 1, -_position.GetZ(),
			0, 0, 0, 1
		);

	// Rebuild the viewport, perspective and screen matrices.
	_viewportMatrix = ((transform * rotateZ) * rotateY) * rotateX;

	_perspectiveMatrix = Matrix3D(1, 0, 0, 0,
								  0, 1, 0, 0,
								  0, 0, 1, 0,
								  0, 0, 1, 0);

	_screenMatrix = Matrix3D((float)(_viewWidth / 2), 0, 0, (float)(_viewWidth / 2),
							 0, (float)(-(_viewHeight / 2)), 0, (float)(_viewHeight / 2), 
							 0, 0, 1, 0,
							 0, 0, 0, 1);
}
