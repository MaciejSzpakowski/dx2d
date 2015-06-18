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
std::wstring cin;

void Activity()
{
	Sleep(1);
	if ((int)time(0) != t1)
	{
		t1 = (int)time(0);
		char sfps[20];
		sprintf_s(sfps, "%i", (int)Core->GetFps());
		Core->SetWindowTitle(sfps);
	}

	Camera->Velocity = XMFLOAT3(0, 0, 0);
	Camera->AngularVel.z = 0;
	float move = 10;
	if (Input->IsKeyDown(Key.Up))
		Camera->Velocity.y = move;
	if (Input->IsKeyDown(Key.Down))
		Camera->Velocity.y = -move;
	if (Input->IsKeyDown(Key.Left))
		Camera->Velocity.x = -move;
	if (Input->IsKeyDown(Key.Right))
		Camera->Velocity.x = move;
	c1->Velocity = XMFLOAT3(0, 0, 0);
	if (Input->IsKeyDown('W'))
		c1->Velocity.y = move;
	if (Input->IsKeyDown('S'))
		c1->Velocity.y = -move;
	if (Input->IsKeyDown('A'))
		c1->Velocity.x = -move;
	if (Input->IsKeyDown('D'))
		c1->Velocity.x = move;

	if (Input->IsKeyPressed('C'))
	{
		POINTF p = Core->GetCursorWorldPos(0);
		printf("cursor %.10f %.10f\n", p.x, p.y);
		printf("object %.10f %.10f\n",c1->Position.x, c1->Position.y);
		printf("ratio %.10f %.10f\n", p.x/c1->Position.x, p.y/c1->Position.y);
	}
}

int main(int argc, char** argv)
{
	Functions::NewCore(800, 600, Activity);
	t1 = (int)time(0);

	Core->OpenConsole();
	Core->SetBackgroundColor(SColor(0, 0, 0, 1));	
	Core->SetWindowTitle("Hello");
	c1 = DrawManager->AddCircle(0.4f, 5);
	c1->Position = XMFLOAT3(0, 0, 0);
	c1->Color = SColor(1, 1, 1, 0);	
	brick = DrawManager->AddSprite(L"brick.jpg");
	brick->Scale = XMFLOAT2(22, 17);
	brick->Position.z = 0.03f;
	brick->Color.a = 1;
	a1 = DrawManager->AddAnimation(L"ani.png", 2, 4);
	a1->TexFilter = TEX_FILTER::LINEAR;
	a1->Speed = 15;
	a1->Scale = XMFLOAT2(6, 3);

	//gothic font
	//36x40 one char
	//28x8 all chars
	vector<UV> chars;
	for (int i = 0; i<8; i++)
		for (int j = 0; j < 28; j++)
		{
			chars.push_back(UV(36.0f / 1024.0f*j, 40.0f / 512.0f*i, 36.0f / 1024.0f*(j + 1), 40.0f / 512.0f*(i + 1)));
		}
	CBitmapFont* bmfont = DrawManager->AddBitmapFont(L"font.png", chars);
	bmtext = DrawManager->AddBitmapText(bmfont);
	bmtext->Text = L"Bmtext test\nyes";
	bmtext->Position = XMFLOAT3(-15, 8, 0);
	bmtext->HorizontalAlign = TextHorAlign::Left;
	//bmtext->Parent = c2;

	CRectangle* r1 = DrawManager->AddRect(1, 1);
	r1->Color = XMFLOAT4(1, 1, 1, 0);
	EventManager->AddEvent([r1]()
	{
		if (Input->IsKeyDown('O'))
			r1->Scale.x += 0.01f;
		if (Input->IsKeyDown('P'))
			r1->Scale.x -= 0.01f;
		if (Input->IsKeyDown('K'))
			r1->Scale.y += 0.01f;
		if (Input->IsKeyDown('L'))
			r1->Scale.y -= 0.01f;
		wchar_t str[30];
		swprintf(str, L"%f %f", r1->Scale.x, r1->Scale.y);
		bmtext->Text = str;
		
		return 1;
	}, "", 0);

	Core->Run();
	Core->Destroy();
	return 0;
}