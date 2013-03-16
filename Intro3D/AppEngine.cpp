// =========================================================================================
//	AppEngine.cpp
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#include "StdAfx.h"
#include "AppEngine.h"
#include <cmath>
#include <windows.h>

// Initialize display mode array here as it causes multiple definition linker errors in header file.
const WCHAR* DisplayModeNames[] =
{
	L"Wireframe",
	L"Flat Shaded - Unlit",
	L"Flat Shaded - Ambient Lighting",
	L"Flat Shaded - Ambient + Directional Lighting",
	L"Flat Shaded - Ambient + Directional + Point Lighting",
	L"Gouraud Shaded - Unlit",
	L"Gouraud Shaded - Ambient Lighting",
	L"Gouraud Shaded - Ambient + Directional Lighting",
	L"Gouraud Shaded - Ambient + Directional + Point Lighting",
	L"Textured (Perspective Adjusted) - Unlit",
	L"Textured (Perspective Adjusted) - Ambient Lighting",
	L"Textured (Perspective Adjusted) - Ambient + Directional Lighting",
	L"Textured (Perspective Adjusted, Normal Mapped) - Ambient + Directional Lighting",
	L"Textured (Perspective Adjusted) - Ambient + Directional + Point Lighting",
	L"Textured (Perspective Adjusted, Normal Mapped) - Ambient + Directional + Point Lighting"
};

// Initializer.
AppEngine::AppEngine(void)
{
	_rasterizer = NULL;
	_model1 = NULL;
	_model2 = NULL;
	_camera = NULL;
}

// Destructor.
AppEngine::~AppEngine(void)
{
	// Make sure everything is cleaned up (in case Shutdown() was now called directly
	Shutdown();
}

// This method initializes the main application, including the rasterizer, loading models, etc.
//  hWnd : Handle of window we will be rendering to.
void AppEngine::Initialise(HWND hWnd)
{
	_hWnd = hWnd;

	// Get the width and height of the window so that we can tell the rasterizer to create a bitmap
	// of the same size.  Note that this only works for the initial size of the window. If you want 
	// your code to handle resizing of the window, you will need to add code for this yourself. 
	LONG width;
	LONG height;
	RECT clientWindowRect;
	if (GetClientRect(_hWnd, &clientWindowRect))
	{
		width = clientWindowRect.right;
		height = clientWindowRect.bottom;
	}
	else
	{
		width = 640;
		height = 480;
	}
	_rasterizer = new Rasterizer((unsigned int)width, (unsigned int)height);

	// Load a model 1 (our character).
	_model1 = new Model3D();
	bool result = MD2Loader::LoadModel("baron.md2", *_model1, "baron.pcx", "baron_nm.pcx");
	//bool result = MD2Loader::LoadModel("cube.md2", *_model1, "cube.pcx", "cube_nm.pcx");
	//bool result = MD2Loader::LoadModel("loadtruckb.md2", *_model1, "loadtruckb.pcx", "loadtruckb_nm.pcx");
//	cube.md2

	// Load a model 2 (our grass floor).
	_model2 = new Model3D();
	result = MD2Loader::LoadModel("grass.md2", *_model2, "grass.pcx");

	// Make a new camera.
	_camera = new Camera(0, 0, 0, Vertex(0, 50, -100, 1, Gdiplus::Color::Black, Vector3D(0,0,0), 0), 640, 480);

	// Create all of our lights that we will
	// be using in our demo.
	_directionalLight = new DirectionalLight();
	_directionalLight->SetPosition(Vertex(500, 150, 50, 1, Gdiplus::Color::Red, Vector3D(0,0,0), 0));
	_directionalLight->SetIntensity(Color(255, 0, 0));
	_directionalLightList.push_back(_directionalLight);

	_ambientLight = new AmbientLight();
	_ambientLight->SetPosition(Vertex(0, 0, 0, 1, Gdiplus::Color::Red, Vector3D(0,0,0), 0));
	_ambientLight->SetIntensity(Color(100, 100, 100));
	_ambientLightList.push_back(_ambientLight);

	_unlitAmbientLight = new AmbientLight();
	_unlitAmbientLight->SetPosition(Vertex(0, 0, 0, 1, Gdiplus::Color::Red, Vector3D(0,0,0), 0));
	_unlitAmbientLight->SetIntensity(Color::White);
	_ambientLightList.push_back(_unlitAmbientLight);

	_pointLight = new PointLight(Color(0, 0, 255), 0, 0.1f, 0);
	_pointLight->SetPosition(Vertex(-500, 150, 10, 1, Gdiplus::Color::Black, Vector3D(0,0,0), 0));
	_pointLightList.push_back(_pointLight);

//	_spotLight = new PointLight(Color(0, 0, 255), 0, 0.1f, 0);
//	_spotLight->SetPosition(Vertex(-500, 150, 10, 1, Gdiplus::Color::Black, Vector3D(0,0,0), 0));
//	_spotLightList.push_back(_spotLight);

	// Sets up the scene for a given display mode.
	_displayMode = 0;
	_displayModeTimer = GetTickCount() + DISPLAY_MODE_DURATION;
	SetDisplayMode(WireFrame);
	
	// Setup our scaling and rotation values.
	_scale = 1.0f;
	_scaleDir = false;
	_angle = 0.0f;
}

