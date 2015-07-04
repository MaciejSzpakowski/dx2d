#include "Private.h"

namespace dx2d
{
	void Render(CCore* d3d);

	CCore::CCore(int sizex, int sizey, std::function<void()> worker, int style)
	{
		srand((int)time(0));
		hr = 0;
		zFullscreen = false;
		zClientSize.x = sizex;
		zClientSize.y = sizey;
		//assign global variable
		Core = this;
		//create window
		zWindow = new CWindow(sizex, sizey, style);
		zWindow->zWorker = worker;
		zWindow->zRender = Render;

		zBackBufferColor[0] = 1.0f;
		zBackBufferColor[1] = 1.0f;
		zBackBufferColor[2] = 1.0f;
		zBackBufferColor[3] = 1.0f;

		//// *********** PIPELINE SETUP STARTS HERE *********** ////
		// create a struct to hold information about the swap chain
		DXGI_SWAP_CHAIN_DESC scd;
		ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
		scd.BufferCount = 1;                                    // one back buffer
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
		scd.OutputWindow = zWindow->zHandle;                 // the window to be used
		scd.SampleDesc.Quality = 0;
		scd.SampleDesc.Count = 1;                               // no anti aliasing
		scd.Windowed = TRUE;                                    // windowed/full-screen mode
		//scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // alternative fullscreen mode

		////    DEVICE, DEVICE CONTEXT AND SWAP CHAIN    ////
		hr = D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
			D3D11_SDK_VERSION,
			&scd,
			&zSwapChain,
			&zDevice, NULL,
			&zContext);
		CHECKHR();

		////    BACK BUFFER AS RENDER TARGET, DEPTH STENCIL   ////
		// get the address of the back buffer
		ID3D11Texture2D* buf;
		zSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&buf);
		// use the back buffer address to create the render target
		hr = zDevice->CreateRenderTargetView(buf, NULL, &zBackBuffer); CHECKHR();
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
		hr = zDevice->CreateTexture2D(&depthStencilDesc, NULL, &zDepthStencilBuffer); CHECKHR();
		hr = zDevice->CreateDepthStencilView(zDepthStencilBuffer, NULL, &zDepthStencilView); CHECKHR();

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
		zContext->RSSetViewports(1, &viewport);

		////    VS and PS    ////
		//default shaders
		ID3D10Blob *vs; //release vs after CreateInputLayout()
		//alternative method to loading shader from cso file
		hr = D3DCompileFromFile(L"VertexShader.hlsl", 0, 0, "main", "vs_5_0", 0, 0, &vs, 0); CHECKHR();
		hr = zDevice->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), 0,
			&zDefaultVS); CHECKHR();
		zContext->VSSetShader(zDefaultVS, 0, 0);
		zDefaultPS = Functions::CreatePSFromFile(L"PixelShader.hlsl", "main");
		zDefaultPost = Functions::CreatePSFromFile(L"PostProcessing.hlsl", "main");

		////    INPUT LAYOUT    ////
		//defaul input layout
		D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
				0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//if you need to pass something on your own to PS or VS per vertex
			//{ "SOME_MORE_DATA", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		hr = zDevice->CreateInputLayout(ied, 3, vs->GetBufferPointer(), vs->GetBufferSize(), 
			&zLayout); CHECKHR();
		vs->Release();
		zContext->IASetInputLayout(zLayout);

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
		hr = zDevice->CreateBlendState(&blendDesc, &zBlendState); CHECKHR();
		if (zBlendState == nullptr)
			exit(0);

		//// *********** PIPELINE SETUP ENDS HERE *********** ////

		//main objects
		DrawManager = new CDrawManager;
		Camera = new CCamera;
		Input = new CInput;
		EventManager = new CEventManager;
		ResourceManager = new CResourceManager;
		DebugManager = new CDebugManager;

		//timer
		LARGE_INTEGER li;
		if (!QueryPerformanceFrequency(&li))
		{
			MessageBox(0, L"QueryPerformanceFrequency() failed", L"Error", MB_ICONERROR);
			exit(0);
		}
		zFrequency = double(li.QuadPart);
		QueryPerformanceCounter(&li);
		zStartTime = li.QuadPart;
		zPrevFrameTime = zStartTime;
		zGameTime = 0;
		zFrameTime = 0;

		//defalut font for drawmanager
		//15x21 one char
		//20x5 all chars
		vector<Rect> chars1;
		for (int i = 0; i<5; i++)
			for (int j = 0; j < 20; j++)
			{
				chars1.push_back(Rect(15.0f / 300.0f*j, 21.0f / 105.0f*i, 15.0f /
					300.0f*(j + 1), 21 / 105.0f*(i + 1)));
			}
		CTexture* tex1 = Functions::GetCachedTextureFromFile(L"font.png");
		DrawManager->zDefaultFont = new CBitmapFont(tex1, chars1);
		DrawManager->AddBitmapFont(DrawManager->zDefaultFont);
		DebugManager->Init(DrawManager->GetDefaultFont());
	}

	void CCore::zUpdateGameTime()
	{
		LARGE_INTEGER currentTime;
		long long frameTickCount;
		QueryPerformanceCounter(&currentTime);
		frameTickCount = currentTime.QuadPart - zPrevFrameTime;
		zFrameTime = double(frameTickCount) / zFrequency;
		zPrevFrameTime = currentTime.QuadPart;
		zGameTime = double(currentTime.QuadPart - zStartTime) / zFrequency;
	}

	HWND CCore::GetWindowHandle()
	{
		return zWindow->zHandle;
	}

	int CCore::Run()
	{
		return zWindow->zRun();
	}

	void CCore::SetWindowTitle(const wchar_t* title)
	{
		SetWindowText(zWindow->zHandle, title);
	}

	void CCore::SetBackgroundColor(Color color)
	{
		zBackBufferColor[0] = color.r;
		zBackBufferColor[1] = color.g;
		zBackBufferColor[2] = color.b;
		zBackBufferColor[3] = color.a;
	}

	void CCore::OpenConsole()
	{
		AllocConsole();
		SetConsoleTitle(L"Console");
		freopen("CONOUT$", "w", stdout);
		freopen("CONIN$", "r", stdin);
	}

	void CCore::CloseConsole()
	{
		FreeConsole();
	}

	double CCore::GetFrameTime()
	{
		return zFrameTime;
	}

	double CCore::GetGameTime()
	{
		return zGameTime;
	}

	double CCore::GetFps()
	{
		return 1 / zFrameTime;
	}

	void CCore::SetFullscreen(bool state)
	{
		zFullscreen = state;
		zSwapChain->SetFullscreenState(state, NULL);
	}

	bool CCore::GetFullscreen()
	{
		return zFullscreen;
	}

	POINT CCore::GetCursorPos()
	{
		POINT p;
		::GetCursorPos(&p);
		ScreenToClient(zWindow->zHandle, &p);
		return p;
	}	

	void CCore::Destroy()
	{
		//engine objects
		DebugManager->Destroy();
		EventManager->Destroy();
		Input->Destroy();
		Camera->Destroy();
		DrawManager->Destroy();
		ResourceManager->Destroy();
		//com objects
		zLayout->Release();
		zDefaultPS->Release();
		zDefaultVS->Release();
		zSwapChain->Release();
		zDepthStencilView->Release();
		zDepthStencilBuffer->Release();
		zBlendState->Release();
		zBackBuffer->Release();
		zDevice->Release();
		zContext->Release();
		delete zWindow;
		delete this;
	}
}