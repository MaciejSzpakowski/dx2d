#pragma once

namespace Viva
{
	namespace Functions
	{
		//creates new CCore object so you dont have to use new operator
		void InitViva(Size clientSize, std::function<void()> worker, int style = WS_OVERLAPPEDWINDOW);
				
		//check for HR when directx init functions are called
		void Checkhr(HRESULT hr, int line);
	}

	namespace Random
	{
		//return double between 0(inclusive) and 1(exclusive)
		double RndDouble();

		//return integer
		//between min and max (both inclusive)
		int RndInt(int min, int max);
	}

	
}
