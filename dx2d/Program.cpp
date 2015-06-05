#include "Private.h"
#include <gdiplus.h>
#pragma comment(lib,"Gdiplus.lib")

using namespace dx2d;

Circle* c1;
Circle* c2;
Circle* c3;
dx2d::Rectangle* r1;

void Activity()
{
	Sleep(10);

	Camera->Velocity = XMFLOAT3(0, 0, 0);
	Camera->Spin.z = 0;
	if (InputManager->IsKeyDown(Key.Up))
		Camera->Velocity.y = 0.1f;
	if (InputManager->IsKeyDown(Key.Down))
		Camera->Velocity.y = -0.1f;
	if (InputManager->IsKeyDown(Key.Left))
		Camera->Velocity.x = -0.1f;
	if (InputManager->IsKeyDown(Key.Right))
		Camera->Velocity.x = 0.1f;
	if (InputManager->IsKeyDown('Q'))
		Camera->Spin.z = 0.01f;
	if (InputManager->IsKeyDown('E'))
		Camera->Spin.z = -0.01f;
}

int main(int argc,char** argv)
{
	ULONG_PTR m_gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	Gdiplus::Bitmap* i = new Gdiplus::Bitmap(L"a.bmp");
	UINT h = i->GetHeight();
	UINT w = i->GetWidth();
	HBITMAP b;
	Gdiplus::Color c(0, 0, 0);
	i->GetHBITMAP(c, &b);
	Gdiplus::GdiplusShutdown(m_gdiplusToken);

	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	printf("Hello\n");
	Core* engine = NewCore(800, 600, Activity);

	engine->SetWindowTitle("Hello");
	c2 = DrawManager->AddCircle(2, 20);
	c2->Position = XMFLOAT3(0, 5, 0);
	c2->Color = SColor(0, 0, 1, 0);
	c3 = DrawManager->AddCircle(2, 3);
	c3->Position = XMFLOAT3(10, 10, 0);
	c3->Spin = XMFLOAT3(0, 0, -0.04f);
	c3->Color = SColor(1.0f, 0.6f, 0, 0);
	r1 = DrawManager->AddRect(2, 4);
	r1->Position = XMFLOAT3(-3, 2, 0);
	r1->Spin = XMFLOAT3(0, 0, 0.03f);
	r1->Color = SColor(1, 0, 0.0f, 0);
	Sprite* s = DrawManager->AddSprite("");
	s->Scale = XMFLOAT2(3, 3);
	s->Color = SColor(0, 0.5, 0.5, 1);

	engine->Run();
	engine->Destroy();
	return 0;
}