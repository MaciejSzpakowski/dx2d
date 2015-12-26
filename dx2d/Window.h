#pragma once

namespace Viva
{
	class CWindow
	{
	public:
		HWND zHandle;
		MSG zMsg;
		std::function<void()> zWorker;
		std::function<void()> zActivity;
		CWindow(int sizex, int sizey, int style);
		int zRun();
		void Destroy();
	};
}