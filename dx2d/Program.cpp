#include "Viva.h"

using namespace Viva;

BitmapText* text1;
vector<std::function<void()>> tests;
vector<Dynamic*> obj;
vector<Event*> events;

void Activity()
{
	Sleep(1);
}

void test12()
{
	text1->SetText(L"");

	BitmapText* n = DrawManager->AddBitmapText(DrawManager->GetDefaultFont());
	n->SetPosition(XMFLOAT3(-10, 10, 0));
	n->SetColor(Color(0, 0, 0, 1));
	obj.push_back(n);

	DebugManager->SetColor(Color(0, 0, 0, 1));

	static wstring nt = L"";

	Event* e1 = EventManager->AddEvent([=]
	{
		DebugManager->Debug(L"This text is displayed using debug manager, type something", L"Hello");
		DebugManager->Debug(Core->GetCamera()->GetCursorWorldPos(0).x, L"Cursor world x");
		DebugManager->Debug(Core->GetCamera()->GetCursorWorldPos(0).y, L"Cursor world y");

		int c = InputManager->GetChar();

		if (InputManager->IsKeyPressed(Keys::BackSpace) && nt.length() > 0)
			nt.pop_back();
		else if(c != 0)
			nt += c;


		n->SetText(nt);
		return 1;
	}, L"typing", 0, 0, 0);

	events.push_back(e1);
}

void test11()
{
	wstring msg = L"Test11: Memory test\n"
	"Open task manager and check if memory is leaking\n";
	text1->SetText(msg);
}

void test10()
{
	wstring msg = L"Test10: Render targets and pixel shaders\n"
	"This text is in the top render target not affected by pixel shader\n";
	text1->SetText(msg);

	DebugManager->SetColor(Color(1, 0, 0, 1));

	static float time1 = (float)Core->GetGameTime();
	Event* e1 = EventManager->AddEvent([=]
	{
		time1 = (float)Core->GetGameTime();
		DebugManager->Debug(L"Test string", L"Test");
		return 1;
	}, L"e1", 0, 0, 0);

	events.push_back(e1);

	RenderTarget* r1 = DrawManager->AddRenderTarget();
	r1->MoveToBottom();
	Sprite* s1 = DrawManager->AddSprite(L"brick.jpg",r1);
	s1->SetPositionZ(0.1f);
	s1->SetPixelPerfectScale();
	s1->SetSize(1.2f);

	Sprite* s2 = DrawManager->AddSprite(L"leaf.png", r1);
	s2->SetPositionX(-10);
	s2->SetAngularVelocityZ(1);
	s2->SetPixelPerfectScale();
	s2->SetSize(0.5f);

	Sprite* s3 = DrawManager->AddSprite(L"leaf.png", r1);
	s3->SetPositionX(10);
	s3->SetAngularVelocityZ(-1);
	s3->SetPixelPerfectScale();
	s3->SetSize(0.5f);

	Pixel pixels[] = { Pixel(255,255,255,255), };
	Texture* w1 = new Texture(pixels, Size(1, 1),L"whiteBackground");
	Sprite* s4 = new Sprite(w1);
	s4->SetName(L"back");
	s4->SetScaleX(19);
	s4->SetPositionY(12.5f);
	s4->SetPositionZ(0.1f);
	DrawManager->AddSprite(s4);

	obj.push_back(s4);

	s1->SetExtraBufferPS(&time1);

	const char ps2[] = R"(
			cbuffer cbBufferPS
			{
				float4 color;
			};

			cbuffer cbExtra
			{
				float var[4];
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
				float2 coord = input.TexCoord;
				coord[0] += sin(var[0]+coord[1]*2) * 0.4;
				coord[1] += sin(var[0]+coord[0]*2) * 0.4;
				float4 result = ObjTexture.Sample(ObjSamplerState, coord);
	
				clip(result.a - 0.001f);
				return result * color;
			}
		)";

	ID3D11PixelShader* ps = Core->CreatePixelShaderFromString(ps2, "main");
	r1->SetPixelShader(ps);
	
	EventManager->AddEvent([=]
	{
		if (InputManager->IsKeyPressed(Keys::Left) || InputManager->IsKeyPressed(Keys::Right))
		{
			w1->Destroy();
			r1->Destroy();
			return 0;
		}
		return 1;
	}, L"e3", 0, 0, 0);
}

