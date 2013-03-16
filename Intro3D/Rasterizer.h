// =========================================================================================
//	Rasterizer.h
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#pragma once
#include "Vertex.h"
#include "Model3D.h"

using namespace Gdiplus;

// This struct is used to store the values
// needed to render a polygons scanline.
struct ScanLine
{
	float xStart;
	float xEnd;

	float redStart;
	float greenStart;
	float blueStart;

	float redEnd;
	float greenEnd;
	float blueEnd;

	float uStart;
	float vStart;
	float zStart;

	float uEnd;
	float vEnd;
	float zEnd;

	float xNormalStart;
	float yNormalStart;
	float zNormalStart;

	float xNormalEnd;
	float yNormalEnd;
	float zNormalEnd;

	float pixelXStart;
	float pixelYStart;
	float pixelZStart;

	float pixelXEnd;
	float pixelYEnd;
	float pixelZEnd;
};

// This is the rasterizer class, it is responsible for rendering 
// everything to the screen.
class Rasterizer
{
	public:
		Rasterizer(unsigned int width, unsigned int height);
		~Rasterizer(void);

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		Bitmap * GetBitmap() const;
		unsigned int GetPolygonsRendered();
		void ResetPolygonsRendered();

		void BeginLockBits();
		void FinishLockBits();
		void WritePixel(int x, int y, Color color);

		void Clear(const Color& color);
		
		void DrawLine(float x1, float y1, float x2, float y2);
		void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, Gdiplus::Color color);
		
		void FillPolygon(Vertex v1, Vertex v2, Vertex v3, Gdiplus::Color color);
		void FillPolygonShaded(Vertex v1, Vertex v2, Vertex v3, Gdiplus::Color color);
		void FillPolygonTextured(Vertex v1, Vertex v2, Vertex v3, Gdiplus::Color color, Model3D& model);
		void FillPolygonTexturedNormalMapped(Vertex v1, Vertex v2, Vertex v3, Gdiplus::Color color, Model3D& model, std::vector<DirectionalLight*> directionalLights, std::vector<AmbientLight*> ambientLights, std::vector<PointLight*> pointLights);
		void InterpolateScanline(ScanLine* scanlines, Vertex v1, Vertex v2);

		void DrawWireFrame(Model3D& model);
		void DrawSolidFlat(Model3D& model);
		void DrawSolidShaded(Model3D& model);
		void DrawSolidTextured(Model3D& model);
		void DrawSolidTexturedNormalMapped(Model3D& model, std::vector<DirectionalLight*> directionalLights, std::vector<AmbientLight*> ambientLights, std::vector<PointLight*> pointLights);

		void DrawText(float x, float y, const WCHAR* string);

	private:
		unsigned int _width;
		unsigned int _height;
		unsigned int _polygonsRendered;
		Bitmap * _bitmap;
		Graphics * _graphics; 
		BitmapData _bitmapData;

		bool _bitsLocked;

		// Private constructor. Should not be used directly.
		Rasterizer(void);
};

