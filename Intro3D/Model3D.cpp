// =========================================================================================
//	Model3D.cpp
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#include "StdAfx.h"
#include "Model3D.h"
#include <algorithm>

// This struct is used to sort polygon lists by depth.
struct SortPolygonsByDepth 
{
    bool operator() (const Polygon3D& lhs, const Polygon3D& rhs) 
	{
        float ld = lhs.GetAvgDepth();
		float rd = rhs.GetAvgDepth();
		return ld > rd;
    }
};

// Constructor.
Model3D::Model3D(void)
{
	_kd_red = 1.0f;
	_kd_green = 1.0f;
	_kd_blue = 1.0f;

	_texture = NULL;
	_palette = NULL;
	_textureWidth = 0;
}

// Destructor.
Model3D::~Model3D()
{
	if (_texture != NULL)
		delete [] _texture;
	if (_palette != NULL)
		delete [] _palette;
}

// Accessor methods. Simple get/set code.
void Model3D::SetTexture(BYTE* texture, Gdiplus::Color* palette, int textureWidth)
{
	_texture = texture;
	_palette = palette;
	_textureWidth = textureWidth;
}
void Model3D::GetTexture(BYTE** texture, Gdiplus::Color** palette, int* textureWidth)
{
	*texture = _texture;
	*palette = _palette;
	*textureWidth = _textureWidth;
}
void Model3D::SetNormalMapTexture(BYTE* texture, Gdiplus::Color* palette, int textureWidth)
{
	_normalMapTexture = texture;
	_normalMapPalette = palette;
	_normalMapTextureWidth = textureWidth;
}
void Model3D::GetNormalMapTexture(BYTE** texture, Gdiplus::Color** palette, int* textureWidth)
{
	*texture = _normalMapTexture;
	*palette = _normalMapPalette;
	*textureWidth = _normalMapTextureWidth;
}
void Model3D::SetNormalMapOn(bool val)
{
	_normalMapOn = val;
}
bool Model3D::GetNormalMapOn()
{
	return _normalMapOn;
}
std::vector<Polygon3D>& Model3D::GetPolygonList()
{
	return _polygons;
}
std::vector<UVCoordinate>& Model3D::GetUVCoordinateList()
{
	return _uvCoordinates;
}
std::vector<Vertex>& Model3D::GetVertexList()
{
	return _vertices;
}
std::vector<Vertex>& Model3D::GetTransformedVertexList()
{
	return _transformedVertices;
}
void Model3D::SetReflectionCoefficients(float ka, float kd, float ks)
{
	_kd_red = ka;
	_kd_green = kd;
	_kd_blue = ks;
}
void Model3D::GetReflectionCoefficients(float& r, float& g, float& b)
{
	r = _kd_red;
	g = _kd_green;
	b = _kd_blue;
}

// Applys the given matrix transformation to the un-transformated vertices
// and pushes them into the transformed vertices list.
void Model3D::ApplyTransformToLocalVertices(const Matrix3D& transform)
{
	_transformedVertices.clear();
	for (unsigned int i = 0; i < _vertices.size(); i++)
		_transformedVertices.push_back(transform * _vertices[i]);
}

// Applys the given matrix transformation to the transformated vertices.
void Model3D::ApplyTransformToTransformedVertices(const Matrix3D& transform)
{
	for (unsigned int i = 0; i < _transformedVertices.size(); i++)
	{
		Vertex retVal = transform * _transformedVertices[i];
		_transformedVertices[i] = retVal;
	}
}

// Resets the transformed vertices list to their non-transformed positions.
void Model3D::RebuildTransformedVerticesList()
{
	for (unsigned int i = 0; i < _vertices.size(); i++)
		_transformedVertices.push_back(_vertices[i]);
}

// Dehomohenizes each vertice in the transformed vertices list.
void Model3D::DehomogenizeTransformedVertices()
{
	for (unsigned int i = 0; i < _transformedVertices.size(); i++)
		_transformedVertices[i].Dehomogenize();
}

