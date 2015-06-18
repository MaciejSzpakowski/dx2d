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
			Input->mouseWheel = GET_WHEEL_DELTA_WPARAM(wParam);
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		return 0;
	}

	CWindow::CWindow(int sizex, int sizey,int style)
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

		RECT rect = { 0, 0, sizex, sizey };
		AdjustWindowRectEx(&rect, style | WS_CLIPSIBLINGS,
			FALSE, 0);

		handle = CreateWindowEx(0,
			className,	"",	style,
			CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
			NULL, NULL, GetModuleHandle(0), NULL);

		if (handle == NULL)
		{
			MessageBox(NULL, "Window Creation Failed!", "Error!",
				MB_ICONEXCLAMATION | MB_OK);
			exit(0);
		}
	}

	int CWindow::Run()
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
