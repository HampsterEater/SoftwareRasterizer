// =========================================================================================
//	MD2Loader.h
// =========================================================================================
//	Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#pragma once
#include "Model3D.h"

// Forward reference
class Model3D;

// BYTE added in case Windows.h is not included.
typedef unsigned char BYTE; 

// Magic number for MD2 files  "IDP2" or 844121161
const int MD2_IDENT = (('2'<<24) + ('P'<<16) + ('D'<<8) + 'I');

// MS2 version
const int MD2_VERSION = 8;

struct Md2Header
{
		int indent;               // The magic number used to identify the file.
		int version;              // The file version number (must be 8).
		int skinWidth;            // The width in pixels of our image.
		int skinHeight;           // The height in pixels of our image.
		int frameSize;            // The size in bytes the frames are.
		int numSkins;             // The number of skins associated with the model.
		int numVertices;		  // The number of vertices.
		int numTexCoords;		  // The number of texture coordinates.
		int numTriangles;		  // The number of faces (polygons).
		int numGlCommands;        // The number of gl commands.
		int numFrames;			  // The number of animated frames.
		int offsetSkins;		  // The offset in the file for the skin data.
		int offsetTexCoords;	  // The offset in the file for the texture data.
		int offsetTriangles;	  // The offset in the file for the face data.
		int offsetFrames;		  // The offset in the file for the frames data.
		int offsetGlCommands;	  // The offset in the file for the gl commands data.
		int offsetEnd;            // The end of the file offset.
};

struct Md2Triangle
{
	short vertexIndex[3];   // Vertex indices of the triangle
	short uvIndex[3];       // Texture coordinate indices 
};

struct Md2TexCoord
{
	short texCoord[2];
};

struct Md2Vertex
{
	BYTE v[3];                // Compressed vertex (x, y, z) coordinates
	BYTE lightNormalIndex;    // Index to a normal vector for the lighting
};

struct Md2Frame
{
	float       scale[3];       // Scale values
	float       translate[3];   // Translation vector
	char        name[16];       // Frame name
	Md2Vertex   verts[1];       // First vertex of this frame
};

class MD2Loader
{
	public:
		MD2Loader(void);
		~MD2Loader(void);
		static bool LoadPCX(const char* filename, BYTE* texture, Gdiplus::Color* palette, const Md2Header* md2Header);
		static bool LoadModel(const char* md2Filename, Model3D& model, const char* textureFilename = 0, const char* normalMapTextureFilename = 0);
};