void test9()
{
	wstring msg = L"Test9: events, T=-10s and then lift off!\n";
	text1->SetText(msg);

	vector<Pixel> pixels;
	pixels.push_back(Pixel(200, 200, 0, 255));

	Texture* t1 = new Texture(pixels.data(), Size(1, 1));
	Sprite* s1 = new Sprite(t1);
	DrawManager->AddSprite(s1);
	s1->SetPositionX(-10);
	s1->SetPositionY(-6);

	obj.push_back(s1);

	BitmapText* text3 = DrawManager->AddBitmapText(DrawManager->GetDefaultFont());
	text3->SetColor(Color(0, 0, 0, 1));
	text3->SetPositionX(-13);
	text3->SetSize(3);

	obj.push_back(text3);

	int* countDown = new int;
	*countDown = 11;

	Event* e2 = EventManager->AddEvent([=]
	{
		(*countDown)--;
		text3->SetText(std::to_wstring(*countDown));		
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
		if (InputManager->IsKeyPressed(Keys::Left) || InputManager->IsKeyPressed(Keys::Right))
		{
			try
			{
				EventManager->RemoveEvent(e1);
				EventManager->RemoveEvent(e2);
			}
			catch (VivaError er)
			{
			}
			return 0;
		}
		return 1;
	}, L"e3", 0, 0, 0);
}

void test8()
{
	wstring msg = L"Test8: collision test. Press W to move forward and A,D to turn\n"
	"When triangle is colliding with other traingles, they change color to red\n"
	"When triangle is colliding with blue circle, pink arrow points to the first\n"
	"collision point detected";
	text1->SetText(msg);
	Viva::Polygon* p1;
	Viva::Circle* r;
	Viva::Polygon* arrow;

	vector<XMFLOAT2> points;
	points.push_back(XMFLOAT2(0, -1));
	points.push_back(XMFLOAT2(0, 0));
	points.push_back(XMFLOAT2(1, 0));
	points.push_back(XMFLOAT2(0, 0));
	points.push_back(XMFLOAT2(2, -2));
	arrow = DrawManager->AddPoly(points);
	arrow->SetColor(Color(1, 0, 1, 1));
	arrow->SetPosition(XMFLOAT3(100, 100, -0.01f));

	Viva::Polygon* player;
	points.clear();
	points.push_back(XMFLOAT2(-0.3f, 0));
	points.push_back(XMFLOAT2(0, 0.8f));
	points.push_back(XMFLOAT2(0.3f, 0));
	points.push_back(XMFLOAT2(-0.3f, 0));
	player = DrawManager->AddPoly(points);
	player->TransformVertices(true);
	points.clear();
	obj.push_back(player);

	r = DrawManager->AddCircle(3,12);
	r->TransformVertices(true);
	r->SetColor(Color(0, 0, 1, 1));
	obj.push_back(r);

	for (int i = 0; i < 30; i++)
	{
		points.clear();
		points.push_back(XMFLOAT2((float)-Random::RndDouble()*3-1, 0));
		points.push_back(XMFLOAT2(0, (float)Random::RndDouble()*3+1));
		points.push_back(XMFLOAT2((float)-Random::RndDouble()*3+1, 0));
		points.push_back(*points.begin());
		p1 = DrawManager->AddPoly(points);
		p1->TransformVertices(true);
		p1->SetRotationZ((float)Random::RndDouble()*3.1415f);
		p1->SetPosition((float)Random::RndDouble() * 20 - 10, (float)Random::RndDouble() * 20 - 10, 0);
		obj.push_back(p1);
	}

	Event* e1 = EventManager->AddEvent([=]
	{
		XMFLOAT3 col;

		player->SetVelocity(0, 0, 0);
		player->SetAngularVelocityZ(0);
		if (InputManager->IsKeyDown(Keys::KeyW))
		{
			player->SetVelocityX(-sinf(player->GetRotation().z)*3);
			player->SetVelocityY(cosf(player->GetRotation().z)*3);
		}
		if (InputManager->IsKeyDown(Keys::KeyA))
		{
			player->SetAngularVelocityZ(3);
		}
		else if (InputManager->IsKeyDown(Keys::KeyD))
		{
			player->SetAngularVelocityZ(-3);
		}

		if (Collision::IsColliding(player, r, &col))
		{
			arrow->SetPositionX(col.x);
			arrow->SetPositionY(col.y);
		}
		else
			arrow->SetPosition(XMFLOAT3(100, 100, -0.01f));

		for (int i = 2; i < obj.size(); i++)
		{
			obj[i]->SetColor(Color(0, 0, 0, 1));
			if (Collision::IsCollidingSat(player, (Viva::Polygon*)obj[i]))
				obj[i]->SetColor(Color(1, 0, 0, 1));
		}
		return 1;
	}, L"e1", 0, 0, 0);

	events.push_back(e1);
}

