#include "Viva.h"

using namespace Viva;


void Activity()
{
	Sleep(10);

}

int main(int argc, char** argv)
{
	Functions::InitCore(800, 600, Activity);

	Core->SetBackgroundColor(Color(0,0,0,0));

	Core->Run();
	Core->Destroy();
	return 0;
}