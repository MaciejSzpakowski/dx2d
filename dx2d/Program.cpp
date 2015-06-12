#include "Private.h"
#include <gdiplus.h>
#include <ctime>
#pragma comment(lib,"Gdiplus.lib")

using namespace dx2d;

CCircle* c2;
CCircle* c3;
CSprite* brick;
CSprite* s1, *s2;
CRectangle* r1;
CAnimation* a1;
std::vector<CSprite*> sprites;
int t1;

void Activity()
{
	Sleep(1);
	if ((int)time(0) != t1)
	{
		t1 = (int)time(0);
		char sfps[20];
		sprintf_s(sfps, "%i", (int)Core->GetFps());
		Core->SetWindowTitle(sfps);
	}

	Camera->Velocity = XMFLOAT3(0, 0, 0);
	Camera->Spin.z = 0;
	float move = 10;
	if (Input->IsKeyDown(Key.Up))
		Camera->Velocity.y = move;
	if (Input->IsKeyDown(Key.Down))
		Camera->Velocity.y = -move;
	if (Input->IsKeyDown(Key.Left))
		Camera->Velocity.x = -move;
	if (Input->IsKeyDown(Key.Right))
		Camera->Velocity.x = move;
	if (Input->IsKeyDown('Q'))
		Camera->Spin.z = move/4;
	if (Input->IsKeyDown('E'))
		Camera->Spin.z = -move/4;

	if (Input->IsKeyPressed(' '))
		printf("%.10f %d\n", Functions::RndDouble(), Functions::RndInt(-500, 500));
}

int main(int argc, char** argv)
{
	Functions::NewCore(800, 600, Activity);	

	Core->OpenConsole();
	Core->SetBackgroundColor(SColor(0, 0, 0, 1));
	srand((int)time(0));
	t1 = (int)time(0);
	Core->SetWindowTitle("Hello");
	c2 = DrawManager->AddCircle(2, 20);
	c2->Position = XMFLOAT3(0, 0, 0);
	c2->Color = SColor(0, 0, 1, 0);
	c3 = DrawManager->AddCircle(2, 3);
	c3->Position = XMFLOAT3(10, 10, 0);
	c3->Spin = XMFLOAT3(0, 0, -4);
	c3->Color = SColor(1.0f, 0.6f, 0, 0);
	r1 = DrawManager->AddRect(2, 4);
	r1->Position = XMFLOAT3(-3, 2, 0);
	r1->Spin = XMFLOAT3(0, 0, 3);
	r1->Color = SColor(1, 0, 0.0f, 0);
	brick = DrawManager->AddSprite(L"brick.jpg");
	brick->Scale = XMFLOAT2(22, 17);
	brick->Position.z = 0.03f;
	brick->Color.a = 1;
	a1 = DrawManager->AddAnimation(L"ani.png", 2, 4);
	a1->TexFilter = TEX_FILTER::LINEAR;
	a1->Speed = 15;
	a1->Scale = XMFLOAT2(6, 3);
	/*s1 = DrawManager->AddSprite(L"leaf.png");
	s1->Scale = XMFLOAT2(10, 10);
	s1->Position.z = -0.5f;
	s1->Color.a = 0.5f;*/
	CText* t1 = DrawManager->AddText(L"Hello World");
	t1->TexFilter = TEX_FILTER::LINEAR;
	t1->Position = XMFLOAT3(-10, 10, 0);
	t1->Scale = XMFLOAT2(3, 3);


	Core->Run();
	Core->Destroy();
	return 0;
}