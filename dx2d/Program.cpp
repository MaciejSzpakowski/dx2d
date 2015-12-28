#include "Viva.h"

using namespace Viva;

BitmapFont* courier;
CBitmapText* text1;
vector<std::function<void()>> tests;
vector<CDynamic*> obj;
vector<Event*> events;

void Activity()
{
	Sleep(10);
}

void test10()
{
	wstring msg = L"Test10: Render targets and pixel shaders\n";
	text1->Text = msg;

	CRenderTarget* r1 = DrawManager->AddRenderTarget();
	CSprite* s1 = DrawManager->AddSprite(L"brick.jpg",r1);
	s1->SetNaturalScale();

	const char ps2[] = R"(
cbuffer cbBufferPS
{
	float4 color;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 Col : COLOR;
	float2 TexCoord : TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float4 result = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
	if(input.TexCoord > 0.5)
		result = float4(0.0,0.0,0.0,1.0);
	clip(result.a - 0.001f);
	return result * color;
}
		)";

	r1->PixelShader = Core->CreatePixelShaderFromString(ps2, "main");
}

void test9()
{
	wstring msg = L"Test9: events\n";
	text1->Text = msg;

	vector<Pixel> pixels;
	pixels.push_back(Pixel(200, 200, 0, 255));

	CTexture* t1 = new CTexture(pixels.data(), Size(1, 1));
	CSprite* s1 = new CSprite(t1);
	DrawManager->AddSprite(s1);
	s1->SetPositionX(-10);
	s1->SetPositionY(-6);

	obj.push_back(s1);

	CBitmapText* text3 = DrawManager->AddBitmapText(courier);
	text3->color = Color(0, 0, 0, 1);
	text3->SetPositionX(-13);
	text3->SetPixelScale(10, 19);
	text3->size = 5;

	obj.push_back(text3);

	int* countDown = new int;
	*countDown = 11;

	Event* e2 = EventManager->AddEvent([=]
	{
		(*countDown)--;
		text3->Text = std::to_wstring(*countDown);		
		if (*countDown == 0)
			delete countDown;
		return 1;
	}, L"", 0, 11, 1);

	Event* e1 = EventManager->AddEvent([=]
	{
		s1->SetAccelerationY(0.25f);
		return 0;
	}, L"e1", 10, 0, 0);

	Event* e3 = EventManager->AddEvent([=]
	{
		if (Input->IsKeyDown(Keys::Left) || Input->IsKeyDown(Keys::Right))
		{
			try
			{
				EventManager->RemoveEvent(e1);
				EventManager->RemoveEvent(e2);
			}
			catch (std::runtime_error er)
			{
			}
			return 0;
		}
		return 1;
	}, L"e3", 0, 0, 0);
}

void test8()
{
	wstring msg = L"Test8: collision test\n";
	text1->Text = msg;
	CPolygon* p1;
	CPolygon* r;

	CPolygon* player;
	vector<XMFLOAT2> points;
	points.push_back(XMFLOAT2(-0.3, 0));
	points.push_back(XMFLOAT2(0, 0.8));
	points.push_back(XMFLOAT2(0.3, 0));
	points.push_back(XMFLOAT2(-0.3, 0));
	player = DrawManager->AddPoly(points);
	points.clear();
	obj.push_back(player);

	for (int i = 0; i < 20;i++)
		points.push_back(XMFLOAT2(Random::RndDouble() * 20 - 10, Random::RndDouble() * 20 - 10));
	r = DrawManager->AddPoly(points);
	r->color = Color(0, 0, 1, 1);
	obj.push_back(r);

	for (int i = 0; i < 30; i++)
	{
		points.clear();
		points.push_back(XMFLOAT2(-Random::RndDouble()*3-1, 0));
		points.push_back(XMFLOAT2(0, Random::RndDouble()*3+1));
		points.push_back(XMFLOAT2(-Random::RndDouble()*3+1, 0));
		points.push_back(*points.begin());
		p1 = DrawManager->AddPoly(points);
		p1->SetRotationZ(Random::RndDouble()*3.1415);
		p1->SetPosition(Random::RndDouble() * 20 - 10, Random::RndDouble() * 20 - 10, 0);
		obj.push_back(p1);
	}

	Event* e1 = EventManager->AddEvent([=]
	{
		player->SetVelocity(0, 0, 0);
		player->SetAngularVelZ(0);
		if (Input->IsKeyDown(Keys::KeyW))
		{
			player->SetVelocityX(-sinf(player->GetRotation().z)*3);
			player->SetVelocityY(cosf(player->GetRotation().z)*3);
		}
		if (Input->IsKeyDown(Keys::KeyA))
		{
			player->SetAngularVelZ(3);
		}
		else if (Input->IsKeyDown(Keys::KeyD))
		{
			player->SetAngularVelZ(-3);
		}

		for (int i = 2; i < obj.size(); i++)
		{
			obj[i]->color = Color(0, 0, 0, 1);
			if (Collision::IsCollidingSat(player, (CPolygon*)obj[i]))
				obj[i]->color = Color(1, 0, 0, 1);
		}
		return 1;
	}, L"e1", 0, 0, 0);

	events.push_back(e1);
}