// Goes through each polygon and flags those that are backfacing from given camera position.
void Model3D::CalculateBackfaces(Camera* camera)
{
	for (unsigned int i = 0; i < _polygons.size(); i++)
	{
		Polygon3D polygon = _polygons[i];
		Vertex v1 = _transformedVertices[polygon.GetVertexIndex(0)];
		Vertex v2 = _transformedVertices[polygon.GetVertexIndex(1)];
		Vertex v3 = _transformedVertices[polygon.GetVertexIndex(2)];
	
		// Get the polygons normal by working out the cross product of
		// the vectors from the first vertex to each of the other vertexs.
		Vector3D a = Vertex::GetVector(v1, v2);
		Vector3D b = Vertex::GetVector(v1, v3);
		Vector3D normal = Vector3D::CrossProduct(a, b);

		// Work out the vector between the camera and the first vertex. (should be changed to middle of polygon not first vertex).
		Vector3D eyeVector = Vertex::GetVector(v1, camera->GetPosition());

		// Work out the dot product of the eye vector and the polygons normal.
		float dotProduct = Vector3D::DotProduct(eyeVector, normal);

		// If the dot product is less than 0, then polygon is backfacing, otherwise not.
		if (dotProduct < 0)
			_polygons[i].SetBackfacing(true);
		else
			_polygons[i].SetBackfacing(false);
	}
}

// Sorts the polygon list by depth.
void Model3D::DepthSort()
{
	// Calculate average Z depths.
	for (unsigned int i = 0; i < _polygons.size(); i++)
	{
		Polygon3D polygon = _polygons[i];
		
		// Calculate the sum of all vertex Z coordinates.
		float depthSum = 0;
		for (int j = 0; j < 3; j++)
			depthSum += _transformedVertices[polygon.GetVertexIndex(j)].GetZ();
		
		// Work out and set the average (sum/3).
		depthSum /= 3.0f;
		_polygons[i].SetAvgDepth(depthSum);
	}

	// Sort the collection using the standard sort function.
	std::sort(_polygons.begin(), _polygons.end(), SortPolygonsByDepth());
}

// Resets the lighting of the polygon back to black, ready for lighting calculations.
void Model3D::ResetLighting()
{
	for (unsigned int i = 0; i < _polygons.size(); i++)
	{
		_polygons[i].SetColor(Gdiplus::Color::Black);
	}
}

// Calculates the directional lighting of a given pixel.
Gdiplus::Color Model3D::CalculateLightingDirectionalPerPixel(std::vector<DirectionalLight*> lights, Vertex pixelPosition, Vector3D pixelNormal, Gdiplus::Color startColor)
{
	float totalR, totalG, totalB;
	float tempR, tempG, tempB;
	
	// Reset total light.
	totalR = startColor.GetR();
	totalG = startColor.GetG();
	totalB = startColor.GetB();

	// Calculate lighting.
	for (unsigned int j = 0; j < lights.size(); j++)
	{
		DirectionalLight* light = lights[j];
		Gdiplus::Color c = light->GetIntensity();

		if (light->GetEnabled() == false)
			continue;

		tempR = c.GetR();
		tempG = c.GetG();
		tempB = c.GetB();
		
		// Apply the lighting coefficients.
		tempR *= _kd_red;
		tempG *= _kd_green;
		tempB *= _kd_blue;
			
		// Work out vector to light source.
		Vector3D lightVector = Vertex::GetVector(pixelPosition, light->GetPosition());
		lightVector.Normalize();

		// Work out dot product.
		float dotProduct = Vector3D::DotProduct(lightVector, pixelNormal);
		if (dotProduct >= 0)
			continue;

		// Multiply the colour by the dot product.
		tempR *= dotProduct;
		tempG *= dotProduct;
		tempB *= dotProduct;
			
		// Add the light color to the total polygon color.
		totalR += abs(tempR);
		totalG += abs(tempG);
		totalB += abs(tempB);
	}

	// Clamp lighting.
	totalR = max(min(totalR, 255.0f), 0.0f);
	totalG = max(min(totalG, 255.0f), 0.0f);
	totalB = max(min(totalB, 255.0f), 0.0f);

	return Gdiplus::Color((BYTE)totalR, (BYTE)totalG, (BYTE)totalB);
}

