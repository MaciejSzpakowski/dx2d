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
		Camera->SetVelocityX(move);*/
	/*Camera->SetPosition(p1->GetPosition());
	Camera->SetPositionZ(-20);*/

	p1->Color = p2->Color = p3->Color = XMFLOAT4(1, 1, 1, 1);
	pa->SetColor(1, 0, 0, 1);
	p1->SetVelocity(0, 0, 0);
	p1->SetAngularVelZ(0);
	p2->SetVelocity(0, 0, 0);
	p2->SetAngularVelZ(0);
	p3->SetVelocity(0, 0, 0);
	p3->SetAngularVelZ(0);
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
		pa->SetAngularVelZ(5);
	if (Input->IsKeyDown('E'))
		pa->SetAngularVelZ(-5);/**/

	if (Input->IsKeyPressed('1'))
		pa = p1;
	if (Input->IsKeyPressed('2'))
		pa = p2;
	if (Input->IsKeyPressed('3'))
		pa = p3;

	wstringstream ws;
	auto pos = p1->GetAbsolutePosition();
	ws << pos.x << " " << pos.y;
	DebugManager->Debug(ws.str(), L"p1 absolute position");
	auto cur = Camera->GetCursorWorldPos(0);
	wstringstream ws1;
	ws1 << cur.x << " " << cur.y;
	DebugManager->Debug(ws1.str(), L"cur position");
}

int main(int argc, char** argv)
{
	Functions::InitCore(800, 600, Activity);

	EventManager->AddEvent([]()	{
		fps = (int)Core->GetFps();
		return 1;	}, L"", 0, 0, 0.5f);
	EventManager->AddEvent([]()	{
		DebugManager->Debug(fps, L"FPS");
		return 1;	}, L"", 0, 0, 0);

	XMFLOAT2 f2[] = { { 0,0 },{ 0,1 },{ 0.5f,2 },{ 1,1 },{ 1,0 },{ 0,0 } };
	p1 = DrawManager->AddPoly(f2, 6);
	p2 = DrawManager->AddPoly(f2, 6);
	p3 = DrawManager->AddPoly(f2, 6);
	pa = p1;
	p1->SetParent(p2);
	p2->SetParent(p3);

	Core->Run();
	Core->Destroy();
	return 0;
}