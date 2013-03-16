// =========================================================================================
//	MD2Loader.cpp
// =========================================================================================
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#include "stdafx.h"
#include "MD2Loader.h"

// File reading
#include <iostream>
#include <fstream>

using namespace std;


MD2Loader::MD2Loader(void)
{
}

MD2Loader::~MD2Loader(void)
{
}

// ----------------------------------------------
// LoadPCX() - load pcx texture from file.
//
// assume the caller has allocated enough memory for texture - md2Header->skinWidth * md2Header->skinHeight
// ----------------------------------------------
bool MD2Loader::LoadPCX(const char* filename, BYTE* texture, Gdiplus::Color* palette, const Md2Header* md2Header)
{
	std::ifstream   file;   // file stream

	// try to open filename
    file.open( filename, std::ios::in | std::ios::binary );

    if( file.fail() )
        return false;

    // read md2 header file
    PcxHeader header;         
	file.read( reinterpret_cast<char*>(&header), sizeof( PcxHeader ) );

    /////////////////////////////////////////////
    //      verify that this is a valid PCX file

	// only handle those with 256 colour palette
	if( (header.Version != 5) || (header.BitsPerPixel != 8) || 
		(header.Encoding != 1) || (header.NumPlanes != 1) ||
		(md2Header && (header.BytesPerLine != md2Header->skinWidth) ) )
    {
        // this is not valid supported PCX
        file.close();
        return false;
    }

    /////////////////////////////////////////////
	//		check dimensions

	int XSize = header.XMax - header.XMin + 1;
	int YSize = header.YMax - header.YMin + 1;
	int Size = XSize * YSize;

	// check matches our MD2 expected texture
	// note. valid size is <= because uses RLE (so potentially smaller)
	if ( md2Header && ( Size > (md2Header->skinHeight * md2Header->skinWidth) ) )
	{
		// doesn't match expected MD2 skin size
		file.close();
		return false;
	}
	
	/////////////////////////////////////////////
    //          reading file data

	BYTE processByte, colourByte;
	int count = 0;
	while(count < Size)
	{
		file.read( reinterpret_cast<char*>(&processByte), 1 );

		// Run length encoding - test if byte is an RLE byte
		if ((processByte & 192) == 192)
		{
			// extract number of times repeated byte
			processByte &= 63;
			file.read( reinterpret_cast<char*>(&colourByte), 1 );
			for (int index = 0; index < processByte; ++index)
			{
				// repeatedly write colour 
				texture[count] = colourByte;
				++count;
			}
		}
		else
		{
			// process byte is the colour
			texture[count] = processByte;
			++count;
		}
	}

	bool returnValue = false;

	// read palette data...
	file.seekg( -769, std::ios::end);	// from this offset from end of file
	file.read( reinterpret_cast<char*>(&processByte), 1 );
	if (processByte == 12)
	{
		BYTE rawPalette[768];
		file.read( reinterpret_cast<char*>(&rawPalette), 768 );

		// build palette
		for (int palIndex = 0; palIndex < 256; ++palIndex)
		{			
			palette[palIndex].SetFromCOLORREF( RGB(rawPalette[ palIndex * 3],
												   rawPalette[(palIndex * 3) + 1],
												   rawPalette[(palIndex * 3) + 2] ));
		}

		returnValue = true;
	}

	// all done
	file.close();
	return returnValue;
}

// ----------------------------------------------
// LoadModel() - load model from file.
// ----------------------------------------------

