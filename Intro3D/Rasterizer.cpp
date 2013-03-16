// =========================================================================================
//	Rasterizer.cpp
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#include "StdAfx.h"
#include "Rasterizer.h"

// No-Argument Constructor. Declared private, should not get called.
Rasterizer::Rasterizer(void)
{
	// Declared private so should not be called.
	_bitmap = NULL;
	_graphics = NULL;
}

// Constructor. Sets up the rendering bitmap and graphics with the given the width and height.
Rasterizer::Rasterizer(unsigned int width, unsigned int height)
{
	// Create a bitmap of the specified size and get a reference to the graphics object
	// for that bitmap. Note that these calls could theoretically fail so we really should
	// handle that, but we will leave that for now. 
	_width = width;
	_height = height;
	_bitmap = new Bitmap(_width, _height, PixelFormat32bppARGB);
	_graphics = new Graphics(_bitmap);
}

// Destructor.
Rasterizer::~Rasterizer(void)
{
	// Clean up all dynamically created objects
	if (_graphics)
	{
		delete _graphics;
		_graphics = NULL;
	}
	if (_bitmap)
	{
		delete _bitmap;
		_bitmap = NULL;
	}
}

// Accessors
unsigned int Rasterizer::GetWidth() const
{
	return _width;
}
unsigned int Rasterizer::GetHeight() const
{
	return _height;
}
Bitmap * Rasterizer::GetBitmap() const
{
	return _bitmap;
}
unsigned int Rasterizer::GetPolygonsRendered()
{
	return _polygonsRendered;
}
void Rasterizer::ResetPolygonsRendered()
{
	_polygonsRendered = 0;
}

// Clear the bitmap using the specified colour
void Rasterizer::Clear(const Color& color)
{
	if (_graphics)
	{
		FinishLockBits();
		_graphics->Clear(color);	
		BeginLockBits();
	}
}

// Begins rendering a frame, sets everything ready to render.
void Rasterizer::BeginLockBits()
{
	if (_bitsLocked == true)
		return;

	Rect rect(0, 0, _width, _height);

	// Lock the bitmap ready for rendering.
	Status status = _bitmap->LockBits
	(
		&rect,
		ImageLockModeWrite,
		PixelFormat32bppARGB,
		&_bitmapData
	);
	if (status != Ok)
	{
		throw "Error locking bitmap bits.";	
	}

	_bitsLocked = true;
}

// Finishes rendering a frame.
void Rasterizer::FinishLockBits()
{
	if (_bitsLocked == false)
		return;

	// Unlock the bitmaps data.
	_bitmap->UnlockBits(&_bitmapData);

	_bitsLocked = false;
}

// Writes a pixel colour to the screen via the lockbits method.
void Rasterizer::WritePixel(int x, int y, Color color)
{
	INT* pixelPtr = ((INT*)(_bitmapData.Scan0));
	int offset = (y * _bitmapData.Stride / 4 + x);
	INT* pixelOffsetInt = pixelPtr + offset;

	*pixelOffsetInt = (int)color.GetValue();
}

// Draws a line from one point to another.
void Rasterizer::DrawLine(float x1, float y1, float x2, float y2)
{
	Gdiplus::Pen* pen = new Gdiplus::Pen(Color(255, 255, 255, 255));
	FinishLockBits();
	_graphics->DrawLine(pen, x1, y1, x2, y2);
	BeginLockBits();
	delete pen;
}

// Draws a triangle given 3 points and a color.
void Rasterizer::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, Gdiplus::Color color)
{
	Gdiplus::Brush* brush = new Gdiplus::SolidBrush(color);
	Gdiplus::PointF points[3];
	
	points[0] = Gdiplus::PointF(x1, y1);
	points[1] = Gdiplus::PointF(x2, y2);
	points[2] = Gdiplus::PointF(x3, y3);
	FinishLockBits();
	_graphics->FillPolygon(brush, points, 3);
	BeginLockBits();

	_polygonsRendered++;
	
	delete brush;
}

