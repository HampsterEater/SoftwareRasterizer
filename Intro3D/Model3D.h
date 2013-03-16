// =========================================================================================
//	Model3D.h
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#pragma once
#include "stdafx.h"
#include "Vertex.h"
#include "Polygon3D.h"
#include "Matrix3D.h"
#include "Rasterizer.h"
#include "Camera.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "AmbientLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "UVCoordinate.h"
#include <vector>

// PCXHeader structure, used to load and store
// the values contained in the header of a pcx file.
struct PcxHeader
{
	BYTE  ID;
	BYTE  Version;
	BYTE  Encoding;
	BYTE  BitsPerPixel;
	short XMin;
	short YMin;
	short XMax;
	short YMax;
	short HRes;
	short VRes;
	BYTE  ClrMap[16*3];
	BYTE  Reserved;
	BYTE  NumPlanes;
	short BytesPerLine;
	short Pal;
	BYTE  Filler[58];
};

// This is the model class it's responsible for storing all the data required
// to render a model to the screne.
class Model3D
{
	public:
		Model3D(void);
		~Model3D();

		std::vector<Polygon3D>& GetPolygonList();
		std::vector<Vertex>& GetVertexList();
		std::vector<UVCoordinate>& GetUVCoordinateList();
		std::vector<Vertex>& GetTransformedVertexList();
		
		void SetTexture(BYTE* texture, Gdiplus::Color* palette, int textureWidth);
		void GetTexture(BYTE** texture, Gdiplus::Color** palette, int* textureWidth);
		
		void SetNormalMapTexture(BYTE* texture, Gdiplus::Color* palette, int textureWidth);
		void GetNormalMapTexture(BYTE** texture, Gdiplus::Color** palette, int* textureWidth);

		void SetNormalMapOn(bool val);
		bool GetNormalMapOn();

		void ApplyTransformToLocalVertices(const Matrix3D& transform);
		void ApplyTransformToTransformedVertices(const Matrix3D& transform);
		void RebuildTransformedVerticesList();
		
		void DehomogenizeTransformedVertices();

		void CalculateBackfaces(Camera* camera);
		void DepthSort();

		void SetReflectionCoefficients(float r, float g, float b);
		void GetReflectionCoefficients(float& r, float& g, float& b);

		void ResetLighting();
		void CalculateLightingDirectional(std::vector<DirectionalLight*> lights);
		void CalculateLightingAmbient(std::vector<AmbientLight*> lights);
		void CalculateLightingPoint(std::vector<PointLight*> lights);
		void CalculateLightingSpot(std::vector<SpotLight*> lights);
		
		Gdiplus::Color CalculateLightingDirectionalPerPixel(std::vector<DirectionalLight*> lights, Vertex pixelPosition, Vector3D pixelNormal, Gdiplus::Color startColor);
		Gdiplus::Color CalculateLightingAmbientPerPixel(std::vector<AmbientLight*> lights, Vertex pixelPosition, Vector3D pixelNormal, Gdiplus::Color startColor);
		Gdiplus::Color CalculateLightingPointPerPixel(std::vector<PointLight*> lights, Vertex pixelPosition, Vector3D pixelNormal, Gdiplus::Color startColor);

		void CalculateVertexNormals();

	private:
		std::vector<Polygon3D> _polygons;
		std::vector<Vertex> _vertices;	
		std::vector<UVCoordinate> _uvCoordinates;

		std::vector<Vertex> _transformedVertices;

		float _kd_red, _kd_green, _kd_blue; // Reflection coefficients

		BYTE* _texture;
		Gdiplus::Color* _palette;
		int _textureWidth;

		BYTE* _normalMapTexture;
		Gdiplus::Color* _normalMapPalette;
		int _normalMapTextureWidth;

		bool _normalMapOn;
};
