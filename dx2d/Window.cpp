#include "Viva.h"

namespace Viva
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
			InputManager->_SetMouseWheel (GET_WHEEL_DELTA_WPARAM(wParam));
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		return 0;
	}

	CWindow::CWindow(Size clientSize,int style)
	{
		const wchar_t className[] = L"myWindowClass";
		WNDCLASSEX wc;		
		ZeroMemory(&zMsg, sizeof(zMsg));
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
			MessageBox(NULL, L"Window Registration Failed!", L"Error!",
				MB_ICONEXCLAMATION | MB_OK);
			exit(0);
		}

		RECT rect = { 0, 0, (LONG)clientSize.width, (LONG)clientSize.height };
		AdjustWindowRectEx(&rect, style | WS_CLIPSIBLINGS,
			FALSE, 0);

		zHandle = CreateWindowEx(0,
			className,	L"",	style,
			CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
			NULL, NULL, GetModuleHandle(0), NULL);

		if (zHandle == NULL)
		{
			MessageBox(NULL, L"Window Creation Failed!", L"Error!",
				MB_ICONEXCLAMATION | MB_OK);
			exit(0);
		}
	}

	int CWindow::zRun()
	{
		ShowWindow(zHandle, SW_SHOW);
		UpdateWindow(zHandle);
		while (WM_QUIT != zMsg.message)
		{
			if (PeekMessage(&zMsg, NULL, 0, 0, PM_REMOVE))
			{
					TranslateMessage(&zMsg);
					DispatchMessage(&zMsg);
			}
			else
			{
				zWorker();
				zActivity();
			}
		}
		UnregisterClass(L"myWindowClass", GetModuleHandle(0));
		return (int)zMsg.wParam;
	}

	void CWindow::Destroy()
	{		
		delete this;
	}
}