void test7()
{
	wstring msg = L"Test7: cursor picking, click and drag bricks\n";
	text1->SetText(msg);
	Sprite* s1;
	static Dynamic* pick = nullptr;

	for (int i = 0; i < 20; i++)
	{
		s1 = DrawManager->AddSprite(L"brick.jpg");
		s1->SetUv(Rect(0, 0, 0.5f, 0.5f));
		s1->SetPixelScale(Size(100, 70));
		s1->SetRotationZ((float)Random::RndDouble()*3.1415f);
		s1->SetPosition((float)Random::RndDouble() * 20 - 10, (float)Random::RndDouble() * 20 - 10, 0);
		s1->SetPickable(true);

		obj.push_back(s1);
	}

	Event* e1 = EventManager->AddEvent([]
	{
		if (!InputManager->IsKeyDown(Keys::MouseLeft))
		{
			if (pick != nullptr)
			{
				pick->SetColor(Color(1, 1, 1, 1));
				pick->SetPositionZ(0);
			}
			pick = nullptr;

			for (Dynamic* s : obj)
			{
				if (s->IsUnderCursor())
				{
					pick = s;
					break;
				}
			}

			if (pick != nullptr)
				pick->SetColor(Color(0.5, 0.5, 1, 1));
		}
		else
		{
			if (pick != nullptr)
			{
				pick->SetPositionZ(-0.01f);
				auto p = pick->GetPosition();
				auto u = Core->GetCamera()->GetUnitsPerPixel(0);
				pick->SetPositionX(p.x + InputManager->GetCursorDelta().x*u.x);
				pick->SetPositionY(p.y - InputManager->GetCursorDelta().y*u.y);
			}
		}
		return 1;
	}, L"e1", 0, 0, 0);

	events.push_back(e1);
}