// Fills a polygon given 3 points and a color.
void Rasterizer::FillPolygon(Vertex v1, Vertex v2, Vertex v3, Gdiplus::Color color)
{
	ScanLine* _scanlines = new ScanLine[_height];

	// Set the scanlines to very high and very low values so
	// they will be set on the first set of interpolation.
	for (unsigned int i = 0; i < _height; i++)
	{
		_scanlines[i].xStart = 99999;
		_scanlines[i].xEnd = -99999;
	}

	// Interpolates between each of the vertexs of the polygon and sets the start
	// and end values for each of the scanlines it comes in contact with.
	InterpolateScanline(_scanlines, v1, v2);
	InterpolateScanline(_scanlines, v2, v3);
	InterpolateScanline(_scanlines, v3, v1);
	
	// Go through each scanline and each pixel in the scanline and 
	// sets its color.
	for (unsigned int y = 0; y < _height; y++)
	{
		for (int x = (int)_scanlines[y].xStart; x <= (int)_scanlines[y].xEnd; x++)
		{
			if (x < 0 || x >= (int)_width)
				continue;

			WritePixel(x, y, color);
			//_bitmap->SetPixel(x, y, color);
		}
	}

	// Dispose of dynamic objects.
	delete[] _scanlines;

	_polygonsRendered++;
}

// Fills a polygon using gouraud shading given 3 points and a color.
void Rasterizer::FillPolygonShaded(Vertex v1, Vertex v2, Vertex v3, Gdiplus::Color color)
{
	ScanLine* _scanlines = new ScanLine[_height];
	
	// Set the scanlines to very high and very low values so
	// they will be set on the first set of interpolation.
	for (unsigned int i = 0; i < _height; i++)
	{
		_scanlines[i].xStart = 99999;
		_scanlines[i].xEnd = -99999;
	}
	
	// Interpolates between each of the vertexs of the polygon and sets the start
	// and end values for each of the scanlines it comes in contact with.
	InterpolateScanline(_scanlines, v1, v2);
	InterpolateScanline(_scanlines, v2, v3);
	InterpolateScanline(_scanlines, v3, v1);

	// Go through each scanline and each pixel in the scanline and 
	// sets its color.
	for (unsigned int y = 0; y < _height; y++)
	{
		// Work out the differences between the start an end colors of the current scanline.
		float redColorDiff = (_scanlines[y].redEnd - _scanlines[y].redStart);
		float greenColorDiff = (_scanlines[y].greenEnd - _scanlines[y].greenStart);
		float blueColorDiff = (_scanlines[y].blueEnd - _scanlines[y].blueStart);
		float diff = (_scanlines[y].xEnd - _scanlines[y].xStart) + 1;

		for (int x = (int)_scanlines[y].xStart; x <= (int)_scanlines[y].xEnd; x++)
		{
			if (x < 0 || x >= (int)_width)
				continue;

			int offset = (int)(x - _scanlines[y].xStart);

			// Use simple interpolation to work out the current color value of this pixel.
			Gdiplus::Color pixelColor = Color
				(
					(BYTE)(_scanlines[y].redStart + ((redColorDiff / diff) * offset)),
					(BYTE)(_scanlines[y].greenStart + ((greenColorDiff / diff) * offset)),
					(BYTE)(_scanlines[y].blueStart + ((blueColorDiff / diff) * offset))	
				);

			// Set the value of the current pixel.
			WritePixel(x, y, pixelColor);
		}
	}
	
	// Dispose of dynamic objects.
	delete[] _scanlines;

	_polygonsRendered++;
}

