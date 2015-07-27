#include "Private.h"
#include <ctime>

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

	/*Camera->SetVelocity(0, 0, 0);
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
		a1->SetAngularVelZ(-5);*/

	Input->zTest();
}

int main(int argc, char** argv)
{
	Functions::InitCore(400, 300, Activity);

	/*EventManager->AddEvent([]()	{
		fps = (int)Core->GetFps();
		return 1;	}, L"", 0, 0, 0.5f);
	EventManager->AddEvent([]()	{
		DebugManager->Debug(fps, L"FPS");
		return 1;	}, L"", 0, 0, 0);

	Core->SetWindowTitle(L"Hello");
	Core->OpenConsole();
	Core->SetBackgroundColor({ 0, 0, 0, 1 });
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

	s1 = DrawManager->AddSprite(L"filter.png");
	s1->TexFilter = TEX_FILTER::LINEAR;
	s2 = DrawManager->AddSprite(L"filter.png");
	s1->TexFilter = TEX_FILTER::POINT;
	s2->SetPositionX(4);
	s1->SetPositionX(12);
	s1->Size = 4;
	s2->Size = 4;*/
	Core->OpenConsole();
	Input->AcceptGamepads = 1;

	Core->Run();
	Core->Destroy();
	return 0;
}