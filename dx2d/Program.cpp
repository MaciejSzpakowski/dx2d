#include "Private.h"
#include <gdiplus.h>
#include <ctime>
#pragma comment(lib,"Gdiplus.lib")

using namespace dx2d;

Circle* c1;
Circle* c2;
Circle* c3;
Sprite* brick;
dx2d::Rectangle* r1;
std::vector<Sprite*> sprites;
int t1;
int t2;
int fps;

void Activity()
{
	Sleep(1);
	fps++;
	if ((t2 = time(0)) != t1)
	{
		t1 = t2;
		char sfps[10];
		sprintf(sfps, "%i", fps);
		fps = 0;
		SetWindowText(Global->GetWindowHandle(), sfps);
	}

	Camera->Velocity = XMFLOAT3(0, 0, 0);
	Camera->Spin.z = 0;
	float move = 0.01f;
	if (InputManager->IsKeyDown(Key.Up))
		Camera->Velocity.y = move;
	if (InputManager->IsKeyDown(Key.Down))
		Camera->Velocity.y = -move;
	if (InputManager->IsKeyDown(Key.Left))
		Camera->Velocity.x = -move;
	if (InputManager->IsKeyDown(Key.Right))
		Camera->Velocity.x = move;
	if (InputManager->IsKeyDown('Q'))
		Camera->Spin.z = move;
	if (InputManager->IsKeyDown('E'))
		Camera->Spin.z = -move;
}

int main(int argc,char** argv)
{

	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	printf("Hello\n");
	Core* engine = NewCore(800, 600, Activity);

	srand(time(0));
	t1 = time(0);
	fps = 0;
	engine->SetWindowTitle("Hello");
	c2 = DrawManager->AddCircle(2, 20);
	c2->Position = XMFLOAT3(0, 5, 0);
	c2->Color = SColor(0, 0, 1, 0);
	c3 = DrawManager->AddCircle(2, 3);
	c3->Position = XMFLOAT3(10, 10, 0);
	c3->Spin = XMFLOAT3(0, 0, -0.004f);
	c3->Color = SColor(1.0f, 0.6f, 0, 0);
	r1 = DrawManager->AddRect(2, 4);
	r1->Position = XMFLOAT3(-3, 2, 0);
	r1->Spin = XMFLOAT3(0, 0, 0.003f);
	r1->Color = SColor(1, 0, 0.0f, 0);
	brick = DrawManager->AddSprite(L"C:/bajery/bLeNdEr/tekstury/bricks.jpeg");
	brick->Scale = XMFLOAT2(20, 15);
	brick->Position.z = 1;
	for (int i = 0; i <= 2000; i++)
	{
		if (i % 10 == 0)
			printf("%i\n", i);
		float x = (float)rand() / (2 << 15) * 40 - 10;
		float y = (float)rand() / (2 << 15) * 40 - 10;
		Sprite* s1 = DrawManager->AddSprite(L"c:/bajery/blender.png");
		s1->Position = XMFLOAT3(x, y, -1);
		s1->Spin.z = x / 333 + y/333;
		s1->Scale = XMFLOAT2(1, 1);
	}

	engine->Run();
	engine->Destroy();
	return 0;
}