#pragma once

namespace Viva
{
	class CCore
	{
	public:
		CWindow* zWindow;
		ID3D11BlendState* zBlendState;
		IDXGISwapChain* zSwapChain;
		ID3D11RenderTargetView* zBackBuffer;
		ID3D11Device* zDevice;
		ID3D11DeviceContext* zContext;
		ID3D11VertexShader* zDefaultVS;
		ID3D11PixelShader* zDefaultPS;
		ID3D11PixelShader* zDefaultPost;
		ID3D11InputLayout* zLayout; //vertex input layout pos:float[3] col:float[3] uv:float[2]
		ID3D11DepthStencilView* zDepthStencilView;
		ID3D11Texture2D* zDepthStencilBuffer;
		float zBackBufferColor[4];
		double zFrequency;
		long long zStartTime;
		long long zPrevFrameTime;
		double zGameTime;
		double zFrameTime;
		void zUpdateGameTime();
		bool zFullscreen;
		Size clientSize;
		bool EnableAlpha;

		CCore(Size _clientSize, std::function<void()> worker, int style);
		HWND GetWindowHandle();
		void SetWindowTitle(LPCWSTR title);
		void SetBackgroundColor(Color color);
		int Run();
		void OpenConsole();
		void CloseConsole();
		double GetGameTime();
		//time per frame, to get FPS -> 1 / GetFrameTime()
		double GetFrameTime();
		double GetFps();
		void SetFullscreen(bool state);
		bool GetFullscreen();
		void Destroy();
		void SaveScreenshot(LPCWSTR file);
		void Exit();

		// CREATOR

		// create pixel shader from file
		ID3D11PixelShader* CreatePixelShaderFromFile(const wchar_t* filepath, const char* entryPoint, const char* = "ps_5_0");

		// create pixel shader from const char*
		ID3D11PixelShader* CreatePixelShaderFromString(const char* str, const char* entryPoint, const char* = "ps_5_0");

		// create render target
		CRenderTarget* CreateRenderTarget();
	};
}