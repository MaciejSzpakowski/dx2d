#include "Viva.h"

namespace Viva
{
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_SYSKEYDOWN:
        {
            if (wParam == VK_MENU)//ignore left alt stop
            {}
            else
                return DefWindowProc(hwnd, msg, wParam, lParam); // this makes ALT+F4 work
            break;
        }
		case WM_CLOSE:
        {
            ShowWindow(hwnd, false);
            PostQuitMessage(0);
            break;
        }
		case WM_COMMAND:
			break;
		case WM_MOUSEWHEEL:
        {
            InputManager->_SetMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
            break;
        }
        case WM_INPUT:
        {
            UINT dwSize = 48; // 48 for 64bit build
            static BYTE lpb[48];

            GetRawInputData((HRAWINPUT)lParam, RID_INPUT,
                lpb, &dwSize, sizeof(RAWINPUTHEADER)); // this gets relative coords

            RAWINPUT* raw = (RAWINPUT*)lpb;

            if (raw->header.dwType == RIM_TYPEMOUSE)
            {
                int xPosRelative = raw->data.mouse.lLastX;
                int yPosRelative = raw->data.mouse.lLastY;
                InputManager->_SetCursorDelta(xPosRelative, yPosRelative);
            }
            break;
        }
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		return 0;
	}

	Window::Window(const Size& clientSize, int style, const std::function<void()>& _worker)
	{
		worker = _worker;

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

		handle = CreateWindowEx(0, className, L"", style, CW_USEDEFAULT, CW_USEDEFAULT, 
			rect.right - rect.left, rect.bottom - rect.top,
			NULL, NULL, GetModuleHandle(0), NULL);

        USHORT HID_USAGE_PAGE_GENERIC = 1;
        USHORT HID_USAGE_GENERIC_MOUSE = 2;

        Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
        Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
        Rid[0].dwFlags = RIDEV_INPUTSINK;
        Rid[0].hwndTarget = handle;
        RegisterRawInputDevices(Rid, 1, sizeof(RAWINPUTDEVICE));

		if (handle == NULL)
			throw VIVA_ERROR("Window creation failed");
	}

	void Window::Destroy()
	{
		DestroyWindow(handle);
		UnregisterClass(L"myWindowClass", GetModuleHandle(0));
		delete this;
	}

	int Window::_Run(std::function<void()> _activity)
	{
		ShowWindow(handle, SW_SHOW);
		UpdateWindow(handle);

		activity = _activity;

		while (true)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					ZeroMemory(&msg, sizeof(msg));
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
                worker();
                activity();
                // reset cursor delta
                InputManager->_SetCursorDelta(0, 0);
			}
		}

		return (int)msg.wParam;
	}
}
