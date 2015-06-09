#include "Private.h"

namespace dx2d
{
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_COMMAND:
			break;
		case WM_MOUSEWHEEL:
			Input->MouseWheel = GET_WHEEL_DELTA_WPARAM(wParam);
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		return 0;
	}

	Window::Window(int sizex, int sizey)
	{
		const char className[] = "myWindowClass";
		WNDCLASSEX wc;		
		ZeroMemory(&Msg, sizeof(Msg));
		ZeroMemory(&wc, sizeof(wc));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.lpfnWndProc = WndProc;
		wc.hInstance = GetModuleHandle(0);
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
		wc.lpszClassName = className;
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		if (!RegisterClassEx(&wc))
		{
			MessageBox(NULL, "Window Registration Failed!", "Error!",
				MB_ICONEXCLAMATION | MB_OK);
			exit(0);
		}

		handle = CreateWindowEx(WS_EX_CONTROLPARENT | WS_EX_CLIENTEDGE,
			className,
			"",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, sizex, sizey,
			NULL, NULL, GetModuleHandle(0), NULL);

		if (handle == NULL)
		{
			MessageBox(NULL, "Window Creation Failed!", "Error!",
				MB_ICONEXCLAMATION | MB_OK);
			exit(0);
		}
	}

	int Window::Run()
	{
		ShowWindow(handle, SW_SHOW);
		UpdateWindow(handle);
		while (WM_QUIT != Msg.message)
		{
			if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
			{
					TranslateMessage(&Msg);
					DispatchMessage(&Msg);
			}
			else
			{
				Worker();
				Render(Core);
			}
		}
		return (int)Msg.wParam;
	}
}
