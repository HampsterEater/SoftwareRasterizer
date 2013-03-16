// =========================================================================================
//	stdafx.h
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#pragma once

#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <GdiPlus.h>
#include <stdio.h>
#include <string>

#include "AppEngine.h"
#include "Rasterizer.h"
#include "Vertex.h"
#include "Matrix3D.h"
#include "Polygon3D.h"
#include "Model3D.h"
#include "MD2Loader.h"
#include "Camera.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "AmbientLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "UVCoordinate.h"