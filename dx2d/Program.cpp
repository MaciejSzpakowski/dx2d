#include "Private.h"
#include <ctime>

using namespace dx2d;

CCircle* c1, *c2, *c3;
CPolygon* p1, *p2, *p3;
CSprite* s1, *s2, *s3;
CAnimation* a1, *a2, *a3;
CBitmapText* t1, *t2, *t3;

int fps;
CDynamic* pa;

void Activity()
{
	Sleep(10);

	pa->SetVelocity(0, 0, 0);
	pa->SetAngularVelZ(0);
	if (Input->IsKeyDown('A'))
		pa->SetVelocityX(-10);
	if (Input->IsKeyDown('D'))
		pa->SetVelocityX(10);
	if (Input->IsKeyDown('W'))
		pa->SetVelocityY(10);
	if (Input->IsKeyDown('S'))
		pa->SetVelocityY(-10);
	if (Input->IsKeyDown('Q'))
		pa->SetVelocityZ(1);
	if (Input->IsKeyDown('E'))
		pa->SetVelocityZ(-1);/**/

	if (Input->IsKeyPressed('1'))
		pa = s1;
	if (Input->IsKeyPressed('2'))
		pa = s2;
	if (Input->IsKeyPressed('3'))
		pa = s3;

}

int main(int argc, char** argv)
{
	Functions::InitCore(800, 600, Activity);

	Core->SetBackgroundColor(Color(0,0,0,0));

	EventManager->AddEvent([]()	{
		fps = (int)Core->GetFps();
		return 1;	}, L"", 0, 0, 0.5f);
	EventManager->AddEvent([]()	{
		DebugManager->Debug(fps, L"FPS");
		return 1;	}, L"", 0, 0, 0);

	s1 = DrawManager->AddSprite(L"brick.jpg");
	s1->SetNaturalScale();
	s1->Size = 0.5f;

	s2 = DrawManager->AddSprite(L"brick.jpg");
	s2->SetNaturalScale();
	s2->Size = 0.5f;
	s2->Color.a = 0.5f;
	
	s3 = DrawManager->AddSprite(L"leaf.png");
	s3->SetNaturalScale();
	s3->Size = 0.5f;
	pa = s1;

	Core->Run();
	Core->Destroy();
	return 0;
}