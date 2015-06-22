#include "Private.h"
#include <ctime>
#include <Windows.h>

using namespace dx2d;

CCircle* c1,*c2,*c3;
CSprite* brick;
CSprite* s1, *s2;
CAnimation* a1;
CBitmapText* bmtext;
std::vector<CSprite*> sprites;
int t1;

void Activity()
{
	Sleep(1);
	if ((int)time(0) != t1)
	{
		t1 = (int)time(0);
		wchar_t sfps[20];
		swprintf_s(sfps, 20, L"%i", (int)Core->GetFps());
		Core->SetWindowTitle(sfps);
	}

	Camera->Velocity = XMFLOAT3(0, 0, 0);
	Camera->AngularVel.z = 0;
	float move = 10;
	if (Input->IsKeyDown(Key.Up))
		Camera->Velocity.y = move;
	if (Input->IsKeyDown(Key.Down))
		Camera->Velocity.y = -move;
	if (Input->IsKeyDown(Key.Left))
		Camera->Velocity.x = -move;
	if (Input->IsKeyDown(Key.Right))
		Camera->Velocity.x = move;

	if (Input->IsKeyDown(Key.MouseLeft))
	{
		POINTF pf = Camera->GetCursorWorldPos(0);
		c1->Position = XMFLOAT3(pf.x, pf.y, 0);
	}

	DebugManager->Debug(c1->Position.x, L"x");
	DebugManager->Debug(c1->Position.y, L"y");
}

int main(int argc, char** argv)
{
	Functions::NewCore(800, 600, Activity);
	t1 = (int)time(0);

	Core->OpenConsole();
	Core->SetBackgroundColor({ 0, 0, 0, 1 });
	c1 = DrawManager->AddCircle(0.5f, 5);
	c1->Position = XMFLOAT3(0, 0, 0);
	c1->Color = XMFLOAT4(1, 1, 1, 0);
	brick = DrawManager->AddSprite(L"brick.jpg");
	brick->Position.z = 0.03f;
	brick->SetNaturalScale();
	brick->Color.a = 1;
	a1 = DrawManager->AddAnimation(L"ani.png", 2, 4);
	a1->Speed = 15;
	a1->SetPixelScale(512, 256);
	a1->Scale.x /= 3.0f;
	a1->Scale.y /= 3.0f;
	EventManager->AddEvent([]()
	{
		a1->Speed = 0;
		a1->Velocity = XMFLOAT3(0, 0, 0);
		if (Input->IsKeyDown('A'))
		{
			a1->Speed = 15;
			a1->FlipHorizontally = true;
			a1->Velocity.x = -10;
		}
		else if (Input->IsKeyDown('D'))
		{
			a1->Speed = 15;
			a1->FlipHorizontally = false;
			a1->Velocity.x = 10;
		}
		else
			a1->Frame = 7;
		return 1;
	}, L"", 0);

	CRectangle* r1 = DrawManager->AddRect(1, 1);
	r1->Color = XMFLOAT4(1, 1, 1, 0);

	Core->Run();
	Core->Destroy();
	return 0;
}