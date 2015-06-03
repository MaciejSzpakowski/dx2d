#include "Private.h"
using DirectX::XMFLOAT2;

int main()
{
	dx2d::Core* engine = new dx2d::Core(800, 600);
	engine->SetWindowTitle("Hello");

	XMFLOAT2 path[] = { { 0.0f, 0.0f }, { 0.0f, 0.1f }, { 0.1f, 0.1f }, { 0.1f, 0.0f }, {0,0} };
	DrawManager->AddRect(1, 1);

	engine->Run();
	engine->Destroy();
	return 0;
}