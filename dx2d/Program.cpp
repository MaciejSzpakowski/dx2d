#include "Viva.h"

using namespace Viva;

BitmapFont* courier;
CBitmapText* text1;
vector<CDynamic*> obj;
vector<Event*> events;

void Activity()
{
	Sleep(10);
}

void startTest3()
{
	wstring msg = L"Test3: texture and sprites\n";
	msg += L"";
	text1->Text = msg;


}

void startTest2()
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

	EventManager->AddEvent([c1]
	{
		c1->color = Color((float)sin(Core->GetGameTime()), (float)cos(Core->GetGameTime()*0.6), (float)sin(Core->GetGameTime() *0.5),1);
		return 1;
	}, L"rainbow", 0, 0, 0);

	EventManager->AddEvent([=]
	{
		if (Input->IsKeyPressed(Keys::Enter))
		{
			startTest3();
			EventManager->RemoveEvent(L"rainbow");
			c1->Destroy();
			r1->Destroy();
			p1->Destroy();
			return 0;
		}
		return 1;
	}, L"test2", 0, 0, 0);
}

void startTest1()
{	
	wstring msg = L"Test1: primer, event manager, basic input\n";
	msg += L"If you see this text it means that primer, bitmap font, \nbitmap text some texture loading works.\n";
	msg += L"Press Enter. If test2 will load, it means that some input \nand basics of event manager works.";
	text1->Text = msg;

	EventManager->AddEvent([]
	{
		if (Input->IsKeyPressed(Keys::Enter))
		{
			startTest2();
			return 0;
		}
		return 1;
	}, L"test1", 0, 0, 0);
}

int wrapper()
{
	Functions::InitViva(Size(800, 600), Activity);
	Core->SetBackgroundColor(Color(1, 1, 1, 1));

	courier = new BitmapFont(L"courier.png", Size(10,19), 19);
	text1 = DrawManager->AddBitmapText(courier);
	text1->SetPosition(XMFLOAT3(-18, 13, 0));
	text1->color = Color(0, 0, 0, 1);
	text1->SetPixelScale(10, 19);
	text1->Size = 2;

	MessageBox(0, L"1st test should display text on screen", L"Test1", 0);
	startTest1();

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