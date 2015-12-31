#include "Viva.h"

namespace Viva
{
	void IntActivity()
	{
		InputManager->_Activity();
		EventManager->_Activity();
		Core->GetCamera()->_CamTransform();
		DrawManager->_DrawAll();
		Core->_UpdateGameTime();
	}

	namespace Functions
	{
		void InitViva(Size clientSize, std::function<void()> worker, int style)
		{
			// for now core assigns itself to global variable
			new CCore(clientSize, worker, style);
		}

		void Checkhr(HRESULT hr, int line)
		{
			if (hr == 0)
				return;
			char str[128];
			FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0,
				hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
				str, 128, 0);

			std::string s(str);
			s += "\nLine:";
			s += std::to_string(line);
			throw VIVA_ERROR(s.c_str());
		}
	}

	namespace Random
	{
		double RndDouble()
		{
			static std::random_device rd;
			static std::mt19937 gen(rd());
			static std::uniform_real_distribution<> dis(0, 1);
			return dis(gen);
		}

		int RndInt(int min, int max)
		{
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::uniform_int_distribution<> dis(min, max);
			return dis(gen);
		}
	}

	
}
