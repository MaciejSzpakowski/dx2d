#pragma once

namespace Viva
{
	class CCore
	{
	private:
		Camera* camera;
		Window* window;
		ID3D11BlendState* blendState;
		IDXGISwapChain* swapChain;
		ID3D11RenderTargetView* backBuffer;
		ID3D11Device* device;
		ID3D11DeviceContext* context;
		ID3D11VertexShader* defaultVS;
		ID3D11PixelShader* defaultPS;
		ID3D11PixelShader* defaultPost;
		ID3D11InputLayout* layout; //vertex input layout pos:float[3] col:float[3] uv:float[2]
		ID3D11DepthStencilView* depthStencilView;
		ID3D11Texture2D* depthStencilBuffer;
		float backBufferColor[4];
		double frequency;
		long long startTime;
		long long prevFrameTime;
		double gameTime;
		double frameTime;
		bool fullscreen;
		Size clientSize;
		bool alphaEnabled;

	public:

		CCore(Size _clientSize, std::function<void()> worker, int style);
		HWND GetWindowHandle() const { return window->GetHandle(); }
		void SetWindowTitle(LPCWSTR title) { SetWindowText(window->GetHandle(), title); }
		void SetBackgroundColor(const Color& color);
		int Run() { return window->_Run(); }
		void OpenConsole();
		void CloseConsole();
		Size GetClientSize() const { return clientSize; }

		// Time elapsed in seconds since the beginning of the game.
		double GetGameTime() const { return gameTime; }

		// Time elapsed since last frame.
		double GetFrameTime() const { return frameTime; }

		// Frames per second.
		double GetFps() const { return 1 / frameTime; }

		void SetFullscreen(bool state);
		bool IsFullscreen() const { return fullscreen; }
		void SaveScreenshot(LPCWSTR file);
		Camera* GetCamera() const { return camera; }
		void EnableTransparency(bool flag) { alphaEnabled = flag; }
		bool IsAlphaEnabled() const { return alphaEnabled; }
		void Exit();
		void Destroy();

		void _UpdateGameTime();
		const float* _GetBackBufferColor() const { return backBufferColor; }
		IDXGISwapChain* _GetSwapChain() const { return swapChain; }
		ID3D11DeviceContext* _GetContext() const { return context; }
		ID3D11Device* _GetDevice() const { return device; }
		ID3D11PixelShader* _GetDefaultPS() const { return defaultPS; }
		ID3D11PixelShader* _GetDefaultPost() const { return defaultPost; }
		ID3D11VertexShader* _GetDefaultVS() const { return defaultVS; }
		ID3D11BlendState* _GetBlendState() const { return blendState; }
		ID3D11DepthStencilView* _GetDepthStencilView() const { return depthStencilView; }
		ID3D11RenderTargetView* _GetBackBuffer() const { return backBuffer; }
		ID3D11RenderTargetView *const* _GetBackBufferAddress() const { return &backBuffer; }

		// CREATOR

		// create pixel shader from file
		ID3D11PixelShader* CreatePixelShaderFromFile(const wchar_t* filepath, const char* entryPoint, const char* = "ps_5_0");

		// create pixel shader from const char*
		ID3D11PixelShader* CreatePixelShaderFromString(const char* str, const char* entryPoint, const char* = "ps_5_0");
	};
}