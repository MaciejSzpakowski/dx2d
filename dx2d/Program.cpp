#include "Private.h"
#include <ctime>

using namespace dx2d;

CCircle* c1,*c2,*c3;
CPolygon* p1;
vector<CPolygon*> vp;
CSprite* s1, *s2;
CAnimation* a1;
CBitmapText* t1;
int fps;
float gt;

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
	gt = (float)Core->GetGameTime();	
}

int main(int argc, char** argv)
{
	for(int i=0;i<1;i++)
	{
		Functions::InitCore(800, 600, Activity);

		EventManager->AddEvent([]()	{
			fps = (int)Core->GetFps();
			return 1;	}, L"", 0, 0, 0.5f);
		EventManager->AddEvent([]()	{
			DebugManager->Debug(fps, L"FPS");
			return 1;	}, L"", 0, 0, 0);

		XMFLOAT2 f1[] = { { 0, 0 }, { 0, 1 }, { 1, 1 } };
		p1 = DrawManager->AddPoly(f1, 3);
		p1->Color = XMFLOAT4(1, 1, 1, 1);

		for (int i = 0; i < 10; i++)
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

		s1 = DrawManager->AddSprite(L"brick.jpg");
		s1->SetNaturalScale();
		DrawManager->CreateExtraBuffer(16);		
		auto ps = Functions::CreatePSFromFile(L"ps.hlsl", "main");
		s1->PixelShader = ps;
		s1->SetExtraBufferPS(&gt);
		a1 = DrawManager->AddAnimation(L"ani.png", 2, 4);
		a1->Speed = 15;
		a1->SetPositionZ(-0.1f);

		Core->Run();
		vp.clear();
		Core->Destroy();
	}
	return 0;
}