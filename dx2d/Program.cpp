#include "Private.h"

using namespace dx2d;

Circle* c1;
Circle* c2;
dx2d::Rectangle* r1;

void Activity()
{
}

int main()
{
	Core* engine = NewCore(800, 600, Activity);
	engine->SetWindowTitle("Hello");

	c1 = DrawManager->AddCircle(1, 10);
	c2 = DrawManager->AddCircle(2, 20);
	c2->Position = XMFLOAT3(0, 5, 0);
	c2->Acceleration = XMFLOAT3(0, -0.0000001f, 0);
	r1 = DrawManager->AddRect(2, 4);
	r1->Position = XMFLOAT3(-3, 2, 0);
	r1->Spin = XMFLOAT3(0, 0, 0.0003f);

	engine->Run();
	engine->Destroy();
	return 0;
}