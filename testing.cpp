#define OLC_PGE_APPLICATION

#include "PGE.h"

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(255, 128, 0));
		
		return true;
	}
};

int main()
{
	Example demo;
	if (demo.Construct(512, 480, 2, 2))
		demo.Start();

	return 0;
}