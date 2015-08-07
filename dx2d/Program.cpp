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

	for (CPolygon* p : vp)
		p->Color = XMFLOAT4(1, 1, 1, 1);

	for (int i = 0; i < vp.size() - 1;i++)
		for (int j = i+1; j < vp.size(); j++)
	{
		if (Collision::IsCollidingSat(vp[i], vp[j]))
		{
			if (i == 0)
			{
				vp[i]->Color = XMFLOAT4(1, 0, 0, 1);
				vp[j]->Color = XMFLOAT4(1, 0, 0, 1);
			}
			else
			{
				vp[i]->Color = XMFLOAT4(0, 0, 1, 1);
				vp[j]->Color = XMFLOAT4(0, 0, 1, 1);
			}
		}
	}
	gt = sinf((float)Core->GetGameTime()*30)/100;
}

int main(int argc, char** argv)
{
	auto si = sizeof(CDynamic);
	for(int i=0;i<1;i++)
	{
		Functions::InitCore(800, 600, Activity);

		EventManager->AddEvent([]()	{
			fps = (int)Core->GetFps();
			return 1;	}, L"", 0, 0, 0.5f);
		EventManager->AddEvent([]()	{
			DebugManager->Debug(fps, L"FPS");
			return 1;	}, L"", 0, 0, 0);

		XMFLOAT2 f2[] = { { 0,0 },{ 0,1 },{ 0.5f,2 },{ 1,1 },{ 1,0 },{ 0,0 } };
		
		for (int i = 0; i < 200; i++)
		{
			
			auto p2 = DrawManager->AddPoly(f2, 6);
			p2->TransformVertices = true;
			p2->Color = XMFLOAT4(1, 1, 1, 1);
			float x = (float)Functions::RndDouble() * 40.0f - 20.0f;
			float y = (float)Functions::RndDouble() * 30.0f - 15.0f;
			float rot = (float)Functions::RndDouble() * XM_PI;
			p2->SetPosition(x, y, 0);
			p2->SetRotationZ(rot);
			vp.push_back(p2);
		}

		vp[0]->SetPosition(0, 0, 0);
		p1 = vp[0];

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