// Fills a polygon using a texture and gouraud shading given 3 points and a color.
void Rasterizer::FillPolygonTextured(Vertex v1, Vertex v2, Vertex v3, Gdiplus::Color color, Model3D& model)
{
	ScanLine* _scanlines = new ScanLine[_height];

	BYTE* texture;
	Gdiplus::Color* palette;
	int textureWidth;

	// Get the texture properties of the model.
	model.GetTexture(&texture, &palette, &textureWidth); 
	
	// Set the scanlines to very high and very low values so
	// they will be set on the first set of interpolation.
	for (unsigned int i = 0; i < _height; i++)
	{
		_scanlines[i].xStart = 99999;
		_scanlines[i].xEnd = -99999;
	}

	// Interpolates between each of the vertexs of the polygon and sets the start
	// and end values for each of the scanlines it comes in contact with.
	InterpolateScanline(_scanlines, v1, v2);
	InterpolateScanline(_scanlines, v2, v3);
	InterpolateScanline(_scanlines, v3, v1);

	// Go through each scanline and each pixel in the scanline and 
	// sets its color.
	for (unsigned int y = 0; y < _height; y++)
	{
		// Work out the color and UV differences between the start and end of the scanline.
		float redColorDiff = (_scanlines[y].redEnd - _scanlines[y].redStart);
		float greenColorDiff = (_scanlines[y].greenEnd - _scanlines[y].greenStart);
		float blueColorDiff = (_scanlines[y].blueEnd - _scanlines[y].blueStart);
		float uCoordDiff = _scanlines[y].uEnd - _scanlines[y].uStart;
		float vCoordDiff = _scanlines[y].vEnd - _scanlines[y].vStart;
		float zCoordDiff = _scanlines[y].zEnd - _scanlines[y].zStart;
		float diff = (_scanlines[y].xEnd - _scanlines[y].xStart) + 1;

		for (int x = (int)_scanlines[y].xStart; x <= (int)_scanlines[y].xEnd; x++)
		{
			if (x < 0 || x >= (int)_width)
				continue;

			int offset = (int)(x - _scanlines[y].xStart);
			
			// Work out the UV coordinate of the current pixel.
			float uCoord = _scanlines[y].uStart + ((uCoordDiff / diff) * offset);
			float vCoord = _scanlines[y].vStart + ((vCoordDiff / diff) * offset);
			float zCoord = _scanlines[y].zStart + ((zCoordDiff / diff) * offset);

			uCoord /= zCoord;
			vCoord /= zCoord;

			// Work out the lighting colour of the current pixel.
			float lightR = (_scanlines[y].redStart + ((redColorDiff / diff) * offset)) / 180.0f;
			float lightG = (_scanlines[y].greenStart + ((greenColorDiff / diff) * offset)) / 180.0f;
			float lightB = (_scanlines[y].blueStart + ((blueColorDiff / diff) * offset)) / 180.0f;	

			// Using the UV coordinate work out which pixel in the texture to use to draw this pixel.
			int pixelIndex = (int)vCoord * textureWidth + (int)uCoord;
			if (pixelIndex >= textureWidth * textureWidth || pixelIndex < 0)
			{
				pixelIndex = (textureWidth * textureWidth) - 1;
			}

			int paletteOffset = texture[pixelIndex]; 
			if (paletteOffset >= 255)
				paletteOffset = 255;

			Gdiplus::Color textureColor = palette[paletteOffset];

			// Apply the lighting value to the texture colour and use the result to set the colour of the current pixel.
			int finalR = (int)max(0, min(255, textureColor.GetR() * lightR));
			int finalG = (int)max(0, min(255, textureColor.GetG() * lightG));
			int finalB = (int)max(0, min(255, textureColor.GetB() * lightB));

			WritePixel(x, y, Gdiplus::Color(finalR, finalG, finalB));
		}
	}
	
	// Dispose of dynamic objects.
	delete[] _scanlines;

	_polygonsRendered++;
}