void test7()
{
	wstring msg = L"Test7: cursor picking, click and drag bricks\n";
	text1->Text = msg;
	CSprite* s1;
	static CDynamic* pick = nullptr;

	for (int i = 0; i < 20; i++)
	{
		s1 = DrawManager->AddSprite(L"brick.jpg");
		s1->UV = Rect(0, 0, 0.5f, 0.5f);
		s1->SetPixelScale(Size(100, 70));
		s1->SetRotationZ(Random::RndDouble()*3.1415);
		s1->SetPosition(Random::RndDouble() * 20 - 10, Random::RndDouble() * 20 - 10, 0);
		s1->Pickable = true;

		obj.push_back(s1);
	}

	Event* e1 = EventManager->AddEvent([]
	{
		if (!Input->IsKeyDown(Keys::MouseLeft))
		{
			if (pick != nullptr)
			{
				pick->color = Color(1, 1, 1, 1);
				pick->SetPositionZ(0);
			}
			pick = nullptr;

			for (CDynamic* s : obj)
			{
				if (s->IsUnderCursor())
				{
					pick = s;
					break;
				}
			}

			if (pick != nullptr)
				pick->color = Color(0.5, 0.5, 1, 1);
		}
		else
		{
			if (pick != nullptr)
			{
				pick->SetPositionZ(-0.01f);
				auto p = pick->GetPosition();
				auto u = Camera->GetUnitsPerPixel(0);
				pick->SetPositionX(p.x + Input->GetCursorDelta().x*u.x);
				pick->SetPositionY(p.y - Input->GetCursorDelta().y*u.y);
			}
		}
		return 1;
	}, L"e1", 0, 0, 0);

	events.push_back(e1);
}

void test6()
{
	wstring msg = L"Test6: position, rotation, scale, origin\n";
	msg += L"left leaf, spin ccw\n";
	msg += L"moddle leaf, alternate spinning\n";
	msg += L"left brick, spin cw around its corner\n";
	msg += L"middle brick, grow/shrink and stretch on x axis\n";
	msg += L"right brick, bubble up/ fall down";
	text1->Text = msg;
	
	CSprite* s1 = DrawManager->AddSprite(L"leaf.png");
	s1->SetNaturalScale();
	s1->size = 0.3f;
	s1->SetPositionX(-15);
	s1->SetPositionY(4);
	s1->SetAngularVelZ(2);

	CSprite* s2 = DrawManager->AddSprite(L"brick.jpg");
	s2->SetNaturalScale();
	s2->size = 0.3f;
	s2->SetPositionX(-11);
	s2->SetPositionY(4);
	s2->SetAngularVelZ(-3);
	s2->Origin = XMFLOAT2(-1, 1);

	CSprite* s3 = DrawManager->AddSprite(L"brick.jpg");
	s3->SetNaturalScale();
	s3->size = 0.1f;
	s3->SetPositionX(0);
	s3->SetPositionY(4);
	s3->SizeAcceleration = 10;

	CSprite* s4 = DrawManager->AddSprite(L"brick.jpg");
	s4->SetNaturalScale();
	s4->size = 0.1f;
	s4->SetPositionX(10);
	s4->SetPositionY(4);
	s4->SetAccelerationY(10);

	CSprite* s5 = DrawManager->AddSprite(L"leaf.png");
	s5->SetNaturalScale();
	s5->size = 0.3f;
	s5->SetPositionX(0);
	s5->SetPositionY(-4);
	s5->SetAngularAccZ(1);

	Event* e1 = EventManager->AddEvent([=]
	{
		if (s3->SizeVelocity > 0.4f)
		{
			s3->SizeAcceleration = -1;
		}
		else if (s3->SizeVelocity < -0.4f)
		{
			s3->SizeAcceleration = 1;
		}
		s3->Scale.x = (sinf((float)Core->GetGameTime()*10) + 1.5f) * 11;

		if (s4->GetPosition().y > 10)
		{
			s4->SetPositionY(10);
			s4->SetVelocityY(0);
			s4->SetAccelerationY(-10);
		}
		else if (s4->GetPosition().y < -10)
		{
			s4->SetPositionY(-10);
			s4->SetVelocityY(0);
			s4->SetAccelerationY(10);
		}

		if (s5->GetAngularVel().z > 10)
		{
			s5->SetAngularAccZ(-1);
		}
		if (s5->GetAngularVel().z < -10)
		{
			s5->SetAngularAccZ(1);
		}

		return 1;
	}, L"e1", 0, 0, 0);

	obj.push_back(s1);
	obj.push_back(s2);
	obj.push_back(s3);
	obj.push_back(s4);
	obj.push_back(s5);

	events.push_back(e1);
}

