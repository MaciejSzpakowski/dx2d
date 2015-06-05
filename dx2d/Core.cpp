#include "Private.h"

namespace dx2d
{
	Core::Core(int sizex, int sizey, std::function<void()> worker)
	{
		//assign global variable
		Global = this;
		//create window
		window = new Window(sizex, sizey);
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
		D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			NULL,
			NULL,
			NULL,
			D3D11_SDK_VERSION,
			&scd,
			&swapChain,
			&device,
			NULL,
			&context);

		////    BACK BUFFER AS RENDER TARGET    ////
		// get the address of the back buffer
		ID3D11Texture2D* buf;
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&buf);
		// use the back buffer address to create the render target
		device->CreateRenderTargetView(buf, NULL, &backBuffer);
		buf->Release();
		// set the render target as the back buffer
		context->OMSetRenderTargets(1, &backBuffer, NULL);

		////   VIEWPORT    ////
		// Set the viewport
		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (float)sizex;
		viewport.Height = (float)sizey;
		context->RSSetViewports(1, &viewport);		
		
		////    VS and PS    ////
		//default shaders
		ID3D10Blob *VS, *PS; //release this after CreateInputLayout()
		D3DCompileFromFile(L"VertexShader.hlsl", 0, 0, "main", "vs_5_0", 0, 0, &VS, 0);
		D3DCompileFromFile(L"PixelShader.hlsl", 0, 0, "main", "ps_5_0", 0, 0, &PS, 0);
		device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &defaultVS);
		device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &defaultPS);
		context->VSSetShader(defaultVS, 0, 0);
		context->PSSetShader(defaultPS, 0, 0);
		
		////    INPUT LAYOUT    ////
		//defaul input layout
		D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//if you need to pass something on your own to PS or VS
			//{ "SOME_MORE_DATA", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		device->CreateInputLayout(ied, 3, VS->GetBufferPointer(), VS->GetBufferSize(), &layout);
		VS->Release();
		PS->Release();
		context->IASetInputLayout(layout);

		//library objects	
		drawManager = new CDrawManager;
		camera = new CCamera;
		inputManager = new CInputManager;
	}

	ID3D11Device* Core::GetDevice()
	{
		return device;
	}

	ID3D11DeviceContext* Core::GetContext()
	{
		return context;
	}

	CDrawManager* Core::GetDrawManager()
	{
		return drawManager;
	}

	CCamera* Core::GetCamera()
	{
		return camera;
	}

	CInputManager* Core::GetInputManager()
	{
		return inputManager;
	}

	HWND Core::GetWindowHandle()
	{
		return window->handle;
	}

	int Core::Run()
	{
		return window->Run();
	}

	void Core::SetWindowTitle(const char* title)
	{
		SetWindowText(window->handle, title);
	}

	void Core::SetBackBufferColor(float color[4])
	{
		backBufferColor[0] = color[0];
		backBufferColor[1] = color[1];
		backBufferColor[2] = color[2];
		backBufferColor[3] = color[3];
	}

	ID3D11Texture2D* Core::CreateTexture2D(const char* file)
	{
		ID3D11Texture2D *tex;
		D3D11_TEXTURE2D_DESC tdesc;
		D3D11_SUBRESOURCE_DATA tbsd;
		int w = 64;
		int h = 64;

		int *buf = new int[w*h];

		for (int i = 0; i<h; i++)
			for (int j = 0; j<w; j++)
			{
				if ((i & 32) == (j & 32))
					buf[i*w + j] = 0xffff2200;
				else
					buf[i*w + j] = 0xffffaabb;
			}

		tbsd.pSysMem = (void *)buf;
		tbsd.SysMemPitch = w * 4;
		tbsd.SysMemSlicePitch = w*h * 4; // Not needed since this is a 2d texture

		tdesc.Width = w;
		tdesc.Height = h;
		tdesc.MipLevels = 1;
		tdesc.ArraySize = 1;

		tdesc.SampleDesc.Count = 1;
		tdesc.SampleDesc.Quality = 0;
		tdesc.Usage = D3D11_USAGE_DEFAULT;
		tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		tdesc.CPUAccessFlags = 0;
		tdesc.MiscFlags = 0;

		Device->CreateTexture2D(&tdesc, &tbsd, &tex);

		delete[] buf;
		return tex;
	}

	void Core::Destroy()
	{
		//engine objects
		inputManager->Destroy();
		camera->Destroy();
		drawManager->Destroy();
		//com objects
		layout->Release();
		defaultPS->Release();
		defaultVS->Release();
		swapChain->Release();
		device->Release();
		context->Release();		
		delete window;
		delete this;
	}
}