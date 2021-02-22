// Link to libraries
#pragma comment(lib, "user32.lib")	 // Visual Studio Only
#pragma comment(lib, "gdi32.lib")	 // For other Windows Compilers please add
#pragma comment(lib, "opengl32.lib") // these libs to your linker input
#pragma comment(lib, "gdiplus.lib")

// Include WinAPI
#define NOMINMAX
#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a > b) ? a : b)
#include <windows.h>
#include <gdiplus.h>

// OpenGL Extension
#include <GL/gl.h>
typedef BOOL(WINAPI wglSwapInterval_t)(int interval);
wglSwapInterval_t *wglSwapInterval;

// Standard includes
#include <cmath>
#include <cstdint>
#include <string>
#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <fstream>
#include <map>
#include <codecvt>

namespace olc // All OneLoneCoder stuff will now exist in the "olc" namespace
{
	struct Pixel
	{
		union
		{
			uint32_t n = 0xFF000000;
			struct
			{
				uint8_t r;
				uint8_t g;
				uint8_t b;
				uint8_t a;
			};
		};

		Pixel();
		Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
		enum Mode
		{
			NORMAL,
			MASK,
			ALPHA
		};
	};

	// Some constants for symbolic naming of Pixels
	static const Pixel
		WHITE(255, 255, 255),
		GREY(192, 192, 192), DARK_GREY(128, 128, 128), VERY_DARK_GREY(64, 64, 64),
		RED(255, 0, 0), DARK_RED(128, 0, 0), VERY_DARK_RED(64, 0, 0),
		YELLOW(255, 255, 0), DARK_YELLOW(128, 128, 0), VERY_DARK_YELLOW(64, 64, 0),
		GREEN(0, 255, 0), DARK_GREEN(0, 128, 0), VERY_DARK_GREEN(0, 64, 0),
		CYAN(0, 255, 255), DARK_CYAN(0, 128, 128), VERY_DARK_CYAN(0, 64, 64),
		BLUE(0, 0, 255), DARK_BLUE(0, 0, 128), VERY_DARK_BLUE(0, 0, 64),
		MAGENTA(255, 0, 255), DARK_MAGENTA(128, 0, 128), VERY_DARK_MAGENTA(64, 0, 64),
		BLACK(0, 0, 0);

	enum rcode
	{
		FAIL = 0,
		OK = 1,
		NO_FILE = -1,
	};

	//=============================================================

	struct HWButton
	{
		bool bPressed = false;	// Set once during the frame the event occurs
		bool bReleased = false; // Set once during the frame the event occurs
		bool bHeld = false;		// Set tru for all frames between pressed and released events
	};

	//=============================================================

	// A bitmap-like structure that stores a 2D array of Pixels
	class Sprite
	{
	public:
		Sprite();
		Sprite(std::string sImageFile);
		Sprite(int32_t w, int32_t h);
		~Sprite();

	public:
		olc::rcode LoadFromFile(std::string sImageFile);

	public:
		int32_t width = 0;
		int32_t height = 0;

	public:
		Pixel GetPixel(int32_t x, int32_t y);
		void SetPixel(int32_t x, int32_t y, Pixel p);
		Pixel Sample(float x, float y);
		Pixel *GetData();

	private:
		Pixel *pColData = nullptr;
	};

	//=============================================================

	enum Key
	{
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		K0,
		K1,
		K2,
		K3,
		K4,
		K5,
		K6,
		K7,
		K8,
		K9,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		UP,
		DOWN,
		LEFT,
		RIGHT,
		SPACE,
		TAB,
		LSHIFT,
		RSHIFT,
		LCTRL,
		RCTRL,
		LALT,
		RALT,
		INS,
		DEL,
		HOME,
		END,
		PGUP,
		PGDN,
		BACK,
		ESCAPE,
		ENTER,
		PAUSE,
		SCROLL,
	};

	//=============================================================

	class PixelGameEngine
	{
	public:
		PixelGameEngine();

	public:
		olc::rcode Construct(uint32_t screen_w, uint32_t screen_h, uint32_t pixel_w, uint32_t pixel_h);
		olc::rcode Start();

	public: // Override Interfaces
		// Called once on application startup, use to load your resources
		virtual bool OnUserCreate();
		// Called every frame, and provides you with a time per frame value
		virtual bool OnUserUpdate(float fElapsedTime);
		// Called once on application termination, so you can be a clean coder
		virtual bool OnUserDestroy();

