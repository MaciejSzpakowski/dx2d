#include "Viva.h"

namespace Viva
{
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_SYSKEYDOWN:
			if (wParam == VK_MENU)// left-alt key
			{
				//ignore left alt stop
			}
			break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_COMMAND:
			break;
		case WM_MOUSEWHEEL:
			InputManager->_SetMouseWheel (GET_WHEEL_DELTA_WPARAM(wParam));
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		return 0;
	}

	Window::Window(const Size& clientSize, int style, const std::function<void()>& _worker,
		const std::function<void()>& _activity)
	{
		worker = _worker;
		activity = _activity;

		const wchar_t className[] = L"myWindowClass";
		WNDCLASSEX wc;		
		ZeroMemory(&msg, sizeof(msg));
		ZeroMemory(&wc, sizeof(wc));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.lpfnWndProc = WndProc;
		wc.hInstance = GetModuleHandle(0);
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
		wc.lpszClassName = className;
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		if (!RegisterClassEx(&wc))
			throw VIVA_ERROR("Window Class failed to register");

		RECT rect = { 0, 0, (LONG)clientSize.width, (LONG)clientSize.height };
		AdjustWindowRectEx(&rect, style | WS_CLIPSIBLINGS,
			FALSE, 0);

		handle = CreateWindowEx(0,
			className,	L"",	style,
			CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
			NULL, NULL, GetModuleHandle(0), NULL);

		if (handle == NULL)
			throw VIVA_ERROR("Window creation failed");
	}

	int Window::_Run()
	{
		ShowWindow(handle, SW_SHOW);
		UpdateWindow(handle);
		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
			}
			else
			{
				worker();
				activity();
			}
		}
		UnregisterClass(L"myWindowClass", GetModuleHandle(0));
		return (int)msg.wParam;
	}
}