// Fills a polygon using a texture, gouraud shading and a normal map given 3 points and a color.
void Rasterizer::FillPolygonTexturedNormalMapped(Vertex v1, Vertex v2, Vertex v3, Gdiplus::Color color, Model3D& model, std::vector<DirectionalLight*> directionalLights, std::vector<AmbientLight*> ambientLights, std::vector<PointLight*> pointLights)
{
	ScanLine* _scanlines = new ScanLine[_height];

	BYTE* texture;
	Gdiplus::Color* palette;
	BYTE* normalTexture;
	Gdiplus::Color* normalPalette;
	int textureWidth;

	// Get the texture properties of the model.
	model.GetTexture(&texture, &palette, &textureWidth); 
	model.GetNormalMapTexture(&normalTexture, &normalPalette, &textureWidth); 
	
	// Set the scanlines to very high and very low values so
	// they will be set on the first set of interpolation.
	for (unsigned int i = 0; i < _height; i++)
	{
		_scanlines[i].xStart = 99999;
		_scanlines[i].xEnd = -99999;
	}

	// Interpolates between each of the vertexs of the polygon and sets the start
	// and end values for each of the scanlines it comes in contact with.
	InterpolateScanline(_scanlines, v1, v2);
	InterpolateScanline(_scanlines, v2, v3);
	InterpolateScanline(_scanlines, v3, v1);

	// Go through each scanline and each pixel in the scanline and 
	// sets its color.
	for (unsigned int y = 0; y < _height; y++)
	{
		// Work out the color and UV differences between the start and end of the scanline.
		float redColorDiff = (_scanlines[y].redEnd - _scanlines[y].redStart);
		float greenColorDiff = (_scanlines[y].greenEnd - _scanlines[y].greenStart);
		float blueColorDiff = (_scanlines[y].blueEnd - _scanlines[y].blueStart);
		float uCoordDiff = _scanlines[y].uEnd - _scanlines[y].uStart;
		float vCoordDiff = _scanlines[y].vEnd - _scanlines[y].vStart;
		float zCoordDiff = _scanlines[y].zEnd - _scanlines[y].zStart;

		float xNormalDiff = (_scanlines[y].xNormalEnd - _scanlines[y].xNormalStart);
		float yNormalDiff = (_scanlines[y].yNormalEnd - _scanlines[y].yNormalStart);
		float zNormalDiff = (_scanlines[y].zNormalEnd - _scanlines[y].zNormalStart);

		float xDiff = (_scanlines[y].pixelXEnd - _scanlines[y].pixelXStart);
		float yDiff = (_scanlines[y].pixelYEnd - _scanlines[y].pixelYStart);
		float zDiff = (_scanlines[y].pixelZEnd - _scanlines[y].pixelZStart);

		float diff = (_scanlines[y].xEnd - _scanlines[y].xStart) + 1;

		for (int x = (int)_scanlines[y].xStart; x <= (int)_scanlines[y].xEnd; x++)
		{
			if (x < 0 || x >= (int)_width)
				continue;

			int offset = (int)(x - _scanlines[y].xStart);
			
			// Work out the UV coordinate of the current pixel.
			float uCoord = _scanlines[y].uStart + ((uCoordDiff / diff) * offset);
			float vCoord = _scanlines[y].vStart + ((vCoordDiff / diff) * offset);
			float zCoord = _scanlines[y].zStart + ((zCoordDiff / diff) * offset);

			uCoord /= zCoord;
			vCoord /= zCoord;

			// Work out the normal of the pixel.
			float xNormal = _scanlines[y].xNormalStart + ((xNormalDiff / diff) * offset);
			float yNormal = _scanlines[y].yNormalStart + ((yNormalDiff / diff) * offset);
			float zNormal = _scanlines[y].zNormalStart + ((zNormalDiff / diff) * offset);

			// Work out the position of the pixel.
			float pixelX = _scanlines[y].pixelXStart + ((xDiff / diff) * offset);
			float pixelY = _scanlines[y].pixelYStart + ((yDiff / diff) * offset);
			float pixelZ = _scanlines[y].pixelZStart + ((zDiff / diff) * offset);

			// Work out the lighting colour of the current pixel.
			//float lightR = (_scanlines[y].redStart + ((redColorDiff / diff) * offset)) / 180.0f;
			//float lightG = (_scanlines[y].greenStart + ((greenColorDiff / diff) * offset)) / 180.0f;
			//float lightB = (_scanlines[y].blueStart + ((blueColorDiff / diff) * offset)) / 180.0f;	

			// Using the UV coordinate work out which pixel in the texture to use to draw this pixel.
			int pixelIndex = (int)vCoord * textureWidth + (int)uCoord;
			if (pixelIndex >= textureWidth * textureWidth || pixelIndex < 0)
			{
				pixelIndex = (textureWidth * textureWidth) - 1;
			}

			int paletteOffset = texture[pixelIndex]; 
			if (paletteOffset >= 255)
				paletteOffset = 255;

			Gdiplus::Color textureColor = palette[paletteOffset];

			// Work out the pixel colour of the normalmap.
			pixelIndex = (int)vCoord * textureWidth + (int)uCoord;
			if (pixelIndex >= textureWidth * textureWidth || pixelIndex < 0)
			{
				pixelIndex = (textureWidth * textureWidth) - 1;
			}

			paletteOffset = normalTexture[pixelIndex]; 
			if (paletteOffset >= 255)
				paletteOffset = 255;

			Gdiplus::Color normalTextureColor = normalPalette[paletteOffset];

			// Calculate normal lighting for the pixel.
			Vector3D heightMapVector = Vector3D(normalTextureColor.GetR() / 180.0f, normalTextureColor.GetG() / 180.0f, normalTextureColor.GetB() / 180.0f); 
			heightMapVector = Vector3D((heightMapVector.GetX() - 0.5f) * 2.0f, (heightMapVector.GetY() - 0.5f) * 2.0f, (heightMapVector.GetZ() - 0.5f) * 2.0f);

			// Work out he pixels normal and position.
			Vector3D pixelNormal = Vector3D(xNormal, yNormal, zNormal);//;Vector3D(heightMapVector.GetX(), heightMapVector.GetY(), heightMapVector.GetZ());
			Vertex pixelPosition = Vertex(pixelX, pixelY, pixelZ, 1, Gdiplus::Color::White, Vector3D(0, 0, 0), 0);

			heightMapVector = Vector3D((pixelNormal.GetX() * heightMapVector.GetX()) , 
										(pixelNormal.GetY() * heightMapVector.GetY()) , 
										(pixelNormal.GetZ() * heightMapVector.GetZ()) );

			// Calculate the sum dot product of all lighting vectors for this pixel and divide by the number
			// of lights.
			float lightDot = 0.0f;
			int count = 0;
			for (unsigned int j = 0; j < pointLights.size(); j++)
			{
				PointLight* light = pointLights[j];
				if (light->GetEnabled() == false)
					continue;
			
				// Work out vector to light source.
				Vector3D lightVector = Vertex::GetVector(pixelPosition, light->GetPosition());
				float distance = lightVector.GetLength();
				lightVector.Normalize();

				// Work out dot product.
				lightDot += Vector3D::DotProduct(heightMapVector, lightVector);
				count++;
			}
			for (unsigned int j = 0; j < directionalLights.size(); j++)
			{
				DirectionalLight* light = directionalLights[j];
				if (light->GetEnabled() == false)
					continue;
			
				// Work out vector to light source.
				Vector3D lightVector = Vertex::GetVector(pixelPosition, light->GetPosition());
				float distance = lightVector.GetLength();
				lightVector.Normalize();

				// Work out dot product.
				lightDot += Vector3D::DotProduct(heightMapVector, lightVector);
				count++;
			}
			lightDot /= count;

			// Adjust texture colour based on the lighting dot product.
			Gdiplus::Color pixelColor = textureColor;
			//pixelColor = model.CalculateLightingAmbientPerPixel(ambientLights, pixelPosition, pixelNormal, pixelColor);
			//pixelColor = model.CalculateLightingDirectionalPerPixel(directionalLights, pixelPosition, pixelNormal, pixelColor);
			//pixelColor = model.CalculateLightingPointPerPixel(pointLights, pixelPosition, pixelNormal, pixelColor);

			float lightR = (_scanlines[y].redStart + ((redColorDiff / diff) * offset)) / 180.0f;
			float lightG = (_scanlines[y].greenStart + ((greenColorDiff / diff) * offset)) / 180.0f;
			float lightB = (_scanlines[y].blueStart + ((blueColorDiff / diff) * offset)) / 180.0f;	

			// Apply the lighting value to the texture colour and use the result to set the colour of the current pixel.
			int finalR = (int)max(0, min(255, (lightR * textureColor.GetR()) - ((lightR * textureColor.GetR()) * lightDot) ));
			int finalG = (int)max(0, min(255, (lightG * textureColor.GetG()) - ((lightG * textureColor.GetG()) * lightDot) ));
			int finalB = (int)max(0, min(255, (lightB * textureColor.GetB()) - ((lightB * textureColor.GetB()) * lightDot) ));

			WritePixel(x, y, Gdiplus::Color(finalR, finalG, finalB));
		}
	}
	
	// Dispose of dynamic objects.
	delete[] _scanlines;

	_polygonsRendered++;
}