// This method does and updates and processing for the current frame.
void AppEngine::Process(void)
{
	// At the moment, all we do is render a new image. In the future, we might do more here
	Render();
}

// This method renders the current frame to the window.
void AppEngine::Render(void)
{
	// Cycle through the display modes after every DISPLAY_MODE_DURATION ticks.
	if (GetTickCount() >= _displayModeTimer)
	{
		_displayModeTimer = GetTickCount() + DISPLAY_MODE_DURATION;

		// Cycle the display mode.
		if (++_displayMode >= DISPLAY_MODE_COUNT)
			_displayMode = 0;
		
		// Sets up the scene for the current display mode.
		SetDisplayMode((DisplayMode)_displayMode);
	}

	// Update the angle and scale values used to "animate" the 
	// models being rendered in the scene.
	_angle += 0.05f;
	if (_scaleDir == true)
	{
		_scale += 0.02f;
		if (_scale > 1.5f)
			_scaleDir = !_scaleDir;
	}
	else
	{
		_scale -= 0.02f;
		if (_scale < 0.75f)
			_scaleDir = !_scaleDir;
	}
	
	// Begin rendering frame.
	_rasterizer->BeginLockBits();
	_rasterizer->ResetPolygonsRendered();

	// Clear the window.
	_rasterizer->Clear(Color::SteelBlue);
	
	// Render each of the models using a translation, scale and rotation 
	// matrix (depending on how we are animating them).
	RenderModel(_model2, Matrix3D::ScaleMatrix(_scale, _scale, _scale)	* Matrix3D::TranslateMatrix(0, -50, 150));
	RenderModel(_model1, Matrix3D::RotateMatrix(0, _angle, 0)			* Matrix3D::TranslateMatrix(0, 0, 30));
	
	// Convert the fps/polygons value to a renderable string.
	WCHAR convertArray[256];
	WSTRING fpsString = L"FPS: ";
	wsprintf(convertArray, L"%i", _fps);
	fpsString += WSTRING(convertArray);

	WSTRING polysString = L"Polygons: ";
	wsprintf(convertArray, L"%i", _rasterizer->GetPolygonsRendered());
	polysString += WSTRING(convertArray);

	// Draw the description of the current mode.
	_rasterizer->DrawText(10, 10, L"Software Rasterizer");
	_rasterizer->DrawText(10, 30, L"Timothy Leonard (100119086)");
	_rasterizer->DrawText(10, (float)(_rasterizer->GetHeight() - 67), fpsString.c_str());
	_rasterizer->DrawText(10, (float)(_rasterizer->GetHeight() - 47), polysString.c_str());
	_rasterizer->DrawText(10, (float)(_rasterizer->GetHeight() - 27), DisplayModeNames[_displayMode]);
	
	// Finish rendering frame.
	_rasterizer->FinishLockBits();
	
	// Ensure that Windows is told that the contents of the window now needs painting
	InvalidateRect(_hWnd, NULL, FALSE);

	// Track number of frames per second.
	TrackFPS();
}

