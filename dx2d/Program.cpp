#include "Private.h"
#include <gdiplus.h>
#include <ctime>
#pragma comment(lib,"Gdiplus.lib")

using namespace dx2d;

Circle* c2;
Circle* c3;
Sprite* brick;
Sprite* s1;
dx2d::Rectangle* r1;
std::vector<Sprite*> sprites;
int t1;
int t2;
int fps;

void Activity()
{
	Sleep(1);
	fps++;
	if ((t2 = (int)time(0)) != t1)
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
	
	static float angle = 0;
	//s1->Color.a = (sin(angle) + 1) / 2;
	//brick->Color.a = (sin(angle+XM_PI) + 1) / 2;
	if (InputManager->IsKeyDown('A'))
		brick->Position.z -= 0.01f;
	if (InputManager->IsKeyDown('Z'))
		brick->Position.z += 0.01f;
	angle += 0.002f;
	if (InputManager->IsKeyPressed('S'))
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		long long ll = li.QuadPart;
		printf("%llu\n", ll);
	}

	//char c1 = InputManager->GetChar();
	//if (c1 != 0)
	//	printf("%c", c1);
}

int main(int argc,char** argv)
{
	Core* engine = NewCore(800, 600, Activity);

	engine->OpenConsole();
	engine->SetBackgroundColor(SColor(0, 0, 0, 1));
	srand((int)time(0));
	t1 = (int)time(0);
	fps = 0;
	engine->SetWindowTitle("Hello");
	/*c2 = DrawManager->AddCircle(2, 20);
	c2->Position = XMFLOAT3(0, 5, 0);
	c2->Color = SColor(0, 0, 1, 0);
	/*c3 = DrawManager->AddCircle(2, 3);
	c3->Position = XMFLOAT3(10, 10, 0);
	c3->Spin = XMFLOAT3(0, 0, -0.004f);
	c3->Color = SColor(1.0f, 0.6f, 0, 0);
	r1 = DrawManager->AddRect(2, 4);
	r1->Position = XMFLOAT3(-3, 2, 0);
	r1->Spin = XMFLOAT3(0, 0, 0.003f);
	r1->Color = SColor(1, 0, 0.0f, 0);*/
	brick = DrawManager->AddSprite(L"brick.jpg");
	brick->Scale = XMFLOAT2(22, 17);
	brick->Position.z = 0.03f;
	s1 = DrawManager->AddSprite(L"leaf.png");
	s1->Scale = XMFLOAT2(10, 10);
	s1->Position.z = 0;
	Sprite* s2 = DrawManager->AddSprite(L"leaf.png");
	s2->Scale = XMFLOAT2(10, 10);
	s2->Position = XMFLOAT3(3, 0, -1);

	engine->Run();
	engine->Destroy();
	return 0;
}