// Interpolates between the given vertexs and sets the start and end values of each
// scanline it encounters on the way.
void Rasterizer::InterpolateScanline(ScanLine* scanlines, Vertex v1, Vertex v2)
{
	// Swap the vertexs round if we need to, to make sure
	// the first vertex is the one at the top of the screen.
	if (v2.GetY() < v1.GetY())
	{
		Vertex v3 = v2;
		v2 = v1;
		v1 = v3;
	}
	
	// Work out the difference between each vertex and the number of steps
	// we need to take to interpolate between each of the vertexs.
	float yDiff = (v2.GetY() - v1.GetY());
	float xDiff = (v2.GetX() - v1.GetX());
	float zDiff = (v2.GetPreTransformZ() - v1.GetPreTransformZ());
	float numOfPoints = v2.GetY() - v1.GetY();

	// Work out the difference in colour between each vertex.
	float redColorDiff = (float)(v2.GetColor().GetR() - v1.GetColor().GetR());
	float greenColorDiff = (float)(v2.GetColor().GetG() - v1.GetColor().GetG());
	float blueColorDiff = (float)(v2.GetColor().GetB() - v1.GetColor().GetB());

	// WOrk out the difference between the normals.
	float xNormalDiff = (float)(v2.GetNormal().GetX() - v1.GetNormal().GetX());
	float yNormalDiff = (float)(v2.GetNormal().GetY() - v1.GetNormal().GetY());
	float zNormalDiff = (float)(v2.GetNormal().GetZ() - v1.GetNormal().GetZ());
	
	// Work out the difference in uv coordinate between each vertex.
	UVCoordinate uvCoord1 = v1.GetUVCoordinate();
	UVCoordinate uvCoord2 = v2.GetUVCoordinate();

	uvCoord1.U = uvCoord1.U / v1.GetPreTransformZ();
	uvCoord1.V = uvCoord1.V / v1.GetPreTransformZ();
	uvCoord1.Z = 1.0f / v1.GetPreTransformZ();
	
	uvCoord2.U = uvCoord2.U / v2.GetPreTransformZ();
	uvCoord2.V = uvCoord2.V / v2.GetPreTransformZ();
	uvCoord2.Z = 1.0f / v2.GetPreTransformZ();

	float uCoordDiff = (uvCoord2.U - uvCoord1.U);
	float vCoordDiff = (uvCoord2.V - uvCoord1.V);
	float zCoordDiff = (uvCoord2.Z - uvCoord1.Z);

	// Go through each point in the interpolation and work out the 
	// colour and uv values for pixel along the way.
	for (int i = 0; i < numOfPoints; i++)
	{
		unsigned int scanline = (int)(v1.GetY() + i);
		if (scanline < 0 || scanline >= _height)
			continue;

		// Get the x axis value in our interpolation.
		float x = v1.GetX() + i * (xDiff / yDiff);

		// Work out the colour value of the current pixel.
		float red	= v1.GetColor().GetR() + ((redColorDiff		/ numOfPoints) * i);
		float green = v1.GetColor().GetG() + ((greenColorDiff	/ numOfPoints) * i);
		float blue	= v1.GetColor().GetB() + ((blueColorDiff	/ numOfPoints) * i);
		
		// Work out the normal value of the current pixel.
		float normalX = v1.GetNormal().GetX() + ((xNormalDiff	/ numOfPoints) * i);
		float normalY = v1.GetNormal().GetY() + ((yNormalDiff	/ numOfPoints) * i);
		float normalZ = v1.GetNormal().GetZ() + ((zNormalDiff	/ numOfPoints) * i);

		// Work out the normal value of the current pixel.
		float pixelX = v1.GetX() + ((xDiff	/ numOfPoints) * i);
		float pixelY = v1.GetY() + ((yDiff	/ numOfPoints) * i);
		float pixelZ = v1.GetPreTransformZ() + ((zDiff	/ numOfPoints) * i);

		// Work out the UV value of the current pixel.
		float u	= uvCoord1.U + ((uCoordDiff / numOfPoints) * i);
		float v	= uvCoord1.V + ((vCoordDiff / numOfPoints) * i); 
		float z	= uvCoord1.Z + ((zCoordDiff / numOfPoints) * i); 
		
		// If the x-value is below the scanline's start x-value, then 
		// set the scanlines start value to the current values.
		if (x < scanlines[scanline].xStart)
		{
			scanlines[scanline].xStart = x;
			scanlines[scanline].redStart = red;
			scanlines[scanline].greenStart = green;
			scanlines[scanline].blueStart = blue;
			scanlines[scanline].uStart = u;
			scanlines[scanline].vStart = v;
			scanlines[scanline].zStart = z;
			scanlines[scanline].xNormalStart = normalX;
			scanlines[scanline].yNormalStart = normalY;
			scanlines[scanline].zNormalStart = normalZ;
			scanlines[scanline].pixelXStart = pixelX;
			scanlines[scanline].pixelYStart = pixelY;
			scanlines[scanline].pixelZStart = pixelZ;
		}

		// If the x-value is above the scanline's end x-value, then 
		// set the scanlines end value to the current values.
		if (x > scanlines[scanline].xEnd)
		{
			scanlines[scanline].xEnd = x;
			scanlines[scanline].redEnd = red;
			scanlines[scanline].greenEnd = green;
			scanlines[scanline].blueEnd = blue;
			scanlines[scanline].uEnd = u;
			scanlines[scanline].vEnd = v;
			scanlines[scanline].zEnd = z;
			scanlines[scanline].xNormalEnd = normalX;
			scanlines[scanline].yNormalEnd = normalY;
			scanlines[scanline].zNormalEnd = normalZ;
			scanlines[scanline].pixelXEnd = pixelX;
			scanlines[scanline].pixelYEnd = pixelY;
			scanlines[scanline].pixelZEnd = pixelZ;
		}
	}
}

