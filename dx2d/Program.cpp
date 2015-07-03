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
int fps;

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

	s1->Color.b = 1;
	/*if (Collision::IsColliding(a1, s1))
		s1->Color.b = 3;*/
}

int main(int argc, char** argv)
{
	Functions::NewCore(800, 600, Activity);

	EventManager->AddEvent([]()	{
		fps = (int)Core->GetFps();
		return 1;	}, L"", 0, 0, 1);
	EventManager->AddEvent([]()	{
		DebugManager->Debug(fps, L"FPS");
		return 1;	}, L"", 0, 0, 0);

	Core->SetWindowTitle(L"Hello");
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

	s1 = DrawManager->AddSprite(L"ani.png");
	s1->UV = Rect( 0, 0, 0.5f, 0.25f );
	s1->SetRotationZ(0.2f);
	s1->SetPositionX(6);

	int t1 = (int)time(0);
	for (int i = 0; i < 2000; i++)
	{
		//Functions::CreateTexture2DFromFile(L"sun_flower.png");
	}
	printf("%is\n", time(0) - t1);

	Core->Run();
	Core->Destroy();
	return 0;
}