// Calculates the ambient lighting of a given pixel.
Gdiplus::Color Model3D::CalculateLightingAmbientPerPixel(std::vector<AmbientLight*> lights, Vertex pixelPosition, Vector3D pixelNormal, Gdiplus::Color startColor)
{
	float totalR, totalG, totalB;
	float tempR, tempG, tempB;

	// Reset total light.
	totalR = startColor.GetR();
	totalG = startColor.GetG();
	totalB = startColor.GetB();

	// Calculate lighting.
	for (unsigned int j = 0; j < lights.size(); j++)
	{
		Light* light = lights[j];
		Gdiplus::Color c = light->GetIntensity();

		if (light->GetEnabled() == false)
			continue;

		// Set the temporary light color to the lights color.
		tempR = c.GetR();
		tempG = c.GetG();
		tempB = c.GetB();
		
		// Apply the lighting coefficients.
		tempR *= _kd_red;
		tempG *= _kd_green;
		tempB *= _kd_blue;
			
		// Add the light color to the total polygon color.
		totalR += abs(tempR);
		totalG += abs(tempG);
		totalB += abs(tempB);
	}

	// Clamp lighting.
	totalR = max(min(totalR, 255.0f), 0.0f);
	totalG = max(min(totalG, 255.0f), 0.0f);
	totalB = max(min(totalB, 255.0f), 0.0f);

	return Gdiplus::Color((BYTE)totalR, (BYTE)totalG, (BYTE)totalB);
}

// Calculates the point lighting of a given pixel.
Gdiplus::Color Model3D::CalculateLightingPointPerPixel(std::vector<PointLight*> lights, Vertex pixelPosition, Vector3D pixelNormal, Gdiplus::Color startColor)
{
	float totalR, totalG, totalB;
	float tempR, tempG, tempB;

	// Reset total light.
	totalR = startColor.GetR();
	totalG = startColor.GetG();
	totalB = startColor.GetB();

	// Calculate lighting.
	for (unsigned int j = 0; j < lights.size(); j++)
	{
		PointLight* light = lights[j];
		Gdiplus::Color c = light->GetIntensity();

		if (light->GetEnabled() == false)
			continue;
			
		// Set the temporary light color to the lights color.
		tempR = c.GetR();
		tempG = c.GetG();
		tempB = c.GetB();
		
		// Apply the lighting coefficients.
		tempR *= _kd_red;
		tempG *= _kd_green;
		tempB *= _kd_blue;
			
		// Work out vector to light source.
		Vector3D lightVector = Vertex::GetVector(pixelPosition, light->GetPosition());
		float distance = lightVector.GetLength();
		lightVector.Normalize();

		// Work out attenuation.
		float attnA, attnB, attnC;
		light->GetAttenuation(attnA, attnB, attnC);

		float attenuation = 1 / (attnA + attnB * distance + attnC * (distance * distance));
		attenuation *= 100;

		// Work out dot product.
		float dotProduct = Vector3D::DotProduct(lightVector, pixelNormal);
		if (dotProduct >= 0)
			continue;

		// Apply the dot product to the temporary light color.
		tempR *= dotProduct;
		tempG *= dotProduct;
		tempB *= dotProduct;
			
		// Apply the attenuation to the temporary light color.
		tempR *= attenuation;
		tempG *= attenuation;
		tempB *= attenuation;
			
		// Add the light color to the total polygon color.
		totalR += abs(tempR);
		totalG += abs(tempG);
		totalB += abs(tempB);
	}

	// Clamp lighting.
	totalR = max(min(totalR, 255.0f), 0.0f);
	totalG = max(min(totalG, 255.0f), 0.0f);
	totalB = max(min(totalB, 255.0f), 0.0f);

	return Gdiplus::Color((BYTE)totalR, (BYTE)totalG, (BYTE)totalB);
}

