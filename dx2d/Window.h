#pragma once

namespace Viva
{
	class Window
	{
	private:
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