void test6()
{
	wstring msg = L"Test6: position, rotation, scale, origin\n"
	"left leaf, spin ccw\n"
	"moddle leaf, alternate spinning\n"
	"left brick, spin cw around its corner\n"
	"middle brick, grow/shrink and stretch on x axis\n"
	"right brick, bubble up/ fall down";
	text1->SetText(msg);
	
	Sprite* s1 = DrawManager->AddSprite(L"leaf.png");
	s1->SetPixelPerfectScale();
	s1->SetSize(0.3f);
	s1->SetPositionX(-15);
	s1->SetPositionY(4);
	s1->SetAngularVelocityZ(2);

	Sprite* s2 = DrawManager->AddSprite(L"brick.jpg");
	s2->SetPixelPerfectScale();
	s2->SetSize(0.3f);
	s2->SetPositionX(-11);
	s2->SetPositionY(4);
	s2->SetAngularVelocityZ(-3);
	s2->SetOrigin(XMFLOAT2(-1, 1));

	Sprite* s3 = DrawManager->AddSprite(L"brick.jpg");
	s3->SetPixelPerfectScale();
	s3->SetSize(0.1f);
	s3->SetPositionX(0);
	s3->SetPositionY(4);
	s3->SetSizeAcceleration(10);

	Sprite* s4 = DrawManager->AddSprite(L"brick.jpg");
	s4->SetPixelPerfectScale();
	s4->SetSize(0.1f);
	s4->SetPositionX(10);
	s4->SetPositionY(4);
	s4->SetAccelerationY(10);

	Sprite* s5 = DrawManager->AddSprite(L"leaf.png");
	s5->SetPixelPerfectScale();
	s5->SetSize(0.3f);
	s5->SetPositionX(0);
	s5->SetPositionY(-4);
	s5->SetAngularAccelerationZ(1);

	Event* e1 = EventManager->AddEvent([=]
	{
		if (s3->GetSizeVelocity() > 0.4f)
		{
			s3->SetSizeAcceleration(-1);
		}
		else if (s3->GetSizeVelocity() < -0.4f)
		{
			s3->SetSizeAcceleration(1);
		}
		s3->SetScaleX((sinf((float)Core->GetGameTime()*10) + 1.5f) * 11);

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

		if (s5->GetAngularVelocity().z > 10)
		{
			s5->SetAngularAccelerationZ(-1);
		}
		if (s5->GetAngularVelocity().z < -10)
		{
			s5->SetAngularAccelerationZ(1);
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
	Texture* t1;
	Sprite* s1;

	for (int i = 0; i < 1000; i++)
	{
		t1 = new Texture(v.data(), Size(128, 128)); // this will leak memory but that's fine
		s1 = new Sprite(t1);
		DrawManager->AddSprite(s1);
		obj.push_back(s1);
	}

	QueryPerformanceCounter(&time2);
	msg += L"Creating 1000 textures and sprites for that textures ";
	msg += std::to_wstring(time2.QuadPart - time1.QuadPart);
	msg += L" ticks\n";

	QueryPerformanceCounter(&time1);
	t1 = new Texture(v.data(), Size(128, 128),L"5x5");
	if(!ResourceManager->PeekResource(L"5x5",nullptr))
		ResourceManager->AddResource(t1);

	for (int i = 0; i < 1000; i++)
	{
		s1 = new Sprite((Texture*)ResourceManager->GetResource(L"5x5"));
		DrawManager->AddSprite(s1);
		obj.push_back(s1);
	}

	QueryPerformanceCounter(&time2);
	msg += L"Creating 1000 sprites using cached texture ";
	msg += std::to_wstring(time2.QuadPart - time1.QuadPart);
	msg += L" ticks";

	text1->SetText(msg);
}

void test4()
{
	wstring msg = L"Test4: animation\n"
	"On the left, cat running to the right\n"
	"On the right, cat running slower backwards and flipped hori.\n";
	text1->SetText(msg);

	Animation* s1 = DrawManager->AddAnimation(L"ani.png", 2, 4);
	s1->SetPixelScale(Size(512, 256));
	s1->SetSize(0.5f);
	s1->SetSpeed(20);
	s1->SetPositionX(-10);

	Animation* s2 = DrawManager->AddAnimation(L"ani.png", 2, 4);
	s2->SetPixelScale(Size(512, 256));
	s2->SetSize(0.5f);
	s2->SetSpeed(-7);
	s2->SetFlipHorizontally(true);
	s2->SetPositionX(10);

	obj.push_back(s1);
	obj.push_back(s2);
}

void test3()
{
	wstring msg = L"Test3: texture and sprites\n"
	"On the left, brick 200x200 in pixels (take a screenshot and check)\n"
	"On the right, red-green checkerboard 200x200\n"
	"In the middle, leaf (should be proportional)";
	text1->SetText(msg);

	Sprite* s1 = DrawManager->AddSprite(L"brick.jpg");
	s1->SetPixelScale(Size(200, 200));
	s1->SetPositionX(-10);
	obj.push_back(s1);

	vector<Pixel> pixels;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			pixels.push_back(Pixel(255 - 255 * ((i+j) % 2), 255 * ((i + j) % 2), 0, 255));
	Texture* t1 = new Texture(pixels.data(), Size(8, 8)); // memory leak

	Sprite* s2 = new Sprite(t1);
	DrawManager->AddSprite(s2);
	s2->SetPositionX(10);
	s2->SetPixelScale(Size(200, 200));
	obj.push_back(s2);

	Sprite* s3 = DrawManager->AddSprite(L"leaf.png");
	s3->SetPixelPerfectScale();
	s3->SetSize(0.5f);
	obj.push_back(s3);
}

void test2()
{
	wstring msg = L"Test2: polygons and colors\n"
	"Red star on the left, blue rectangle in the middle, \nrainbow circle on the right";
	text1->SetText(msg);

	vector<XMFLOAT2> v;
	for (int i = 0; i < 10; i+=2)
	{
		v.push_back(XMFLOAT2(sinf(0.31415f*i*2), cosf(0.31415f*i*2)));
		v.push_back(XMFLOAT2(sinf(0.31415f*(i+1)*2)*2, cosf(0.31415f*(i+1)*2)*2));
	}
	v.push_back(*v.begin());
	Viva::Polygon* p1 = DrawManager->AddPoly(v);
	p1->SetPositionX(-10);
	p1->SetColor(Color(1, 0, 0, 1));

	Viva::Rectangle* r1 = DrawManager->AddRect(Size(10, 3));
	r1->SetColor(Color(0, 0, 1, 1));

	Circle* c1 = DrawManager->AddCircle(2, 20);
	c1->SetPositionX(10);

	Event* e1 = EventManager->AddEvent([c1]
	{
		c1->SetColor(Color((float)sin(Core->GetGameTime()), (float)cos(Core->GetGameTime()*0.6), (float)sin(Core->GetGameTime() *0.5),1));
		return 1;
	}, L"rainbow", 0, 0, 0);

	obj.push_back(p1);
	obj.push_back(r1);
	obj.push_back(c1);

	events.push_back(e1);
}

void test1()
{
	wstring msg = L"Test1: primer, event manager, basic input\n"
	"If you see this text it means that primer, bitmap font, \nbitmap text some texture loading works.\n"
	"Press left/right to navigate. If test2 will load, it means that some input \nand basics of event manager works.";
	text1->SetText(msg);
}

void startTest(int i)
{
	tests[i]();

	EventManager->AddEvent([=]
	{
		if (InputManager->IsKeyPressed(Keys::Right))
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
		else if (InputManager->IsKeyPressed(Keys::Left))
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
		else if (InputManager->IsKeyPressed(Keys::Up))
			text1->SetSize(text1->GetSize() + 0.001f);
		else if (InputManager->IsKeyPressed(Keys::Down))
			text1->SetSize(text1->GetSize() - 0.001f);
		return 1;
	}, L"test1", 0, 0, 0);
}

int wrapper()
{
	Functions::InitViva(Size(800, 600));
	Core->SetBackgroundColor(Color(1, 1, 1, 1));
	Core->SetWindowTitle(L"Test");
	Core->OpenConsole();

	text1 = DrawManager->AddBitmapText(DrawManager->GetDefaultFont());
	text1->SetPosition(XMFLOAT3(-18, 13, 0));
	text1->SetColor(Color(0, 0, 0, 1));
		
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
	tests.push_back(test11);
	tests.push_back(test12);

	MessageBox(0, L"TODO:\nSetextrabuffersize instead of createextrabuffer\nGive more meaningful CHECKHR error", L"Test1", 0);
	MessageBox(0, L"1st test should display text on screen", L"Test1", 0);
	startTest(0);

	while (true)
	{
		Core->Run(Activity);
		char input[100];
		printf("Exit ? y/n\n");
		scanf("%99s", input);
		if (memcmp(input, "y",1) == 0)
			break;
	}
	Core->Destroy();
	return 0;
}

int main(int argc, char** argv)
{
	try
	{
		return wrapper();
	}
	catch (VivaError er)
	{
		std::string s = er.function;
		s += "\n";
		s += er.what();
		MessageBoxA(0, s.c_str(), "Error", MB_ICONEXCLAMATION);
		return 0;
	}
}