#include "olcPixelGameEngineOOP.h"

namespace olc
{
	//Pixel
	//==========================================================

	Pixel::Pixel()
	{
		r = 0;
		g = 0;
		b = 0;
		a = 255;
	}

	Pixel::Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
	{
		r = red;
		g = green;
		b = blue;
		a = alpha;
	}

	//End pixel
	//==========================================================

	//Sprite
	//==========================================================

	Sprite::Sprite()
	{
		pColData = nullptr;
		width = 0;
		height = 0;
	}

	Sprite::Sprite(std::string sImageFile)
	{
		LoadFromFile(sImageFile);
	}

	Sprite::Sprite(int32_t w, int32_t h)
	{
		if (pColData)
			delete[] pColData;
		width = w;
		height = h;
		pColData = new Pixel[width * height];
		for (int32_t i = 0; i < width * height; i++)
			pColData[i] = Pixel();
	}

	Sprite::~Sprite()
	{
		if (pColData)
			delete[] pColData;
	}

	olc::rcode Sprite::LoadFromFile(std::string sImageFile)
	{
		// Use GDI+
		std::wstring wsImageFile;
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		wsImageFile = converter.from_bytes(sImageFile);
		Gdiplus::Bitmap *bmp = Gdiplus::Bitmap::FromFile(wsImageFile.c_str());
		if (bmp == nullptr)
			return olc::NO_FILE;

		width = bmp->GetWidth();
		height = bmp->GetHeight();
		pColData = new Pixel[width * height];
		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++)
			{
				Gdiplus::Color c;
				bmp->GetPixel(x, y, &c);
				SetPixel(x, y, Pixel(c.GetRed(), c.GetGreen(), c.GetBlue(), c.GetAlpha()));
			}
		delete bmp;
		return olc::OK;
	}

	Pixel Sprite::GetPixel(int32_t x, int32_t y)
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
			return pColData[y * width + x];
		else
			return Pixel();
	}

	void Sprite::SetPixel(int32_t x, int32_t y, Pixel p)
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
			pColData[y * width + x] = p;
	}

	Pixel Sprite::Sample(float x, float y)
	{
		int32_t sx = (int32_t)(x * (float)width);
		int32_t sy = (int32_t)(y * (float)height);
		return GetPixel(sx, sy);
	}

	Pixel *Sprite::GetData() { return pColData; }

	//End sprite
	//==========================================================

	//End PixelGameEngine
	//==========================================================

	PixelGameEngine::PixelGameEngine()
	{
		sAppName = "Undefined";
	}

	olc::rcode PixelGameEngine::Construct(uint32_t screen_w, uint32_t screen_h, uint32_t pixel_w, uint32_t pixel_h)
	{
		nScreenWidth = screen_w;
		nScreenHeight = screen_h;
		nPixelWidth = pixel_w;
		nPixelHeight = pixel_h;

#ifdef UNICODE
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		wsAppName = converter.from_bytes(sAppName);
#endif
		// Create a sprite that represents the primary drawing target
		pDefaultDrawTarget = new Sprite(nScreenWidth, nScreenHeight);
		SetDrawTarget(nullptr);
		return olc::OK;
	}

	olc::rcode PixelGameEngine::Start()
	{
		// Construct the window
		if (!olc_WindowCreate())
			return olc::FAIL;

		// Load libraries required for PNG file interaction
		// Windows use GDI+
		Gdiplus::GdiplusStartupInput startupInput;
		ULONG_PTR token;
		Gdiplus::GdiplusStartup(&token, &startupInput, NULL);
		// Start the thread
		bAtomActive = true;
		std::thread t = std::thread(&PixelGameEngine::EngineThread, this);

		// Handle Windows Message Loop
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Wait for thread to be exited
		t.join();
		return olc::OK;
	}

	void PixelGameEngine::SetDrawTarget(Sprite *target)
	{
		if (target)
			pDrawTarget = target;
		else
			pDrawTarget = pDefaultDrawTarget;
	}

	Sprite *PixelGameEngine::GetDrawTarget()
	{
		return pDrawTarget;
	}

	int32_t PixelGameEngine::GetDrawTargetWidth()
	{
		if (pDrawTarget)
			return pDrawTarget->width;
		else
			return 0;
	}

	int32_t PixelGameEngine::GetDrawTargetHeight()
	{
		if (pDrawTarget)
			return pDrawTarget->height;
		else
			return 0;
	}

	bool PixelGameEngine::IsFocused()
	{
		return bHasInputFocus;
	}

	HWButton PixelGameEngine::GetKey(Key k)
	{
		return pKeyboardState[k];
	}

	HWButton PixelGameEngine::GetMouse(char b)
	{
		return pMouseState[b];
	}

	int32_t PixelGameEngine::GetMouseX()
	{
		return nMousePosX;
	}

	int32_t PixelGameEngine::GetMouseY()
	{
		return nMousePosY;
	}

	int32_t PixelGameEngine::ScreenWidth()
	{
		return nScreenWidth;
	}

	int32_t PixelGameEngine::ScreenHeight()
	{
		return nScreenHeight;
	}

	void PixelGameEngine::Draw(int32_t x, int32_t y, Pixel p)
	{
		if (!pDrawTarget)
			return;

		if (nPixelMode == Pixel::NORMAL)
		{
			pDrawTarget->SetPixel(x, y, p);
			return;
		}

		if (nPixelMode == Pixel::MASK)
		{
			if (p.a != 255)
				pDrawTarget->SetPixel(x, y, p);
			return;
		}

		if (nPixelMode == Pixel::ALPHA)
		{
			Pixel d = pDrawTarget->GetPixel(x, y);
			float a = (float)p.a / 255.0f;
			float c = 1.0f - a;
			float r = a * (float)p.r + c * (float)d.r;
			float g = a * (float)p.g + c * (float)d.g;
			float b = a * (float)p.b + c * (float)d.b;
			pDrawTarget->SetPixel(x, y, Pixel((uint8_t)r, (uint8_t)g, (uint8_t)b));
			return;
		}
	}

	void PixelGameEngine::DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Pixel p)
	{
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1;
		dy = y2 - y1;
		dx1 = abs(dx);
		dy1 = abs(dy);
		px = 2 * dy1 - dx1;
		py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1;
				y = y1;
				xe = x2;
			}
			else
			{
				x = x2;
				y = y2;
				xe = x1;
			}

			Draw(x, y, p);

			for (i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0)
					px = px + 2 * dy1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						y = y + 1;
					else
						y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				Draw(x, y, p);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1;
				y = y1;
				ye = y2;
			}
			else
			{
				x = x2;
				y = y2;
				ye = y1;
			}

			Draw(x, y, p);

			for (i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						x = x + 1;
					else
						x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				Draw(x, y, p);
			}
		}
	}

	void PixelGameEngine::DrawCircle(int32_t x, int32_t y, int32_t radius, Pixel p)
	{
		int x0 = 0;
		int y0 = radius;
		int d = 3 - 2 * radius;
		if (!radius)
			return;

		while (y0 >= x0) // only formulate 1/8 of circle
		{
			Draw(x - x0, y - y0, p); //upper left left
			Draw(x - y0, y - x0, p); //upper upper left
			Draw(x + y0, y - x0, p); //upper upper right
			Draw(x + x0, y - y0, p); //upper right right
			Draw(x - x0, y + y0, p); //lower left left
			Draw(x - y0, y + x0, p); //lower lower left
			Draw(x + y0, y + x0, p); //lower lower right
			Draw(x + x0, y + y0, p); //lower right right
			if (d < 0)
				d += 4 * x0++ + 6;
			else
				d += 4 * (x0++ - y0--) + 10;
		}
	}

	void PixelGameEngine::FillCircle(int32_t x, int32_t y, int32_t radius, Pixel p)
	{
		// Taken from wikipedia
		int x0 = 0;
		int y0 = radius;
		int d = 3 - 2 * radius;
		if (!radius)
			return;

		auto drawline = [&](int sx, int ex, int ny) {
			for (int i = sx; i <= ex; i++)
				Draw(i, ny, p);
		};

		while (y0 >= x0)
		{
			// Modified to draw scan-lines instead of edges
			drawline(x - x0, x + x0, y - y0);
			drawline(x - y0, x + y0, y - x0);
			drawline(x - x0, x + x0, y + y0);
			drawline(x - y0, x + y0, y + x0);
			if (d < 0)
				d += 4 * x0++ + 6;
			else
				d += 4 * (x0++ - y0--) + 10;
		}
	}

	void PixelGameEngine::DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p)
	{
		DrawLine(x, y, x + w, y, p);
		DrawLine(x + w, y, x + w, y + h, p);
		DrawLine(x + w, y + h, x, y + h, p);
		DrawLine(x, y + h, x, y, p);
	}

	void PixelGameEngine::FillRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p)
	{
		int32_t x2 = x + w;
		int32_t y2 = y + h;

		if (x < 0)
			x = 0;
		if (x >= (int32_t)nScreenWidth)
			x = (int32_t)nScreenWidth;
		if (y < 0)
			y = 0;
		if (y >= (int32_t)nScreenHeight)
			y = (int32_t)nScreenHeight;

		if (x2 < 0)
			x2 = 0;
		if (x2 >= (int32_t)nScreenWidth)
			x2 = (int32_t)nScreenWidth;
		if (y2 < 0)
			y2 = 0;
		if (y2 >= (int32_t)nScreenHeight)
			y2 = (int32_t)nScreenHeight;

		for (int i = x; i < x2; i++)
			for (int j = y; j < y2; j++)
				Draw(i, j, p);
	}

	void PixelGameEngine::DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p)
	{
		DrawLine(x1, y1, x2, y2, p);
		DrawLine(x2, y2, x3, y3, p);
		DrawLine(x3, y3, x1, y1, p);
	}

	// https://www.avrfreaks.net/sites/default/files/triangles.c
	void PixelGameEngine::FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p)
	{
		auto SWAP = [](int &x, int &y) { int t = x; x = y; y = t; };
		auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) Draw(i, ny, p); };

		int t1x, t2x, y, minx, maxx, t1xp, t2xp;
		bool changed1 = false;
		bool changed2 = false;
		int signx1, signx2, dx1, dy1, dx2, dy2;
		int e1, e2;
		// Sort vertices
		if (y1 > y2)
		{
			SWAP(y1, y2);
			SWAP(x1, x2);
		}
		if (y1 > y3)
		{
			SWAP(y1, y3);
			SWAP(x1, x3);
		}
		if (y2 > y3)
		{
			SWAP(y2, y3);
			SWAP(x2, x3);
		}

		t1x = t2x = x1;
		y = y1; // Starting points
		dx1 = (int)(x2 - x1);
		if (dx1 < 0)
		{
			dx1 = -dx1;
			signx1 = -1;
		}
		else
			signx1 = 1;
		dy1 = (int)(y2 - y1);

		dx2 = (int)(x3 - x1);
		if (dx2 < 0)
		{
			dx2 = -dx2;
			signx2 = -1;
		}
		else
			signx2 = 1;
		dy2 = (int)(y3 - y1);

		if (dy1 > dx1)
		{ // swap values
			SWAP(dx1, dy1);
			changed1 = true;
		}
		if (dy2 > dx2)
		{ // swap values
			SWAP(dy2, dx2);
			changed2 = true;
		}

		e2 = (int)(dx2 >> 1);
		// Flat top, just process the second half
		if (y1 == y2)
			goto next;
		e1 = (int)(dx1 >> 1);

		for (int i = 0; i < dx1;)
		{
			t1xp = 0;
			t2xp = 0;
			if (t1x < t2x)
			{
				minx = t1x;
				maxx = t2x;
			}
			else
			{
				minx = t2x;
				maxx = t1x;
			}
			// process first line until y value is about to change
			while (i < dx1)
			{
				i++;
				e1 += dy1;
				while (e1 >= dx1)
				{
					e1 -= dx1;
					if (changed1)
						t1xp = signx1; //t1x += signx1;
					else
						goto next1;
				}
				if (changed1)
					break;
				else
					t1x += signx1;
			}
			// Move line
		next1:
			// process second line until y value is about to change
			while (1)
			{
				e2 += dy2;
				while (e2 >= dx2)
				{
					e2 -= dx2;
					if (changed2)
						t2xp = signx2; //t2x += signx2;
					else
						goto next2;
				}
				if (changed2)
					break;
				else
					t2x += signx2;
			}
		next2:
			if (minx > t1x)
				minx = t1x;
			if (minx > t2x)
				minx = t2x;
			if (maxx < t1x)
				maxx = t1x;
			if (maxx < t2x)
				maxx = t2x;
			drawline(minx, maxx, y); // Draw line from min to max points found on the y
									 // Now increase y
			if (!changed1)
				t1x += signx1;
			t1x += t1xp;
			if (!changed2)
				t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y == y2)
				break;
		}
	next:
		// Second half
		dx1 = (int)(x3 - x2);
		if (dx1 < 0)
		{
			dx1 = -dx1;
			signx1 = -1;
		}
		else
			signx1 = 1;
		dy1 = (int)(y3 - y2);
		t1x = x2;

		if (dy1 > dx1)
		{ // swap values
			SWAP(dy1, dx1);
			changed1 = true;
		}
		else
			changed1 = false;

		e1 = (int)(dx1 >> 1);

		for (int i = 0; i <= dx1; i++)
		{
			t1xp = 0;
			t2xp = 0;
			if (t1x < t2x)
			{
				minx = t1x;
				maxx = t2x;
			}
			else
			{
				minx = t2x;
				maxx = t1x;
			}
			// process first line until y value is about to change
			while (i < dx1)
			{
				e1 += dy1;
				while (e1 >= dx1)
				{
					e1 -= dx1;
					if (changed1)
					{
						t1xp = signx1;
						break;
					} //t1x += signx1;
					else
						goto next3;
				}
				if (changed1)
					break;
				else
					t1x += signx1;
				if (i < dx1)
					i++;
			}
		next3:
			// process second line until y value is about to change
			while (t2x != x3)
			{
				e2 += dy2;
				while (e2 >= dx2)
				{
					e2 -= dx2;
					if (changed2)
						t2xp = signx2;
					else
						goto next4;
				}
				if (changed2)
					break;
				else
					t2x += signx2;
			}
		next4:

			if (minx > t1x)
				minx = t1x;
			if (minx > t2x)
				minx = t2x;
			if (maxx < t1x)
				maxx = t1x;
			if (maxx < t2x)
				maxx = t2x;
			drawline(minx, maxx, y);
			if (!changed1)
				t1x += signx1;
			t1x += t1xp;
			if (!changed2)
				t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y > y3)
				return;
		}
	}

	void PixelGameEngine::DrawSprite(int32_t x, int32_t y, Sprite *sprite)
	{
		if (sprite == nullptr)
			return;

		for (int i = 0; i < sprite->width; i++)
		{
			for (int j = 0; j < sprite->height; j++)
			{
				Draw(x + i, y + j, sprite->GetPixel(i, j));
			}
		}
	}

	void PixelGameEngine::DrawPartialSprite(int32_t x, int32_t y, Sprite *sprite, int32_t ox, int32_t oy, int32_t w, int32_t h)
	{
		if (sprite == nullptr)
			return;

		for (int i = 0; i < w; i++)
		{
			for (int j = 0; j < h; j++)
			{
				Draw(x + i, y + j, sprite->GetPixel(i + ox, j + oy));
			}
		}
	}

	void PixelGameEngine::SetPixelMode(Pixel::Mode m)
	{
		nPixelMode = m;
	}

	// User must override these functions as required. I have not made
	// them abstract because I do need a default behaviour to occur if
	// they are not overwritten
	bool PixelGameEngine::OnUserCreate()
	{
		return false;
	}
	bool PixelGameEngine::OnUserUpdate(float fElapsedTime)
	{
		return false;
	}
	bool PixelGameEngine::OnUserDestroy()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////

	void PixelGameEngine::olc_UpdateMouse(uint32_t x, uint32_t y)
	{
		// Mouse coords come in screen space
		// But leave in pixel space
		nMousePosX = x / nPixelWidth;
		nMousePosY = y / nPixelHeight;
	}

	void PixelGameEngine::EngineThread()
	{
		// Start OpenGL, the context is owned by the game thread
		olc_OpenGLCreate();

		// Create Screen Texture - disable filtering
		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &glBuffer);
		glBindTexture(GL_TEXTURE_2D, glBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		// Create user resources as part of this thread
		if (!OnUserCreate())
			bAtomActive = false;

		auto tp1 = std::chrono::system_clock::now();
		auto tp2 = std::chrono::system_clock::now();

		while (bAtomActive)
		{
			// Run as fast as possible
			while (bAtomActive)
			{
				// Handle Timing
				tp2 = std::chrono::system_clock::now();
				std::chrono::duration<float> elapsedTime = tp2 - tp1;
				tp1 = tp2;

				// Our time per frame coefficient
				float fElapsedTime = elapsedTime.count();

				// Handle User Input - Keyboard
				for (int i = 0; i < 256; i++)
				{
					pKeyboardState[i].bPressed = false;
					pKeyboardState[i].bReleased = false;

					if (pKeyNewState[i] != pKeyOldState[i])
					{
						if (pKeyNewState[i])
						{
							pKeyboardState[i].bPressed = !pKeyboardState[i].bHeld;
							pKeyboardState[i].bHeld = true;
						}
						else
						{
							pKeyboardState[i].bReleased = true;
							pKeyboardState[i].bHeld = false;
						}
					}

					pKeyOldState[i] = pKeyNewState[i];
				}

				// Handle User Input - Mouse
				for (int i = 0; i < 5; i++)
				{
					pMouseState[i].bPressed = false;
					pMouseState[i].bReleased = false;

					if (pMouseNewState[i] != pMouseOldState[i])
					{
						if (pMouseNewState[i])
						{
							pMouseState[i].bPressed = !pMouseState[i].bHeld;
							pMouseState[i].bHeld = true;
						}
						else
						{
							pMouseState[i].bReleased = true;
							pMouseState[i].bHeld = false;
						}
					}

					pMouseOldState[i] = pMouseNewState[i];
				}

				// Handle Frame Update
				if (!OnUserUpdate(fElapsedTime))
					bAtomActive = false;

				// Display Graphics

				// TODO: This is a bit slow (especially in debug, but 100x faster in release mode???)
				// Copy pixel array into texture
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nScreenWidth, nScreenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pDefaultDrawTarget->GetData());

				// Display texture on screen
				glBegin(GL_QUADS);
				glTexCoord2f(0.0, 1.0);
				glVertex3f(-1.0f, -1.0f, 0.0f);
				glTexCoord2f(0.0, 0.0);
				glVertex3f(-1.0f, 1.0f, 0.0f);
				glTexCoord2f(1.0, 0.0);
				glVertex3f(1.0f, 1.0f, 0.0f);
				glTexCoord2f(1.0, 1.0);
				glVertex3f(1.0f, -1.0f, 0.0f);
				glEnd();

				// Present Graphics to screen
				SwapBuffers(glDeviceContext);

				// Update Title Bar
#ifdef UNICODE
				wchar_t sTitle[256];
				swprintf(sTitle, 256, L"OneLoneCoder.com - Pixel Game Engine - %s - FPS: %3.2f", wsAppName.c_str(), 1.0f / fElapsedTime);
				SetWindowText(olc_hWnd, sTitle);
#else
				char sTitle[256];
				sprintf(sTitle, "OneLoneCoder.com - Pixel Game Engine - %s - FPS: %3.2f", sAppName.c_str(), 1.0f / fElapsedTime);
				SetWindowText(olc_hWnd, sTitle);
#endif
			}

			// Allow the user to free resources if they have overrided the destroy function
			if (OnUserDestroy())
			{
				// User has permitted destroy, so exit and clean up
			}
			else
			{
				// User denied destroy for some reason, so continue running
				bAtomActive = true;
			}
		}

		wglDeleteContext(glRenderContext);
		PostMessage(olc_hWnd, WM_DESTROY, 0, 0);
	}

	HWND PixelGameEngine::olc_WindowCreate()
	{
		WNDCLASS wc;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpfnWndProc = olc_WindowEvent;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.lpszMenuName = nullptr;
		wc.hbrBackground = nullptr;
#ifdef UNICODE
		wc.lpszClassName = L"OLC_PIXEL_GAME_ENGINE";
#else
		wc.lpszClassName = "OLC_PIXEL_GAME_ENGINE";
#endif

		RegisterClass(&wc);

		// Define window furniture
		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_VISIBLE;
		RECT rWndRect = {0, 0, (LONG)nScreenWidth * (LONG)nPixelWidth, (LONG)nScreenHeight * (LONG)nPixelHeight};

		// Keep client size as requested
		AdjustWindowRectEx(&rWndRect, dwStyle, FALSE, dwExStyle);

		int width = rWndRect.right - rWndRect.left;
		int height = rWndRect.bottom - rWndRect.top;

#ifdef UNICODE
		olc_hWnd = CreateWindowEx(dwExStyle, L"OLC_PIXEL_GAME_ENGINE", L"", dwStyle,
								  CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, GetModuleHandle(nullptr), this);
#else
		olc_hWnd = CreateWindowEx(dwExStyle, "OLC_PIXEL_GAME_ENGINE", "", dwStyle,
								  CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, GetModuleHandle(nullptr), this);
#endif

		// Create Keyboard Mapping
		mapKeys[0x41] = Key::A;
		mapKeys[0x42] = Key::B;
		mapKeys[0x43] = Key::C;
		mapKeys[0x44] = Key::D;
		mapKeys[0x45] = Key::E;
		mapKeys[0x46] = Key::F;
		mapKeys[0x47] = Key::G;
		mapKeys[0x48] = Key::H;
		mapKeys[0x49] = Key::I;
		mapKeys[0x4A] = Key::J;
		mapKeys[0x4B] = Key::K;
		mapKeys[0x4C] = Key::L;
		mapKeys[0x4D] = Key::M;
		mapKeys[0x4E] = Key::N;
		mapKeys[0x4F] = Key::O;
		mapKeys[0x50] = Key::P;
		mapKeys[0x51] = Key::Q;
		mapKeys[0x52] = Key::R;
		mapKeys[0x53] = Key::S;
		mapKeys[0x54] = Key::T;
		mapKeys[0x55] = Key::U;
		mapKeys[0x56] = Key::V;
		mapKeys[0x57] = Key::W;
		mapKeys[0x58] = Key::X;
		mapKeys[0x59] = Key::Y;
		mapKeys[0x5A] = Key::Z;

		mapKeys[VK_F1] = Key::F1;
		mapKeys[VK_F2] = Key::F2;
		mapKeys[VK_F3] = Key::F3;
		mapKeys[VK_F4] = Key::F4;
		mapKeys[VK_F5] = Key::F5;
		mapKeys[VK_F6] = Key::F6;
		mapKeys[VK_F7] = Key::F7;
		mapKeys[VK_F8] = Key::F8;
		mapKeys[VK_F9] = Key::F9;
		mapKeys[VK_F10] = Key::F10;
		mapKeys[VK_F11] = Key::F11;
		mapKeys[VK_F12] = Key::F12;

		mapKeys[VK_DOWN] = Key::DOWN;
		mapKeys[VK_LEFT] = Key::LEFT;
		mapKeys[VK_RIGHT] = Key::RIGHT;
		mapKeys[VK_UP] = Key::UP;

		mapKeys[VK_BACK] = Key::BACK;
		mapKeys[VK_ESCAPE] = Key::ESCAPE;
		mapKeys[VK_RETURN] = Key::ENTER;
		mapKeys[VK_PAUSE] = Key::PAUSE;
		mapKeys[VK_SCROLL] = Key::SCROLL;
		mapKeys[VK_TAB] = Key::TAB;
		mapKeys[VK_DELETE] = Key::DEL;
		mapKeys[VK_HOME] = Key::HOME;
		mapKeys[VK_END] = Key::END;
		mapKeys[VK_PRIOR] = Key::PGUP;
		mapKeys[VK_NEXT] = Key::PGDN;
		mapKeys[VK_INSERT] = Key::INS;
		mapKeys[VK_LSHIFT] = Key::LSHIFT;
		mapKeys[VK_RSHIFT] = Key::RSHIFT;
		mapKeys[VK_LCONTROL] = Key::LCTRL;
		mapKeys[VK_RCONTROL] = Key::RCTRL;
		mapKeys[VK_SPACE] = Key::SPACE;

		mapKeys[0x30] = Key::K0;
		mapKeys[0x31] = Key::K1;
		mapKeys[0x32] = Key::K2;
		mapKeys[0x33] = Key::K3;
		mapKeys[0x34] = Key::K4;
		mapKeys[0x35] = Key::K5;
		mapKeys[0x36] = Key::K6;
		mapKeys[0x37] = Key::K7;
		mapKeys[0x38] = Key::K8;
		mapKeys[0x39] = Key::K9;

		return olc_hWnd;
	}

	bool PixelGameEngine::olc_OpenGLCreate()
	{
		// Create Device Context
		glDeviceContext = GetDC(olc_hWnd);
		PIXELFORMATDESCRIPTOR pfd =
			{
				sizeof(PIXELFORMATDESCRIPTOR), 1,
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
				PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				PFD_MAIN_PLANE, 0, 0, 0, 0};

		int pf = 0;
		if (!(pf = ChoosePixelFormat(glDeviceContext, &pfd)))
			return false;
		SetPixelFormat(glDeviceContext, pf, &pfd);

		if (!(glRenderContext = wglCreateContext(glDeviceContext)))
			return false;
		wglMakeCurrent(glDeviceContext, glRenderContext);

		// Remove Frame cap
		wglSwapInterval = (wglSwapInterval_t *)wglGetProcAddress("wglSwapIntervalEXT");
		wglSwapInterval(0);
		return true;
	}

	// Windows Event Handler
	LRESULT CALLBACK PixelGameEngine::olc_WindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static PixelGameEngine *sge;
		switch (uMsg)
		{
		case WM_CREATE:
			sge = (PixelGameEngine *)((LPCREATESTRUCT)lParam)->lpCreateParams;
			return 0;
		case WM_MOUSEMOVE:
			sge->olc_UpdateMouse(LOWORD(lParam), HIWORD(lParam));
			return 0;
		case WM_SETFOCUS:
			sge->bHasInputFocus = true;
			return 0;
		case WM_KILLFOCUS:
			sge->bHasInputFocus = false;
			return 0;
		case WM_KEYDOWN:
			sge->pKeyNewState[mapKeys[wParam]] = true;
			return 0;
		case WM_KEYUP:
			sge->pKeyNewState[mapKeys[wParam]] = false;
			return 0;
		case WM_LBUTTONDOWN:
			sge->pMouseNewState[0] = true;
			return 0;
		case WM_LBUTTONUP:
			sge->pMouseNewState[0] = false;
			return 0;
		case WM_RBUTTONDOWN:
			sge->pMouseNewState[1] = true;
			return 0;
		case WM_RBUTTONUP:
			sge->pMouseNewState[1] = false;
			return 0;
		case WM_MBUTTONDOWN:
			sge->pMouseNewState[2] = true;
			return 0;
		case WM_MBUTTONUP:
			sge->pMouseNewState[2] = false;
			return 0;
		case WM_CLOSE:
			bAtomActive = false;
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	// Need a couple of statics as these are singleton instances
	// read from multiple locations
	std::atomic<bool> PixelGameEngine::bAtomActive{false};
	std::map<uint16_t, uint8_t> PixelGameEngine::mapKeys;

	//End pixelGameEngine
	//=============================================================
} // namespace olc