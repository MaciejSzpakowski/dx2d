#pragma once

namespace Viva
{
	class Window
	{
	private:
        RAWINPUTDEVICE Rid[1]; // you can have more than one
		HWND handle;
		MSG msg;
		std::function<void()> worker;   // lib side worker
		std::function<void()> activity; // client size activity

	public:
		Window(const Size& clientSize, int style, const std::function<void()>& _worker);
		int _Run(std::function<void()> _activity);
		HWND GetHandle() const { return handle; }
		void Destroy();
	};
}
