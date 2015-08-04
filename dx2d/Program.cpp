#include "Private.h"
#include <ctime>

using namespace dx2d;

CCircle* c1,*c2,*c3;
CPolygon* p1;
vector<CPolygon*> vp;
CSprite* s1, *s2;
CAnimation* a1;
CBitmapText* t1;
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
		Camera->SetVelocityX(move);*/
	Camera->SetPosition(p1->GetPosition());
	Camera->SetPositionZ(-20);

	p1->SetVelocity(0, 0, 0);
	p1->SetAngularVelZ(0);
	if (Input->IsKeyDown('A'))
		p1->SetVelocityX(-10);
	if (Input->IsKeyDown('D'))
		p1->SetVelocityX(10);
	if (Input->IsKeyDown('W'))
		p1->SetVelocityY(10);
	if (Input->IsKeyDown('S'))
		p1->SetVelocityY(-10);
	if (Input->IsKeyDown('Q'))
		p1->SetAngularVelZ(5);
	if (Input->IsKeyDown('E'))
		p1->SetAngularVelZ(-5);/**/

	p1->Color = XMFLOAT4(1, 1, 1, 1);
	for (CPolygon* p : vp)
	{
		if (Collision::IsColliding(p1, p, nullptr))
		{
			p1->Color = XMFLOAT4(1, 0, 0, 1);
			p->Color = XMFLOAT4(1, 0, 0, 1);
		}
		else
			p->Color = XMFLOAT4(1, 1,1, 1);
	}
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

	/*Core->SetWindowTitle(L"Hello");
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

	XMFLOAT2 f1[] = { { 0, 0 }, { 0, 1 }, { 1, 1 } };
	p1 = DrawManager->AddPoly(f1, 3);
	p1->Color = XMFLOAT4(1, 1, 1, 1);

	for (int i = 0; i < 1000; i++)
	{
		XMFLOAT2 f2[] = { { -1, 1 }, { 0, 0 }, { 1, 1 } };
		auto p2 = DrawManager->AddPoly(f2, 3);
		p2->Color = XMFLOAT4(1, 1, 1, 1);
		float x = (float)Functions::RndDouble() * 200.0f - 100.0f;
		float y = (float)Functions::RndDouble() * 105.0f - 75.0f;
		float rot = (float)Functions::RndDouble() * XM_PI;
		p2->SetPosition(x, y, 0);
		p2->SetRotationZ(rot);
		vp.push_back(p2);
	}

	Core->Run();
	Core->Destroy();
	return 0;
}