bool MD2Loader::LoadModel(const char* filename, Model3D& model, const char* textureFilename, const char* normalMapTextureFilename)
{
	ifstream   file;           

	// Try to open MD2 file
	file.open( filename, ios::in | ios::binary );
	if(file.fail())
	{
		return false;
	}
	// Read file header
	Md2Header header;         
	file.read( reinterpret_cast<char*>(&header), sizeof( Md2Header ) );
		
	// Verify that this is a MD2 file (check for the magic number and version number)
	if( (header.indent != MD2_IDENT) && (header.version != MD2_VERSION) )
	{
		// This is not a MD2 model
		file.close();
		return false;
	}

	// Allocate the memory we need
	Md2Triangle* triangles = new Md2Triangle[header.numTriangles];
	// We are only interested in the first frame 
	BYTE* frameBuffer = new BYTE[ header.frameSize ];
	Md2Frame* frame = reinterpret_cast<Md2Frame*>(frameBuffer);
	Md2TexCoord* texCoords = new Md2TexCoord[header.numTexCoords];

	// Read polygon data...
	file.seekg( header.offsetTriangles, ios::beg );
	file.read( reinterpret_cast<char*>(triangles), sizeof(Md2Triangle) * header.numTriangles );	
		
	// Read frame data...
	file.seekg( header.offsetFrames, ios::beg );
	file.read( reinterpret_cast<char*>(frame), header.frameSize );	

	// Read texture coordinate data
	file.seekg( header.offsetTexCoords, std::ios::beg );
	file.read( reinterpret_cast<char*>(texCoords), sizeof(Md2TexCoord) * header.numTexCoords );

	// Close the file 2.0517745e-038
	file.close();

	//----------------------------------------------------------------------------------------------

	// Initialize model textures.
	bool bHasTexture = false;

	// Attempt to load texture
	if ( textureFilename != 0 )
	{
		BYTE* pTexture = new BYTE[header.skinWidth * header.skinHeight];
		Gdiplus::Color* pPalette = new Gdiplus::Color[256];
		
		bHasTexture = LoadPCX(textureFilename, pTexture, pPalette, &header);
		if ( !bHasTexture )
		{
			delete(pTexture);
			delete(pPalette);
		}
		else
		{
			model.SetTexture(pTexture, pPalette, header.skinWidth);
		}
	}

	// Attempt to load normal map texture
	if ( normalMapTextureFilename != 0 )
	{
		BYTE* pTexture = new BYTE[header.skinWidth * header.skinHeight];
		Gdiplus::Color* pPalette = new Gdiplus::Color[256];
		
		bool valid = LoadPCX(normalMapTextureFilename, pTexture, pPalette, &header);
		if (!valid)
		{
			delete(pTexture);
			delete(pPalette);
		}
		else
		{
			model.SetNormalMapTexture(pTexture, pPalette, header.skinWidth);
		}
	}

	// Polygon array initialization
	for ( int i = 0; i < header.numTriangles; ++i )
	{
		// TODO - Put your own code here to create a new Polygon and store it in your list
		//
		// The following are the expressions you need to access each of the indexes into the list of vertices:
		//
		// Index 0:  triangles[i].vertexIndex[0]
		// Index 1:  triangles[i].vertexIndex[1]
		// Index 2:  triangles[i].vertexIndex[2]

		Polygon3D newPoly = Polygon3D(triangles[i].vertexIndex[0],
									  triangles[i].vertexIndex[1],
									  triangles[i].vertexIndex[2]);
		newPoly.SetUVIndex(0, triangles[i].uvIndex[0]);
		newPoly.SetUVIndex(1, triangles[i].uvIndex[1]);
		newPoly.SetUVIndex(2, triangles[i].uvIndex[2]);

		model.GetPolygonList().push_back(newPoly);
	}

	// Vertex array initialization
	for( int i = 0; i < header.numVertices; ++i )
	{
		// TODO - Put your own code here to create a new Vertex and store it in your list
		//
		// The following are the expressions you need to access each of the co-ordinates.
		// 
		// X co-ordinate:   frame->verts[i].v[0] * frame->scale[0]) + frame->translate[0]
		// Y co-ordinate:   frame->verts[i].v[2] * frame->scale[2]) + frame->translate[2]
		// Z co-ordinate:   frame->verts[i].v[1] * frame->scale[1]) + frame->translate[1]
		//
		// NOTE: We have to swap Y and Z over because Z is up in MD2 and we have Y as up-axis
		
		float x = (frame->verts[i].v[0] * frame->scale[0]) + frame->translate[0];
		float y = (frame->verts[i].v[2] * frame->scale[2]) + frame->translate[2];
		float z = (frame->verts[i].v[1] * frame->scale[1]) + frame->translate[1];

		Vertex vert = Vertex(x, y, z, 1.0f, Gdiplus::Color::Black, Vector3D(0,0,0), 0);
		model.GetVertexList().push_back(vert);
	}

	// Load UV coordinates
	if (bHasTexture)
	{
		for (int i = 0; i < header.numTexCoords; i++)
		{
			short u = texCoords[i].texCoord[0];			
			short v = texCoords[i].texCoord[1];

			UVCoordinate uvCoord;
			uvCoord.U = u;
			uvCoord.V = v;
			model.GetUVCoordinateList().push_back(uvCoord);
		}
	}

	// Rebuild model lists.
	model.RebuildTransformedVerticesList();

	// Free dynamically allocated memory
	delete [] triangles; // NOTE: this is 'array' delete. Must be sure to use this
	triangles = 0;

	delete [] frameBuffer;
	frameBuffer = 0;
	frame = 0;

	delete [] texCoords;
	texCoords = 0;

	return true;
}