// Draws the given model in wireframe mode.
void Rasterizer::DrawWireFrame(Model3D& model)
{
	std::vector<Polygon3D> _polygonList = model.GetPolygonList();
	std::vector<Vertex> _vertexList = model.GetTransformedVertexList();
	
	// Iterate over and render each of the polygons in the list.
	for (unsigned int i = 0; i < _polygonList.size(); i++)
	{
		Polygon3D poly = _polygonList[i];
		if (poly.GetBackfacing() == true)
			continue;

		Vertex v1 = _vertexList[poly.GetVertexIndex(0)];
		Vertex v2 = _vertexList[poly.GetVertexIndex(1)];
		Vertex v3 = _vertexList[poly.GetVertexIndex(2)];
	
		// Draw a line between each of the vertexs in the polygon.
		DrawLine(v1.GetX(), v1.GetY(), v2.GetX(), v2.GetY());
		DrawLine(v2.GetX(), v2.GetY(), v3.GetX(), v3.GetY());
		DrawLine(v1.GetX(), v1.GetY(), v3.GetX(), v3.GetY());

		_polygonsRendered++;
	}
}

// Draws the given model with solid shading.
void Rasterizer::DrawSolidFlat(Model3D& model)
{
	std::vector<Polygon3D> _polygonList = model.GetPolygonList();
	std::vector<Vertex> _vertexList = model.GetTransformedVertexList();
	
	// Iterate over and render each of the polygons in the list.
	for (unsigned int i = 0; i < _polygonList.size(); i++)
	{
		Polygon3D poly = _polygonList[i];
		if (poly.GetBackfacing() == true)
			continue;

		Vertex v1 = _vertexList[poly.GetVertexIndex(0)];
		Vertex v2 = _vertexList[poly.GetVertexIndex(1)];
		Vertex v3 = _vertexList[poly.GetVertexIndex(2)];

		// Fill the polygon using the polygons colour.
		FillPolygon(v1, v2, v3, v1.GetColor());
	}
}

