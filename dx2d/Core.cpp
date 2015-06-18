#include "Private.h"
#include <ctime>
#include <d3dcompiler.h>

namespace dx2d
{
	CCore::CCore(int sizex, int sizey, std::function<void()> worker, int style)
	{
		srand((int)time(0));
		hr = 0;
		fullscreen = false;
		clientSize.x = sizex;
		clientSize.y = sizey;
		//assign global variable
		Core = this;
		//create window
		window = new CWindow(sizex, sizey, style);
		window->Worker = worker;
		window->Render = Render;

		backBufferColor[0] = 1.0f;
		backBufferColor[1] = 1.0f;
		backBufferColor[2] = 1.0f;
		backBufferColor[3] = 1.0f;

		//// *********** PIPELINE SETUP STARTS HERE *********** ////
		// create a struct to hold information about the swap chain
		DXGI_SWAP_CHAIN_DESC scd;
		ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
		scd.BufferCount = 1;                                    // one back buffer
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
		scd.OutputWindow = window->handle;                 // the window to be used
		scd.SampleDesc.Quality = 0;
		scd.SampleDesc.Count = 1;                               // no anti aliasing
		scd.Windowed = TRUE;                                    // windowed/full-screen mode
		//scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // alternative fullscreen mode

		////    DEVICE, DEVICE CONTEXT AND SWAP CHAIN    ////
		hr = D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
			D3D11_SDK_VERSION,
			&scd,
			&swapChain,
			&device, NULL,
			&context);
		CHECKHR();

		////    BACK BUFFER AS RENDER TARGET, DEPTH STENCIL   ////
		// get the address of the back buffer
		ID3D11Texture2D* buf;
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&buf);
		// use the back buffer address to create the render target
		hr = device->CreateRenderTargetView(buf, NULL, &backBuffer); CHECKHR();
		buf->Release();

		//Describe our Depth/Stencil Buffer
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = sizex;
		depthStencilDesc.Height = sizey;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;
		//Create the Depth/Stencil View
		hr = device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer); CHECKHR();
		hr = device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView); CHECKHR();

		// set the render target as the back buffer
		context->OMSetRenderTargets(1, &backBuffer, depthStencilView);

		////   VIEWPORT    ////
		// Set the viewport
		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (float)sizex;
		viewport.Height = (float)sizey;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		context->RSSetViewports(1, &viewport);

		////    VS and PS    ////
		//default shaders
		ID3D10Blob *VS, *PS; //release this after CreateInputLayout()
		hr = D3DCompileFromFile(L"VertexShader.hlsl", 0, 0, "main", "vs_5_0", 0, 0, &VS, 0); CHECKHR();
		hr = D3DCompileFromFile(L"PixelShader.hlsl", 0, 0, "main", "ps_5_0", 0, 0, &PS, 0); CHECKHR();
		hr = device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &defaultVS); CHECKHR();
		hr = device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &defaultPS); CHECKHR();
		context->VSSetShader(defaultVS, 0, 0);
		context->PSSetShader(defaultPS, 0, 0);

		////    INPUT LAYOUT    ////
		//defaul input layout
		D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//if you need to pass something on your own to PS or VS per vertex
			//{ "SOME_MORE_DATA", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		hr = device->CreateInputLayout(ied, 3, VS->GetBufferPointer(), VS->GetBufferSize(), &layout); CHECKHR();
		VS->Release();
		PS->Release();
		context->IASetInputLayout(layout);

		///    BLEND STATE    ////
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));
		D3D11_RENDER_TARGET_BLEND_DESC rtbd;
		ZeroMemory(&rtbd, sizeof(rtbd));
		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;
		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.RenderTarget[0] = rtbd;
		hr = device->CreateBlendState(&blendDesc, &blendState); CHECKHR();
		if (blendState == nullptr)
			exit(0);

		//// *********** PIPELINE SETUP ENDS HERE *********** ////

		//main objects
		DrawManager = new CDrawManager;
		Camera = new CCamera;
		Input = new CInput;
		EventManager = new CEventManager;
		ResourceManager = new CResourceManager;

		//timer
		LARGE_INTEGER li;
		if (!QueryPerformanceFrequency(&li))
		{
			MessageBox(0, "QueryPerformanceFrequency() failed", "Error", MB_ICONERROR);
			exit(0);
		}
		frequency = double(li.QuadPart);
		QueryPerformanceCounter(&li);
		startTime = li.QuadPart;
		prevFrameTime = startTime;
		gameTime = 0;
		frameTime = 0;
	}

	void CCore::UpdateGameTime()
	{
		LARGE_INTEGER currentTime;
		long long frameTickCount;
		QueryPerformanceCounter(&currentTime);
		frameTickCount = currentTime.QuadPart - prevFrameTime;
		frameTime = double(frameTickCount) / frequency;
		prevFrameTime = currentTime.QuadPart;
		gameTime = double(currentTime.QuadPart - startTime) / frequency;
	}

	HWND CCore::GetWindowHandle()
	{
		return window->handle;
	}

	int CCore::Run()
	{
		return window->Run();
	}

	void CCore::SetWindowTitle(const char* title)
	{
		SetWindowText(window->handle, title);
	}

	void CCore::SetBackgroundColor(SColor color)
	{
		backBufferColor[0] = color.r;
		backBufferColor[1] = color.g;
		backBufferColor[2] = color.b;
		backBufferColor[3] = color.a;
	}

	void CCore::OpenConsole()
	{
		AllocConsole();
		SetConsoleTitle("Console");
		freopen("CONOUT$", "w", stdout);
		freopen("CONIN$", "r", stdin);
	}

	void CCore::CloseConsole()
	{
		FreeConsole();
	}

	double CCore::GetFrameTime()
	{
		return frameTime;
	}

	double CCore::GetGameTime()
	{
		return gameTime;
	}

	double CCore::GetFps()
	{
		return 1 / frameTime;
	}

	void CCore::SetFullscreen(bool state)
	{
		fullscreen = state;
		swapChain->SetFullscreenState(state, NULL);
	}

	bool CCore::GetFullscreen()
	{
		return fullscreen;
	}

	POINT CCore::GetCursorPos()
	{
		POINT p;
		::GetCursorPos(&p);
		ScreenToClient(window->handle, &p);
		return p;
	}

	POINTF CCore::GetCursorWorldPos(float z)
	{
		POINT p = GetCursorPos();
		POINTF viewPortCoord;
		viewPortCoord.x = float(2 * p.x) / clientSize.x - 1;
		viewPortCoord.y = float(2 * p.y) / clientSize.y - 1;
		POINTF viewPortWorldSizeAtZ;
		viewPortWorldSizeAtZ.x = (20.0f - z) * tan(Camera->fovAngle) / 2 * viewPortCoord.x + Camera->Position.x;
		viewPortWorldSizeAtZ.y = (20.0f - z) * tan(Camera->fovAngle) / 2 * -viewPortCoord.y + Camera->Position.y;
		return viewPortWorldSizeAtZ;
	}

	void CCore::Destroy()
	{
		//engine objects
		//keep this first
		ResourceManager->Destroy();
		EventManager->Destroy();
		Input->Destroy();
		Camera->Destroy();
		DrawManager->Destroy();
		//com objects
		layout->Release();
		defaultPS->Release();
		defaultVS->Release();
		swapChain->Release();
		depthStencilView->Release();
		depthStencilBuffer->Release();
		blendState->Release();
		backBuffer->Release();
		device->Release();
		context->Release();
		delete window;
		delete this;
	}
}