// This method will render the model with a given transformation matrix.
void AppEngine::RenderModel(Model3D* model, Matrix3D transformMatrix)
{
	model->ApplyTransformToLocalVertices(transformMatrix);
	model->CalculateBackfaces(_camera);
	model->CalculateVertexNormals();
	model->ResetLighting();

	// Calculate lighting.
	model->CalculateLightingAmbient(_ambientLightList);
	model->CalculateLightingDirectional(_directionalLightList);
	model->CalculateLightingPoint(_pointLightList);
	model->CalculateLightingSpot(_spotLightList);

	// Apply the viewport matrix.
	model->ApplyTransformToTransformedVertices(_camera->GetViewportMatrix());
	model->DepthSort();

	// Apply the perspective matrix.
	model->ApplyTransformToTransformedVertices(_camera->GetPerspectiveMatrix());
	model->DehomogenizeTransformedVertices();
	
	// Apply the screen matrix, converting the vertexs to screen space.
	model->ApplyTransformToTransformedVertices(_camera->GetScreenMatrix());

	// Render differently depending on the current display mode.
	switch ((DisplayMode)_displayMode)
	{
	case WireFrame:
		_rasterizer->DrawWireFrame(*model);
		break;

	case FlatShadedUnlit:
	case FlatShadedAmbient:
	case FlatShadedDirectionalAmbient:
	case FlatShadedDirectionalPointAmient:
		_rasterizer->DrawSolidFlat(*model);
		break;

	case GouraudShadedUnlit:
	case GouraudShadedAmbient:
	case GouraudShadedDirectionalAmbient:
	case GouraudShadedDirectionalPointAmient:
		_rasterizer->DrawSolidShaded(*model);
		break;

	case TexturedUnlit:
	case TexturedAmbient:
	case TexturedDirectionalAmbient:
	case TexturedNormalMappedDirectionalAmbient:
	case TexturedDirectionalPointAmbient:
	case TexturedNormalMappedDirectionalPointAmbient:
		//_rasterizer->DrawSolidTextured(*model);
		_rasterizer->DrawSolidTexturedNormalMapped(*model, _directionalLightList, _ambientLightList, _pointLightList);
		break;
	}
}

// This method paints the rasterizers bitmap to the windows device context.
void AppEngine::Paint(HDC hdc)
{
	// Copy the contents of the rasterizer's bitmap to our window
	if (_rasterizer)
	{
		Graphics graphics(hdc);
		graphics.DrawImage(_rasterizer->GetBitmap(), 0, 0, _rasterizer->GetWidth(), _rasterizer->GetHeight());
	}
}

// This method shuts down the application and disposes of all allocated resources.
void AppEngine::Shutdown(void)
{
	// Clean up lights.
	//for (int i = 0; i < _directionalLightList.size(); i++)
	//	delete _directionalLightList[i];
	//for (int i = 0; i < _ambientLightList.size(); i++)
	//	delete _ambientLightList[i];
	//for (int i = 0; i < _pointLightList.size(); i++)
	//	delete _pointLightList[i];

	// Clean up models.
	if (_model1)
	{
		delete _model1;
		_model1 = NULL;
	}
	if (_model2)
	{
		delete _model2;
		_model2 = NULL;
	}

	// Clean up camera.
	if (_camera)
	{
		delete _camera;
		_camera = NULL;
	}

	// Clean up all memory that has been dynamically allocated
	if (_rasterizer)
	{
		delete _rasterizer;
		_rasterizer = NULL;
	}
}

