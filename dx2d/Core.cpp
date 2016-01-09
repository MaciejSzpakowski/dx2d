#include "Viva.h"
#include "Embedded.h"

namespace Viva
{
	void IntActivity();
	extern const char rc_PixelShader[];
	extern const char rc_PostProcessing[];
	extern const char rc_VertexShader[];

	CCore::CCore(Size _clientSize, int style)
	{
		HRESULT hr = 0;
		alphaEnabled = false;
		fullscreen = false;
		clientSize = _clientSize;
		//assign global variable
		Core = this;
		//create window
		window = new Window(_clientSize, style, IntActivity);

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
		scd.OutputWindow = window->GetHandle();                // the window to be used
		scd.SampleDesc.Quality = 0;
		scd.SampleDesc.Count = 1;                               // no anti aliasing
		scd.Windowed = TRUE;                                    // windowed/full-screen mode
		//scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // alternative fullscreen mode

		UINT creationFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;

		////    DEVICE, DEVICE CONTEXT AND SWAP CHAIN    ////
		hr = D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL,
			D3D11_SDK_VERSION, &scd, &swapChain, &device, NULL,
			&context); CHECKHR();

		////    BACK BUFFER AS RENDER TARGET, DEPTH STENCIL   ////
		// get the address of the back buffer
		ID3D11Texture2D* buf;
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&buf);
		// use the back buffer address to create the render target
		hr = device->CreateRenderTargetView(buf, NULL, &backBuffer); CHECKHR();
		buf->Release();

		//Describe our Depth/Stencil Buffer and View
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = (UINT)clientSize.width;
		depthStencilDesc.Height = (UINT)clientSize.height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		hr = device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer); CHECKHR();
		hr = device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView); CHECKHR();

		////   VIEWPORT    ////
		// Set the viewport
		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (FLOAT)clientSize.width;
		viewport.Height = (FLOAT)clientSize.height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		context->RSSetViewports(1, &viewport);

		////    VS and PS    ////
		//default shaders
		ID3D10Blob *vs; //release vs after CreateInputLayout()
						//alternative to loading shader from cso file
						//hr = D3DCompileFromFile(L"VertexShader.hlsl", 0, 0, "main", "vs_5_0", 0, 0, &vs, 0); CHECKHR();
		hr = D3DCompile(rc_VertexShader, strlen(rc_VertexShader),
			0, 0, 0, "main", "vs_5_0", 0, 0, &vs, 0); CHECKHR();
		hr = device->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), 0,
			&defaultVS); CHECKHR();
		context->VSSetShader(defaultVS, 0, 0);
		defaultPS = CreatePixelShaderFromString(rc_PixelShader, "main");
		defaultPost = CreatePixelShaderFromString(rc_PostProcessing, "main");

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
		hr = device->CreateInputLayout(ied, 3, vs->GetBufferPointer(), vs->GetBufferSize(),
			&layout); CHECKHR();
		vs->Release();
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
		rtbd.SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;// D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND_ONE;//D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;
		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.RenderTarget[0] = rtbd;
		hr = device->CreateBlendState(&blendDesc, &blendState); CHECKHR();

		//// *********** PIPELINE SETUP ENDS HERE *********** ////

		//main objects
		DrawManager = new CDrawManager();
		camera = new Camera();
		InputManager = new CInputManager();
		EventManager = new CEventManager();
		ResourceManager = new CResourceManager();
		DebugManager = new CDebugManager(DrawManager->GetDefaultFont(),camera);

		//timer
		LARGE_INTEGER li;
		if (!QueryPerformanceFrequency(&li))
			throw VIVA_ERROR("QueryPerformanceFrequency() failed");
		frequency = double(li.QuadPart);
		QueryPerformanceCounter(&li);
		startTime = li.QuadPart;
		prevFrameTime = startTime;
		gameTime = 0;
		frameTime = 0;
	}

	void CCore::_UpdateGameTime()
	{
		LARGE_INTEGER currentTime;
		long long frameTickCount;
		QueryPerformanceCounter(&currentTime);
		frameTickCount = currentTime.QuadPart - prevFrameTime;
		frameTime = double(frameTickCount) / frequency;
		prevFrameTime = currentTime.QuadPart;
		gameTime = double(currentTime.QuadPart - startTime) / frequency;
	}

	void CCore::SetBackgroundColor(const Color& color)
	{
		backBufferColor[0] = color.r;
		backBufferColor[1] = color.g;
		backBufferColor[2] = color.b;
		backBufferColor[3] = color.a;
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

	void CCore::SetFullscreen(bool state)
	{
		fullscreen = state;
		swapChain->SetFullscreenState(state, NULL);
	}

	void CCore::SaveScreenshot(const wchar_t* file)
	{
		/* HOW TO GET ID3D11TEXTURE2D and D3D11_TEXTURE2D_DESC from ID3D11ShaderResourceView
		ID3D11Texture2D *pTextureInterface = 0;
		ID3D11Resource *res;
		shaderResource->GetResource(&res);
		res->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
		D3D11_TEXTURE2D_DESC desc;
		pTextureInterface->GetDesc(&desc);
		*/

		throw VIVA_ERROR("Not implemented");
		//ID3D11DeviceContext::CopyResource
		//context->CopyResource(
	}

	void CCore::Exit()
	{
		PostMessage(window->GetHandle(), WM_CLOSE, 0, 0);
	}

	ID3D11PixelShader* CCore::CreatePixelShaderFromFile(const wchar_t* filepath, const char* entryPoint, const char* target)
	{
		ID3D11PixelShader* result;
		ID3D10Blob *ps;
		HRESULT hr = D3DCompileFromFile(filepath, 0, 0, entryPoint, target, 0, 0, &ps, 0); CHECKHR();
		if (hr != 0)
			throw VIVA_ERROR("Error compiling pixel shader");
		//D3DCompile
		hr = device->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), 0,	&result); CHECKHR();
		ps->Release();
		return result;
	}

	// create pixel shader from const char*
	ID3D11PixelShader* CCore::CreatePixelShaderFromString(const char* str, const char* entryPoint, const char* target)
	{
		ID3D11PixelShader* result;

		ID3D10Blob *ps;
		HRESULT hr = D3DCompile(str, strlen(str), 0, 0, 0, entryPoint, target, 0, 0, &ps, 0);
		if (hr != 0)
			throw VIVA_ERROR("Error compiling pixel shader");
		//D3DCompile
		hr = device->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), 0,	&result); CHECKHR();
		ps->Release();
		return result;
	}

	void CCore::Destroy()
	{
		//engine objects
		DebugManager->Destroy();
		EventManager->Destroy();
		InputManager->Destroy();
		camera->Destroy();
		DrawManager->Destroy();
		ResourceManager->Destroy();
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
		window->Destroy();
		delete this;
	}
}