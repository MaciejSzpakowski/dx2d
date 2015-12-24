#include "Private.h"
#include <ctime>

using namespace dx2d;


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