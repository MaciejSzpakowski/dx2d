#pragma once

namespace Viva
{
	class CDebugManager
	{
	public:
		BitmapText* zDebugText;
		std::wstringstream zWss;

		CDebugManager();
		void Init(BitmapFont* font);
		void Debug(int debug, wstring name);
		void Debug(float debug, wstring name);
		void Debug(double debug, wstring name);
		void Debug(wstring debug, wstring name);
		//send wss to debugText and clear wss
		void Flush();
		void Destroy();
	};
}
