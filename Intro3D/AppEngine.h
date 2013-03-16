// =========================================================================================
//	AppEngine.h
// =========================================================================================
//	Written by Timothy Leonard
//	For Introduction to 3D Graphics Programming (5CC068)
// =========================================================================================

#pragma once
#include "Rasterizer.h"
#include "Model3D.h"
#include "Camera.h"
#include "Light.h"
#include <vector>
#include <string>

// Enumeration of every display mode show in the demonstration.
enum DisplayMode
{
	WireFrame,
	FlatShadedUnlit,
	FlatShadedAmbient,
	FlatShadedDirectionalAmbient,
	FlatShadedDirectionalPointAmient,
	GouraudShadedUnlit,
	GouraudShadedAmbient,
	GouraudShadedDirectionalAmbient,
	GouraudShadedDirectionalPointAmient,
	TexturedUnlit,
	TexturedAmbient,
	TexturedDirectionalAmbient,
	TexturedNormalMappedDirectionalAmbient,
	TexturedDirectionalPointAmbient,
	TexturedNormalMappedDirectionalPointAmbient
};

// Constants the define how fast and what display modes the demo should play.
#define DISPLAY_MODE_COUNT		15
#define DISPLAY_MODE_DURATION	3000

// Custom data type used when converting integers to wide strings.
typedef std::basic_string<WCHAR> WSTRING;

// This is the AppEngine class it's responsible for updating
// and rendering the the main demonstration mode.
class AppEngine
{
	public:
		AppEngine(void);
		~AppEngine(void);

		void Initialise(HWND hWnd);
		void Process(void);
		void Paint(HDC hdc);
		void Shutdown(void);

	private:
		HWND _hWnd;
		Rasterizer * _rasterizer;

		// Lighting lists.
		std::vector<DirectionalLight*> _directionalLightList;		
		std::vector<AmbientLight*> _ambientLightList;
		std::vector<PointLight*> _pointLightList;
		std::vector<SpotLight*> _spotLightList;

		// Display mode variables.
		unsigned int _displayModeTimer;
		int _displayMode;

		unsigned int _fpsTimer;
		int _fpsTicks;
		int _fps;
	
		// Objects in the scene.
		Model3D* _model1;
		Model3D* _model2;
		Camera* _camera;
		DirectionalLight* _directionalLight;
		AmbientLight* _ambientLight;
		AmbientLight* _unlitAmbientLight;
		PointLight* _pointLight;
		SpotLight* _spotLight;

		// Scale and angle variables.
		float _scale;
		bool _scaleDir;
		float _angle;

		// Private methods.
		void Render(void);
		void RenderModel(Model3D* model, Matrix3D transformMatrix);
		void SetDisplayMode(DisplayMode mode);
		void TrackFPS();
};