// Calculates the compound lighting of all polygons on the model from
// the list of directional lights.
void Model3D::CalculateLightingDirectional(std::vector<DirectionalLight*> lights)
{
	float totalR, totalG, totalB;
	float tempR, tempG, tempB;

	for (unsigned int i = 0; i < _transformedVertices.size(); i++)
	{
		Vertex vert = _transformedVertices[i];

		// Reset total light.
		totalR = vert.GetColor().GetR();
		totalG = vert.GetColor().GetG();
		totalB = vert.GetColor().GetB();

		// Calculate lighting.
		for (unsigned int j = 0; j < lights.size(); j++)
		{
			DirectionalLight* light = lights[j];
			Gdiplus::Color c = light->GetIntensity();

			if (light->GetEnabled() == false)
				continue;

			tempR = c.GetR();
			tempG = c.GetG();
			tempB = c.GetB();
		
			// Apply the lighting coefficients.
			tempR *= _kd_red;
			tempG *= _kd_green;
			tempB *= _kd_blue;
			
			// Work out vector to light source.
			Vector3D lightVector = Vertex::GetVector(vert, light->GetPosition());
			lightVector.Normalize();

			// Work out dot product.
			float dotProduct = Vector3D::DotProduct(lightVector, vert.GetNormal());
			if (dotProduct >= 0)
				continue;

			// Multiply the colour by the dot product.
			tempR *= dotProduct;
			tempG *= dotProduct;
			tempB *= dotProduct;
			
			// Add the light color to the total polygon color.
			totalR += abs(tempR);
			totalG += abs(tempG);
			totalB += abs(tempB);
		}

		// Clamp lighting.
		totalR = max(min(totalR, 255.0f), 0.0f);
		totalG = max(min(totalG, 255.0f), 0.0f);
		totalB = max(min(totalB, 255.0f), 0.0f);

		// Store color.
		_transformedVertices[i].SetColor(Gdiplus::Color((int)totalR, (int)totalG, (int)totalB));
	}
}

// Calculates the compound lighting of all polygons on the model from
// the list of ambient lights.
void Model3D::CalculateLightingAmbient(std::vector<AmbientLight*> lights)
{
	float totalR, totalG, totalB;
	float tempR, tempG, tempB;

	for (unsigned int i = 0; i < _transformedVertices.size(); i++)
	{
		Vertex vert = _transformedVertices[i];

		// Reset total light.
		totalR = vert.GetColor().GetR();
		totalG = vert.GetColor().GetG();
		totalB = vert.GetColor().GetB();

		// Calculate lighting.
		for (unsigned int j = 0; j < lights.size(); j++)
		{
			Light* light = lights[j];
			Gdiplus::Color c = light->GetIntensity();

			if (light->GetEnabled() == false)
				continue;

			// Set the temporary light color to the lights color.
			tempR = c.GetR();
			tempG = c.GetG();
			tempB = c.GetB();
		
			// Apply the lighting coefficients.
			tempR *= _kd_red;
			tempG *= _kd_green;
			tempB *= _kd_blue;
			
			// Add the light color to the total polygon color.
			totalR += abs(tempR);
			totalG += abs(tempG);
			totalB += abs(tempB);
		}

		// Clamp lighting.
		totalR = max(min(totalR, 255.0f), 0.0f);
		totalG = max(min(totalG, 255.0f), 0.0f);
		totalB = max(min(totalB, 255.0f), 0.0f);

		// Store color.
		_transformedVertices[i].SetColor(Gdiplus::Color((int)totalR, (int)totalG, (int)totalB));
	}
}

