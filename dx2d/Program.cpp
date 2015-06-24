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

	DebugManager->Debug(a1->IsUnderCursor(), L"Cursor");
	c1->SetVelocityX(0);
	c2->Color = XMFLOAT4(0, 0, 0, 0);
	if (Input->IsKeyDown('A'))
		c1->SetVelocityX(-0.01f);
	if (Input->IsKeyDown('D'))
		c1->SetVelocityX(0.01f);
	if (Collision::IsColliding(c1, c2))
		c2->Color.r = 1;
}

int main(int argc, char** argv)
{
	Functions::NewCore(800, 600, Activity);
	t1 = (int)time(0);

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
	r1->SetVelocityX(0.01f);
	EventManager->AddEvent([r1]()
	{
		r1->SetVelocityX(-r1->GetVelocity().x);
		return 1;
	}, L"", 0, 0, 2);

	Core->Run();
	Core->Destroy();
	return 0;
}