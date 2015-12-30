#pragma once

namespace Viva
{
	class CDebugManager
	{
	private:
		BitmapText* debugText;
		std::wstringstream wss;
	public:

		CDebugManager(BitmapFont* font, Camera* camera);
		void Debug(int debug, wstring name);
		void Debug(float debug, wstring name);
		void Debug(double debug, wstring name);
		void Debug(wstring debug, wstring name);

		BitmapText* _GetText() const { return debugText; }

		// Send wss to debugText and clear wss.
		void _Flush();

		void Destroy();
	};
}