// Calculates the compound lighting of all polygons on the model from
// the list of point lights.
void Model3D::CalculateLightingPoint(std::vector<PointLight*> lights)
{
	float totalR, totalG, totalB;
	float tempR, tempG, tempB;

	for (unsigned int i = 0; i < _transformedVertices.size(); i++)
	{
		Vertex vert = _transformedVertices[i];

		// Reset total light.
		totalR = vert.GetColor().GetR();
		totalG = vert.GetColor().GetG();
		totalB = vert.GetColor().GetB();

		// Calculate lighting.
		for (unsigned int j = 0; j < lights.size(); j++)
		{
			PointLight* light = lights[j];
			Gdiplus::Color c = light->GetIntensity();

			if (light->GetEnabled() == false)
				continue;
			
			// Set the temporary light color to the lights color.
			tempR = c.GetR();
			tempG = c.GetG();
			tempB = c.GetB();
		
			// Apply the lighting coefficients.
			tempR *= _kd_red;
			tempG *= _kd_green;
			tempB *= _kd_blue;
			
			// Work out vector to light source.
			Vector3D lightVector = Vertex::GetVector(vert, light->GetPosition());
			float distance = lightVector.GetLength();
			lightVector.Normalize();

			// Work out attenuation.
			float attnA, attnB, attnC;
			light->GetAttenuation(attnA, attnB, attnC);

			float attenuation = 1 / (attnA + attnB * distance + attnC * (distance * distance));
			attenuation *= 100;

			// Work out dot product.
			float dotProduct = Vector3D::DotProduct(lightVector, vert.GetNormal());
			if (dotProduct >= 0)
				continue;

			// Apply the dot product to the temporary light color.
			tempR *= dotProduct;
			tempG *= dotProduct;
			tempB *= dotProduct;
			
			// Apply the attenuation to the temporary light color.
			tempR *= attenuation;
			tempG *= attenuation;
			tempB *= attenuation;
			
			// Add the light color to the total polygon color.
			totalR += abs(tempR);
			totalG += abs(tempG);
			totalB += abs(tempB);
		}

		// Clamp lighting.
		totalR = max(min(totalR, 255.0f), 0.0f);
		totalG = max(min(totalG, 255.0f), 0.0f);
		totalB = max(min(totalB, 255.0f), 0.0f);

		// Store color.
		_transformedVertices[i].SetColor(Gdiplus::Color((int)totalR, (int)totalG, (int)totalB));
	}
}