// This method is used to enable & disable lights depending on the current display mode of the demo.
void AppEngine::SetDisplayMode(DisplayMode mode)
{
	switch (mode)
	{
	case WireFrame:
		_directionalLight->SetEnabled(false);
		_ambientLight->SetEnabled(false);
		_pointLight->SetEnabled(false);
		_unlitAmbientLight->SetEnabled(true);
		_model1->SetNormalMapOn(false);
		break;

	case FlatShadedUnlit:
		_directionalLight->SetEnabled(false);
		_ambientLight->SetEnabled(false);
		_pointLight->SetEnabled(false);
		_unlitAmbientLight->SetEnabled(true);
		_model1->SetNormalMapOn(false);
		break;

	case FlatShadedAmbient:
		_directionalLight->SetEnabled(false);
		_ambientLight->SetEnabled(true);
		_pointLight->SetEnabled(false);
		_unlitAmbientLight->SetEnabled(false);
		_model1->SetNormalMapOn(false);
		break;

	case FlatShadedDirectionalAmbient:
		_directionalLight->SetEnabled(true);
		_ambientLight->SetEnabled(true);
		_pointLight->SetEnabled(false);
		_unlitAmbientLight->SetEnabled(false);
		_model1->SetNormalMapOn(false);
		break;

	case FlatShadedDirectionalPointAmient:
		_directionalLight->SetEnabled(true);
		_ambientLight->SetEnabled(true);
		_pointLight->SetEnabled(true);
		_unlitAmbientLight->SetEnabled(false);
		_model1->SetNormalMapOn(false);
		break;

	case GouraudShadedUnlit:
		_directionalLight->SetEnabled(false);
		_ambientLight->SetEnabled(false);
		_pointLight->SetEnabled(false);
		_unlitAmbientLight->SetEnabled(true);
		_model1->SetNormalMapOn(false);
		break;

	case GouraudShadedAmbient:
		_directionalLight->SetEnabled(false);
		_ambientLight->SetEnabled(true);
		_pointLight->SetEnabled(false);
		_unlitAmbientLight->SetEnabled(false);
		_model1->SetNormalMapOn(false);
		break;

	case GouraudShadedDirectionalAmbient:
		_directionalLight->SetEnabled(true);
		_ambientLight->SetEnabled(true);
		_pointLight->SetEnabled(false);
		_unlitAmbientLight->SetEnabled(false);
		_model1->SetNormalMapOn(false);
		break;

	case GouraudShadedDirectionalPointAmient:
		_directionalLight->SetEnabled(true);
		_ambientLight->SetEnabled(true);
		_pointLight->SetEnabled(true);
		_unlitAmbientLight->SetEnabled(false);
		_model1->SetNormalMapOn(false);
		break;

	case TexturedUnlit:
		_directionalLight->SetEnabled(false);
		_ambientLight->SetEnabled(false);
		_pointLight->SetEnabled(false);
		_unlitAmbientLight->SetEnabled(true);
		_model1->SetNormalMapOn(false);
		break;

	case TexturedAmbient:
		_directionalLight->SetEnabled(false);
		_ambientLight->SetEnabled(true);
		_pointLight->SetEnabled(false);
		_unlitAmbientLight->SetEnabled(false);
		_model1->SetNormalMapOn(false);
		break;

	case TexturedDirectionalAmbient:
		_directionalLight->SetEnabled(true);
		_ambientLight->SetEnabled(true);
		_pointLight->SetEnabled(false);
		_unlitAmbientLight->SetEnabled(false);
		_model1->SetNormalMapOn(false);
		break;

	case TexturedNormalMappedDirectionalAmbient:
		_directionalLight->SetEnabled(true);
		_ambientLight->SetEnabled(true);
		_pointLight->SetEnabled(false);
		_unlitAmbientLight->SetEnabled(false);
		_model1->SetNormalMapOn(true);
		break;

	case TexturedDirectionalPointAmbient:
		_directionalLight->SetEnabled(true);
		_ambientLight->SetEnabled(true);
		_pointLight->SetEnabled(true);
		_unlitAmbientLight->SetEnabled(false);
		_model1->SetNormalMapOn(false);
		break;

	case TexturedNormalMappedDirectionalPointAmbient:
		_directionalLight->SetEnabled(true);
		_ambientLight->SetEnabled(true);
		_pointLight->SetEnabled(true);
		_unlitAmbientLight->SetEnabled(false);
		_model1->SetNormalMapOn(true);
		break;
	}
}

// This is more or less just a helper method, it's called each frame and tracks the number
// of frames rendered per second.
void AppEngine::TrackFPS()
{
	if (GetTickCount() >= _fpsTimer)
	{
		_fps = _fpsTicks;
		_fpsTicks = 0;
		_fpsTimer = GetTickCount() + 1000;
	}
	else
		_fpsTicks++;
}