#include "Private.h"

int main()
{
	dx2d::Core* engine = new dx2d::Core(800, 600);
	engine->SetWindowTitle("Hello");

	XMFLOAT2 triangle[] = { { 0.0f, -1}, { 0.0f, 1}, { 0.5, 0 } };
	DrawManager->AddPoly(triangle, 3);

	engine->Run();
	engine->Destroy();
	return 0;
}