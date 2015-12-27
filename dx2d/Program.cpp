#include "Viva.h"

using namespace Viva;


void Activity()
{
	Sleep(10);

}

int main(int argc, char** argv)
{
	Functions::InitCore(Size(800,600), Activity);

	Core->SetBackgroundColor(Color(1,1,1,1));
	CCircle* c1 = DrawManager->AddCircle(2, 10);
	c1->SetAngularVelZ(1);

	CTexture* t1 = Core->CreateTexture(L"brick.jpg");
	CSprite* s1 = new CSprite(t1);
	DrawManager->AddSprite(s1);
	s1->SetNaturalScale();
	s1->Size = 0.5f;
	s1->SetPositionX(4);


	Core->Run();
	Core->Destroy();
	return 0;
}