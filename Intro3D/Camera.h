// =========================================================================================
//	Camera.h
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#pragma once
#include "Vertex.h"
#include "Matrix3D.h"

// The camera class is responsible for defining a viewing position 
// that the scene can be rendered from.
class Camera
{
	public:
		Camera(float xRotation, float yRotation, float zRotation,
			   const Vertex& position,
			   unsigned int viewWidth, unsigned int viewHeight);
		~Camera();

		Matrix3D GetViewportMatrix();
		Matrix3D GetPerspectiveMatrix();
		Matrix3D GetScreenMatrix();

		void SetRotation(float xRotation, float yRotation, float zRotation);
		void GetRotation(float& xRotation, float& yRotation, float& zRotation);

		void SetPosition(Vertex& position);
		Vertex GetPosition();

		void SetViewSize(unsigned int viewWidth, unsigned int viewHeight);
		void GetViewSize(unsigned int& viewWidth, unsigned int& viewHeight);

		void RebuildMatrices();

	private:
		Matrix3D _viewportMatrix;
		Matrix3D _perspectiveMatrix;
		Matrix3D _screenMatrix;

		float _xRotation, _yRotation, _zRotation;
		Vertex _position;
		int _viewWidth, _viewHeight;
};