// Draws the given model with gouraud shading.
void Rasterizer::DrawSolidShaded(Model3D& model)
{
	std::vector<Polygon3D> _polygonList = model.GetPolygonList();
	std::vector<Vertex> _vertexList = model.GetTransformedVertexList();
	
	// Iterate over and render each of the polygons in the list.
	for (unsigned int i = 0; i < _polygonList.size(); i++)
	{
		Polygon3D poly = _polygonList[i];
		if (poly.GetBackfacing() == true)
			continue;

		Vertex v1 = _vertexList[poly.GetVertexIndex(0)];
		Vertex v2 = _vertexList[poly.GetVertexIndex(1)];
		Vertex v3 = _vertexList[poly.GetVertexIndex(2)];
		
		// Fill the polygon using the polygons colour.
		FillPolygonShaded(v1, v2, v3, poly.GetColor());
	}
}

// Draws the given model with texturing and shading.
void Rasterizer::DrawSolidTextured(Model3D& model)
{
	std::vector<Polygon3D> _polygonList = model.GetPolygonList();
	std::vector<Vertex> _vertexList = model.GetTransformedVertexList();
	std::vector<UVCoordinate> _uvCoordList = model.GetUVCoordinateList();

	for (unsigned int i = 0; i < _polygonList.size(); i++)
	{
		Polygon3D poly = _polygonList[i];
		if (poly.GetBackfacing() == true)
			continue;

		Vertex v1 = _vertexList[poly.GetVertexIndex(0)];
		Vertex v2 = _vertexList[poly.GetVertexIndex(1)];
		Vertex v3 = _vertexList[poly.GetVertexIndex(2)];

		// Set the uv coordinates of each vertex temporarily to the coordinates in the 
		// uv coordinate list.
		v1.SetUVCoordinate(_uvCoordList[poly.GetUVIndex(0)]);
		v2.SetUVCoordinate(_uvCoordList[poly.GetUVIndex(1)]);
		v3.SetUVCoordinate(_uvCoordList[poly.GetUVIndex(2)]);

		// Fill the polygon using the models texture.
		FillPolygonTextured(v1, v2, v3, v1.GetColor(), model);
	}
}

