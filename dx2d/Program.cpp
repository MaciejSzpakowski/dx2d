#include "Viva.h"

using namespace Viva;

void Activity()
{
	Sleep(10);
}

int wrapper()
{
	Functions::InitViva(Size(800, 600), Activity);

	Core->SetBackgroundColor(Color(1, 1, 1, 1));
	CCircle* c1 = DrawManager->AddCircle(2, 10);
	c1->SetAngularVelY(1);

	CTexture* t1 = new CTexture(L"brick.jpg");
	CSprite* s1 = new CSprite(t1);
	DrawManager->AddSprite(s1);
	s1->SetNaturalScale();
	s1->Size = 0.5f;
	s1->SetPositionZ(4);

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
	}
}