void test5()
{
	wstring msg = L"Test5: cached vs uncached textures (size 128x128)\n";
	vector<Pixel> v;
	for (int i = 0; i < 128*128; i++)
		v.push_back(Pixel(255, 0, 0, 255));

	LARGE_INTEGER time1;
	LARGE_INTEGER time2;
	QueryPerformanceCounter(&time1);
	CTexture* t1;
	CSprite* s1;

	for (int i = 0; i < 1000; i++)
	{
		t1 = new CTexture(v.data(), Size(128, 128)); // this will leak memory but that's fine
		s1 = new CSprite(t1);
		DrawManager->AddSprite(s1);
		obj.push_back(s1);
	}

	QueryPerformanceCounter(&time2);
	msg += L"Creating 1000 textures and sprites for that textures ";
	msg += std::to_wstring(time2.QuadPart - time1.QuadPart);
	msg += L" ticks\n";

	QueryPerformanceCounter(&time1);
	t1 = new CTexture(v.data(), Size(128, 128),L"5x5");
	if(!ResourceManager->PeekResource(L"5x5",nullptr))
		ResourceManager->AddResource(t1);

	for (int i = 0; i < 1000; i++)
	{
		s1 = new CSprite((CTexture*)ResourceManager->GetResource(L"5x5"));
		DrawManager->AddSprite(s1);
		obj.push_back(s1);
	}

	QueryPerformanceCounter(&time2);
	msg += L"Creating 1000 sprites using cached texture ";
	msg += std::to_wstring(time2.QuadPart - time1.QuadPart);
	msg += L" ticks";

	text1->Text = msg;
}

void test4()
{
	wstring msg = L"Test4: animation\n";
	msg += L"On the left, cat running to the right\n";
	msg += L"On the right, cat running slower backwards and flipped hori.\n";
	text1->Text = msg;

	CAnimation* s1 = DrawManager->AddAnimation(L"ani.png", 2, 4);
	s1->SetPixelScale(Size(512, 256));
	s1->size = 0.5f;
	s1->Speed = 20;
	s1->SetPositionX(-10);

	CAnimation* s2 = DrawManager->AddAnimation(L"ani.png", 2, 4);
	s2->SetPixelScale(Size(512, 256));
	s2->size = 0.5f;
	s2->Speed = -7;
	s2->FlipHorizontally = true;
	s2->SetPositionX(10);

	obj.push_back(s1);
	obj.push_back(s2);
}

void test3()
{
	wstring msg = L"Test3: texture and sprites\n";
	msg += L"On the left, brick 200x200 in pixels (take a screenshot and check)\n";
	msg += L"On the right, red-green checkerboard 200x200\n";
	msg += L"In the middle, leaf (should be proportional)";
	text1->Text = msg;

	CSprite* s1 = DrawManager->AddSprite(L"brick.jpg");
	s1->SetPixelScale(Size(200, 200));
	s1->SetPositionX(-10);
	obj.push_back(s1);

	vector<Pixel> pixels;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			pixels.push_back(Pixel(255 - 255 * ((i+j) % 2), 255 * ((i + j) % 2), 0, 255));
	CTexture* t1 = new CTexture(pixels.data(), Size(8, 8)); // memory leak

	CSprite* s2 = new CSprite(t1);
	DrawManager->AddSprite(s2);
	s2->SetPositionX(10);
	s2->SetPixelScale(Size(200, 200));
	obj.push_back(s2);

	CSprite* s3 = DrawManager->AddSprite(L"leaf.png");
	s3->SetNaturalScale();
	s3->size = 0.5f;
	obj.push_back(s3);
}