// Draws the given model with texturing, shading and a normal map.
void Rasterizer::DrawSolidTexturedNormalMapped(Model3D& model, std::vector<DirectionalLight*> directionalLights, std::vector<AmbientLight*> ambientLights, std::vector<PointLight*> pointLights)
{
	std::vector<Polygon3D> _polygonList = model.GetPolygonList();
	std::vector<Vertex> _vertexList = model.GetTransformedVertexList();
	std::vector<UVCoordinate> _uvCoordList = model.GetUVCoordinateList();

	for (unsigned int i = 0; i < _polygonList.size(); i++)
	{
		Polygon3D poly = _polygonList[i];
		if (poly.GetBackfacing() == true)
			continue;

		Vertex v1 = _vertexList[poly.GetVertexIndex(0)];
		Vertex v2 = _vertexList[poly.GetVertexIndex(1)];
		Vertex v3 = _vertexList[poly.GetVertexIndex(2)];

		// Set the uv coordinates of each vertex temporarily to the coordinates in the 
		// uv coordinate list.
		v1.SetUVCoordinate(_uvCoordList[poly.GetUVIndex(0)]);
		v2.SetUVCoordinate(_uvCoordList[poly.GetUVIndex(1)]);
		v3.SetUVCoordinate(_uvCoordList[poly.GetUVIndex(2)]);

		// Fill the polygon using the models texture.
		if (model.GetNormalMapOn() == true)
			FillPolygonTexturedNormalMapped(v1, v2, v3, v1.GetColor(), model, directionalLights, ambientLights, pointLights);
		else
			FillPolygonTextured(v1, v2, v3, v1.GetColor(), model);
	}
}

// Renders the given text on screen at the given position.
void Rasterizer::DrawText(float x, float y, const WCHAR* string)
{
	// Create all the objects we need to render the string.
	Gdiplus::FontFamily* fontFamily = new FontFamily(L"Courier New");
	Gdiplus::Font* font = new Font(fontFamily, 13, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	Gdiplus::PointF origin = PointF(x, y);
	Gdiplus::PointF shadowOrigin = PointF(x + 1, y + 1);
	Gdiplus::Brush* brush = new SolidBrush(Gdiplus::Color::White);
	Gdiplus::Brush* shadowBrush = new SolidBrush(Gdiplus::Color::Black);

	// Draw the text to string.
	FinishLockBits();
	_graphics->DrawString(string, -1, font, shadowOrigin, shadowBrush); 
	_graphics->DrawString(string, -1, font, origin, brush); 	
	BeginLockBits();

	// Dispose of all objects we created.
	delete fontFamily;
	delete font;
	delete brush;
	delete shadowBrush;
}
