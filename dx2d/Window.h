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
		Window(const Size& clientSize, int style, const std::function<void()>& _worker, 
			const std::function<void()>& _activity);
		int _Run();
		HWND GetHandle() const { return handle; }
		void Destroy() { delete this; }
	};
}
