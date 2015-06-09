#include "Private.h"
#include <gdiplus.h>
#include <ctime>
#pragma comment(lib,"Gdiplus.lib")

using namespace dx2d;

CCircle* c2;
CCircle* c3;
CSprite* brick;
CSprite* s1;
CRectangle* r1;
std::vector<CSprite*> sprites;
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
		sprintf_s(sfps, "%i", fps);
		fps = 0;
		Core->SetWindowTitle(sfps);
	}

	Camera->Velocity = XMFLOAT3(0, 0, 0);
	Camera->Spin.z = 0;
	float move = 0.01f;
	if (Input->IsKeyDown(Key.Up))
		Camera->Velocity.y = move;
	if (Input->IsKeyDown(Key.Down))
		Camera->Velocity.y = -move;
	if (Input->IsKeyDown(Key.Left))
		Camera->Velocity.x = -move;
	if (Input->IsKeyDown(Key.Right))
		Camera->Velocity.x = move;
	if (Input->IsKeyDown('Q'))
		Camera->Spin.z = move;
	if (Input->IsKeyDown('E'))
		Camera->Spin.z = -move;
	s1->Position.x += Input->GetCursorDelta().x / 20.0f;
	s1->Position.y -= Input->GetCursorDelta().y / 20.0f;

	if (Input->IsKeyDown('A'))
		brick->Position.z -= 0.01f;
	if (Input->IsKeyDown('Z'))
		brick->Position.z += 0.01f;

	if (Input->MouseWheel != 0)
		printf("%i ", Input->MouseWheel);
}

int main(int argc,char** argv)
{
	Functions::NewCore(800, 600, Activity);

	Core->OpenConsole();
	Core->SetBackgroundColor(SColor(0, 0, 0, 1));
	srand((int)time(0));
	t1 = (int)time(0);
	fps = 0;
	Core->SetWindowTitle("Hello");
	c2 = DrawManager->AddCircle(2, 20);
	c2->Position = XMFLOAT3(0, 5, -2);
	c2->Color = SColor(0, 0, 1, 0);
	c3 = DrawManager->AddCircle(2, 3);
	c3->Position = XMFLOAT3(10, 10, 0);
	c3->Spin = XMFLOAT3(0, 0, -0.004f);
	c3->Color = SColor(1.0f, 0.6f, 0, 0);
	r1 = DrawManager->AddRect(2, 4);
	r1->Position = XMFLOAT3(-3, 2, 0);
	r1->Spin = XMFLOAT3(0, 0, 0.003f);
	r1->Color = SColor(1, 0, 0.0f, 0);
	brick = DrawManager->AddSprite(L"brick.jpg");
	brick->Scale = XMFLOAT2(22, 17);
	brick->Position.z = 0.03f;
	brick->Color.a = 1;
	s1 = DrawManager->AddSprite(L"leaf.png");
	s1->Scale = XMFLOAT2(10, 10);
	s1->Position.z = -0.5f;
	s1->Color.a = 0.5f;
	CSprite* s2 = DrawManager->AddSprite(L"leaf.png");
	s2->Scale = XMFLOAT2(10, 10);
	s2->Position = XMFLOAT3(6, 0, -1);
	s2->Color.a = 0.5f;

	Core->Run();
	Core->Destroy();
	return 0;
}