// Calculates the compound lighting of all polygons on the model from
// the list of spot lights.
void Model3D::CalculateLightingSpot(std::vector<SpotLight*> lights)
{
	float totalR, totalG, totalB;
	float tempR, tempG, tempB;

	for (unsigned int i = 0; i < _transformedVertices.size(); i++)
	{
		Vertex vert = _transformedVertices[i];

		// Reset total light.
		totalR = vert.GetColor().GetR();
		totalG = vert.GetColor().GetG();
		totalB = vert.GetColor().GetB();

		// Calculate lighting.
		for (unsigned int j = 0; j < lights.size(); j++)
		{
			SpotLight* light = lights[j];
			Gdiplus::Color c = light->GetIntensity();

			if (light->GetEnabled() == false)
				continue;
			
			// Set the temporary light color to the lights color.
			tempR = c.GetR();
			tempG = c.GetG();
			tempB = c.GetB();
		
			// Apply the lighting coefficients.
			tempR *= _kd_red;
			tempG *= _kd_green;
			tempB *= _kd_blue;
			
			// Work out vector to light source.
			Vector3D lightVector = Vertex::GetVector(vert, light->GetPosition());
			float distance = lightVector.GetLength();
			lightVector.Normalize();

			// Work out attenuation.
			float attnA, attnB, attnC;
			light->GetAttenuation(attnA, attnB, attnC);

			float attenuation = 1 / (attnA + attnB * distance + attnC * (distance * distance));
			attenuation *= 100;

			// Work out dot product.
			float dotProduct = Vector3D::DotProduct(lightVector, vert.GetNormal());
			if (dotProduct >= 0)
				continue;

			// Work out spotlight value.
			float spotValue = 1.0f;//light->SmoothStep(cos(light->GetAngle()), cos());

			// Apply the dot product to the temporary light color.
			tempR *= dotProduct;
			tempG *= dotProduct;
			tempB *= dotProduct;
			
			// Apply the attenuation to the temporary light color.
			tempR *= attenuation;
			tempG *= attenuation;
			tempB *= attenuation;

			// Apply spotlight value.
			tempR *= spotValue;
			tempG *= spotValue;
			tempB *= spotValue;
			
			// Add the light color to the total polygon color.
			totalR += abs(tempR);
			totalG += abs(tempG);
			totalB += abs(tempB);
		}

		// Clamp lighting.
		totalR = max(min(totalR, 255.0f), 0.0f);
		totalG = max(min(totalG, 255.0f), 0.0f);
		totalB = max(min(totalB, 255.0f), 0.0f);

		// Store color.
		_transformedVertices[i].SetColor(Gdiplus::Color((int)totalR, (int)totalG, (int)totalB));
	}
}

// Calculates the normals of all transformed vertexs in the model. 
void Model3D::CalculateVertexNormals()
{
	// Reset all the normals and normal counts of all 
	// transformed vertices to default.
	for (unsigned int i = 0; i < _transformedVertices.size(); i++)
	{
		_transformedVertices[i].SetNormal(Vector3D(0, 0, 0));
		_transformedVertices[i].SetNormalCount(0);
	}

	// Work out the normal sum for each vertex in the polygon.
	for (unsigned int i = 0; i < _polygons.size(); i++)
	{
		Polygon3D polygon = _polygons[i];

		// Work out normal.
		Vertex v1 = _transformedVertices[polygon.GetVertexIndex(0)];
		Vertex v2 = _transformedVertices[polygon.GetVertexIndex(1)];
		Vertex v3 = _transformedVertices[polygon.GetVertexIndex(2)];
		Vector3D a = Vertex::GetVector(v1, v2);
		Vector3D b = Vertex::GetVector(v1, v3);
		Vector3D polyNormal = Vector3D::CrossProduct(a, b);

		// Update normals.
		for (int j = 0; j < 3; j++)
		{
			int index = polygon.GetVertexIndex(j);
			Vertex v = _transformedVertices[index]; 
			Vector3D vNormal = v1.GetNormal();

			// Adds the vertex normal to the polygons current normal.
			// Increments the normal count.
			_transformedVertices[index].SetNormal(Vector3D(vNormal.GetX() + polyNormal.GetX(),
												  vNormal.GetY() + polyNormal.GetY(),
												  vNormal.GetZ() + polyNormal.GetZ()));
			_transformedVertices[index].SetNormalCount(v.GetNormalCount() + 1);
		}
	}

	// Work out the normal for each of the transformed vertices.
	for (unsigned int i = 0; i < _transformedVertices.size(); i++)
	{
		Vertex v = _transformedVertices[i]; 

		// Work out the normal for the vertex's by dividing the normal sum
		// by the normal count.
		_transformedVertices[i].SetNormal(Vector3D(v.GetX() / v.GetNormalCount(),
										v.GetY() / v.GetNormalCount(),
										v.GetZ() / v.GetNormalCount()));

		// Normalize the vector and apply it to the transformed vertices.
		Vector3D n = _transformedVertices[i].GetNormal();
		n.Normalize();
		_transformedVertices[i].SetNormal(Vector3D(-n.GetX(), -n.GetY(), -n.GetZ()));
	}
}