	public: // Hardware Interfaces
		// Returns true if window is currently in focus
		bool IsFocused();
		// Get the state of a specific keyboard button
		HWButton GetKey(Key k);
		// Get the state of a specific mouse button
		HWButton GetMouse(char b);
		// Get Mouse X coordinate in "pixel" space
		int32_t GetMouseX();
		// Get Mouse Y coordinate in "pixel" space
		int32_t GetMouseY();

	public: // Utility
		// Returns the width of the screen in "pixels"
		int32_t ScreenWidth();
		// Returns the height of the screen in "pixels"
		int32_t ScreenHeight();
		// Returns the width of the currently selected drawing target in "pixels"
		int32_t GetDrawTargetWidth();
		// Returns the height of the currently selected drawing target in "pixels"
		int32_t GetDrawTargetHeight();
		// Returns the currently active draw target
		Sprite *GetDrawTarget();

	public: // Draw Routines
		// Specify which Sprite should be the target of drawing functions, use nullptr
		// to specify the primary screen
		void SetDrawTarget(Sprite *target);
		// Change the pixel mode for different optimisations
		// olc::Pixel::NORMAL = No transparency
		// olc::Pixel::MASK   = Transparent if alpha is < 255
		// olc::Pixel::ALPHA  = Full transparency
		void SetPixelMode(Pixel::Mode m);

		// Draws a single Pixel
		virtual void Draw(int32_t x, int32_t y, Pixel p);
		// Draws a line from (x1,y1) to (x2,y2)
		void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Pixel p);
		// Draws a circle located at (x,y) with radius
		void DrawCircle(int32_t x, int32_t y, int32_t radius, Pixel p);
		// Fills a circle located at (x,y) with radius
		void FillCircle(int32_t x, int32_t y, int32_t radius, Pixel p);
		// Draws a rectangle at (x,y) to (x+w,y+h)
		void DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p);
		// Fills a rectangle at (x,y) to (x+w,y+h)
		void FillRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p);
		// Draws a triangle between points (x1,y1), (x2,y2) and (x3,y3)
		void DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p);
		// Flat fills a triangle between points (x1,y1), (x2,y2) and (x3,y3)
		void FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p);
		// Draws an entire sprite at location (x,y)
		void DrawSprite(int32_t x, int32_t y, Sprite *sprite);
		// Draws an area of a sprite at location (x,y), where the
		// selected area is (ox,oy) to (ox+w,oy+h)
		void DrawPartialSprite(int32_t x, int32_t y, Sprite *sprite, int32_t ox, int32_t oy, int32_t w, int32_t h);
		// Draws a single line of text
		void DrawString(int32_t x, int32_t y, std::string sText);

	public: // Branding
		std::string sAppName;
		Sprite *fontSprite = nullptr;

	private: // Inner mysterious workings
		Sprite *pDefaultDrawTarget = nullptr;
		Sprite *pDrawTarget = nullptr;
		Pixel::Mode nPixelMode = Pixel::NORMAL;
		uint32_t nScreenWidth = 256;
		uint32_t nScreenHeight = 240;
		uint32_t nPixelWidth = 4;
		uint32_t nPixelHeight = 4;
		uint32_t nMousePosX = 0;
		uint32_t nMousePosY = 0;
		bool bHasInputFocus = false;
		float fFrameTimer = 1.0f;
		int nFrameCount = 0;
		// Sprite *fontSprite = nullptr;

		static std::map<uint16_t, uint8_t> mapKeys;
		bool pKeyNewState[256]{0};
		bool pKeyOldState[256]{0};
		HWButton pKeyboardState[256];

		bool pMouseNewState[5]{0};
		bool pMouseOldState[5]{0};
		HWButton pMouseState[5];

		HDC glDeviceContext = nullptr;
		HGLRC glRenderContext = nullptr;

		GLuint glBuffer;

		void EngineThread();

		// If anything sets this flag to false, the engine
		// "should" shut down gracefully
		static std::atomic<bool> bAtomActive;

		// Common initialisation functions
		void olc_UpdateMouse(uint32_t x, uint32_t y);
		bool olc_OpenGLCreate();
		void olc_ConstructFontSheet();

		// Windows specific window handling
		HWND olc_hWnd = nullptr;
		HWND olc_WindowCreate();
		std::wstring wsAppName;
		static LRESULT CALLBACK olc_WindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};

	//=============================================================
} // namespace olc
