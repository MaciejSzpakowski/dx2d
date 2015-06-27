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

void Activity()
{
	Sleep(1);	

	Camera->SetVelocity(0, 0, 0);
	Camera->SetAngularVelZ(0);
	float move = 10;
	if (Input->IsKeyDown(Key.Up))
		Camera->SetVelocityY(move);
	if (Input->IsKeyDown(Key.Down))
		Camera->SetVelocityY(-move);
	if (Input->IsKeyDown(Key.Left))
		Camera->SetVelocityX(-move);
	if (Input->IsKeyDown(Key.Right))
		Camera->SetVelocityX(move);

	a1->Color.r = 1;
	if (a1->IsUnderCursor())
		a1->Color.r = 3;
	a1->SetVelocity(0, 0, 0);
	a1->SetAngularVelZ(0);
	if (Input->IsKeyDown('A'))
		a1->SetVelocityX(-10);
	if (Input->IsKeyDown('D'))
		a1->SetVelocityX(10);
	if (Input->IsKeyDown('W'))
		a1->SetVelocityY(10);
	if (Input->IsKeyDown('S'))
		a1->SetVelocityY(-10);
	if (Input->IsKeyDown('Q'))
		a1->SetAngularVelZ(5);
	if (Input->IsKeyDown('E'))
		a1->SetAngularVelZ(-5);

	DebugManager->Debug(L"hello", L"h");
}

int main(int argc, char** argv)
{
	Functions::NewCore(800, 600, Activity);

	//fps
	EventManager->AddEvent([]()
	{
		wchar_t sfps[20];
		swprintf_s(sfps, 20, L"%i", (int)Core->GetFps());
		Core->SetWindowTitle(sfps);
		return 1;
	}, L"", 0, 0, 1);
	Core->OpenConsole();
	Core->SetBackgroundColor({ 0, 0, 0, 1 });
	c1 = DrawManager->AddCircle(1, 10);
	c2 = DrawManager->AddCircle(1, 10);
	c2->SetPositionX(10);
	brick = DrawManager->AddSprite(L"brick.jpg");
	brick->SetPositionZ(0.03f);
	brick->SetNaturalScale();
	brick->Color.a = 1;
	a1 = DrawManager->AddAnimation(L"ani.png", 2, 4);
	a1->Speed = 15;
	a1->SetPixelScale(512, 256);
	a1->Scale.x /= 3.0f;
	a1->Scale.y /= 3.0f;
	a1->Pickable = true;
	a1->Speed = 15;

	CRectangle* r1 = DrawManager->AddRect(1, 1);
	r1->Color = XMFLOAT4(0, 1, 0.2f, 0);
	EventManager->AddEvent([r1]()
	{
		if (Input->IsKeyPressed(Key.Right))
		{
			r1->SetVelocityX(1);
			XMFLOAT3 pos = r1->GetPosition();
			EventManager->AddEvent([r1,pos]()
			{
				if (r1->GetPosition().x <= pos.x + 1)
				{
					r1->SetVelocity(0, 0, 0);
					return 0;
				}
				return 1;
			}, L"", 0, 0, 0);
		}
		if (Input->IsKeyPressed(Key.Down))
		{
			r1->SetVelocityY(-1);
			XMFLOAT3 pos = r1->GetPosition();
			EventManager->AddEvent([r1, pos]()
			{
				if (r1->GetPosition().y <= pos.y - 1)
				{
					r1->SetVelocity(0, 0, 0);
					return 0;
				}
				return 1;
			}, L"", 0, 0, 0);
		}
		if (Input->IsKeyPressed(Key.Up))
		{
			r1->SetVelocityY(1);
			XMFLOAT3 pos = r1->GetPosition();
			EventManager->AddEvent([r1, pos]()
			{
				if (r1->GetPosition().y <= pos.y + 1)
				{
					r1->SetVelocity(0, 0, 0);
					return 0;
				}
				return 1;
			}, L"", 0, 0, 0);
		}
		if (Input->IsKeyPressed(Key.Left))
		{
			r1->SetVelocityX(-1);
			XMFLOAT3 pos = r1->GetPosition();
			EventManager->AddEvent([r1, pos]()
			{
				if (r1->GetPosition().x <= pos.x - 1)
				{
					r1->SetVelocity(0, 0, 0);
					return 0;
				}
				return 1;
			}, L"", 0, 0, 0);
		}
		return 1;
	}, L"", 0, 0, 0);

	int t1 = time(0);
	for (int i = 0; i < 2000; i++)
	{
		//Functions::CreateTexture2DFromFile(L"sun_flower.png");
	}
	printf("%is\n", time(0) - t1);

	Core->Run();
	Core->Destroy();
	return 0;
}