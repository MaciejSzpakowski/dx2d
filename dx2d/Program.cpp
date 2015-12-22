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
		pa->SetAngularVelZ(1);
	if (Input->IsKeyDown('E'))
		pa->SetAngularVelZ(-1);

	if (Input->IsKeyPressed('1'))
		pa = p1;
	if (Input->IsKeyPressed('2'))
		pa = p2;
	if (Input->IsKeyPressed('3'))
		pa = p3;

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

	p1 = DrawManager->AddRect(1, 1);
	p1->Color = Color(1, 1, 1, 1);
	p2 = DrawManager->AddRect(1, 1);
	p2->Color = Color(1, 1, 1, 1);
	p3 = DrawManager->AddRect(1, 1);
	p3->Color = Color(1, 1, 1, 1);
	pa = p1;
	p1->SetParent(p2);
	p2->SetParent(p3);

	Core->Run();
	Core->Destroy();
	return 0;
}