void test2()
{
	wstring msg = L"Test2: polygons and colors\n";
	msg += L"Red star on the left, blue rectangle in the middle, \nrainbow circle on the right";
	text1->Text = msg;

	vector<XMFLOAT2> v;
	for (int i = 0; i < 10; i+=2)
	{
		v.push_back(XMFLOAT2(sinf(0.31415f*i*2), cosf(0.31415f*i*2)));
		v.push_back(XMFLOAT2(sinf(0.31415f*(i+1)*2)*2, cosf(0.31415f*(i+1)*2)*2));
	}
	v.push_back(*v.begin());
	CPolygon* p1 = DrawManager->AddPoly(v);
	p1->SetPositionX(-10);
	p1->color = Color(1, 0, 0, 1);

	CRectangle* r1 = DrawManager->AddRect(Size(10, 3));
	r1->color = Color(0, 0, 1, 1);

	CCircle* c1 = DrawManager->AddCircle(2, 20);
	c1->SetPositionX(10);

	Event* e1 = EventManager->AddEvent([c1]
	{
		c1->color = Color((float)sin(Core->GetGameTime()), (float)cos(Core->GetGameTime()*0.6), (float)sin(Core->GetGameTime() *0.5),1);
		return 1;
	}, L"rainbow", 0, 0, 0);

	obj.push_back(p1);
	obj.push_back(r1);
	obj.push_back(c1);

	events.push_back(e1);
}

void test1()
{
	wstring msg = L"Test1: primer, event manager, basic input\n";
	msg += L"If you see this text it means that primer, bitmap font, \nbitmap text some texture loading works.\n";
	msg += L"Press left/right to navigate. If test2 will load, it means that some input \nand basics of event manager works.";
	text1->Text = msg;
}

void startTest(int i)
{
	tests[i]();

	EventManager->AddEvent([=]
	{
		if (Input->IsKeyPressed(Keys::Right))
		{
			if (i == tests.size() - 1)
				return 1;

			for (auto& e : events)
				EventManager->RemoveEvent(e);
			for (auto& d : obj)
				d->Destroy();
			events.clear();
			obj.clear();
			startTest(i+1);
			return 0;
		}
		else if (Input->IsKeyPressed(Keys::Left))
		{
			if (i == 0)
				return 1;

			for (auto& e : events)
				EventManager->RemoveEvent(e);
			for (auto& d : obj)
				d->Destroy();
			events.clear();
			obj.clear();
			startTest(i - 1);
			return 0;
		}
		return 1;
	}, L"test1", 0, 0, 0);
}

int wrapper()
{
	Functions::InitViva(Size(800, 600), Activity);
	Core->SetBackgroundColor(Color(1, 1, 1, 1));
	Core->SetWindowTitle(L"Test");

	courier = new BitmapFont(L"courier.png", Size(10,19), 19);
	text1 = DrawManager->AddBitmapText(courier);
	text1->SetPosition(XMFLOAT3(-18, 13, 0));
	text1->color = Color(0, 0, 0, 1);
	text1->SetPixelScale(10, 19);
	text1->size = 2;

	tests.push_back(test1);
	tests.push_back(test2);
	tests.push_back(test3);
	tests.push_back(test4);
	tests.push_back(test5);
	tests.push_back(test6);
	tests.push_back(test7);
	tests.push_back(test8);
	tests.push_back(test9);
	tests.push_back(test10);

	MessageBox(0, L"1st test should display text on screen", L"Test1", 0);
	startTest(0);

	Core->Run();
	Core->Destroy();
	return 0;
}

int main(int argc, char** argv)
{
	try
	{
		return wrapper();
	}
	catch (std::runtime_error ex)
	{
		MessageBoxA(0, ex.what(), "Error", MB_ICONEXCLAMATION);
		return 0;
	}
}