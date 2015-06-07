#include "Private.h"
#include <gdiplus.h>

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
			D3D_DRIVER_TYPE_HARDWARE,NULL,NULL,NULL,NULL,
			D3D11_SDK_VERSION,
			&scd,
			&swapChain,
			&device,NULL,
			&context);

		////    BACK BUFFER AS RENDER TARGET, DEPTH STENCIL   ////
		// get the address of the back buffer
		ID3D11Texture2D* buf;
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&buf);
		// use the back buffer address to create the render target
		device->CreateRenderTargetView(buf, NULL, &backBuffer);
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
		device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
		device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

		//Set our Render Target
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

		///    BLEND STATE    ////
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));
		D3D11_RENDER_TARGET_BLEND_DESC rtbd;
		ZeroMemory(&rtbd, sizeof(rtbd));
		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
		rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
		rtbd.BlendOp = D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;
		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.RenderTarget[0] = rtbd;
		device->CreateBlendState(&blendDesc, &blendState);

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

	ID3D11Texture2D* Core::CreateTexture2D(const WCHAR* file)
	{
		ULONG_PTR m_gdiplusToken;
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

		Gdiplus::Bitmap* gdibitmap = new Gdiplus::Bitmap(file);
		UINT h = gdibitmap->GetHeight();
		UINT w = gdibitmap->GetWidth();

		HBITMAP hbitmap;		
		Gdiplus::Color c(0, 0, 0);
		gdibitmap->GetHBITMAP(c, &hbitmap);
		delete gdibitmap;
		Gdiplus::GdiplusShutdown(m_gdiplusToken);

		BITMAP bitmap;
		GetObject(hbitmap, sizeof(bitmap), (LPVOID)&bitmap);
		BYTE* data = (BYTE*)bitmap.bmBits;

		ID3D11Texture2D *tex;
		D3D11_TEXTURE2D_DESC tdesc;
		D3D11_SUBRESOURCE_DATA tbsd;

		tbsd.pSysMem = (void *)data;
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

		DeleteObject(hbitmap);

		return tex;
	}

	ID3D11Texture2D* Core::CreateTexture2DFromText(std::wstring text)
	{
		ULONG_PTR m_gdiplusToken;
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

		UINT h = 80;
		UINT w = 80;
		Gdiplus::Bitmap* gdibitmap = new Gdiplus::Bitmap(h, w, PixelFormat32bppARGB);
		Gdiplus::Graphics* g = new Gdiplus::Graphics(gdibitmap);		
		Gdiplus::PointF point(0, 0);
		Gdiplus::Rect rect(0, 0, w, h);
		Gdiplus::Font* font = new Gdiplus::Font(&Gdiplus::FontFamily(L"Arial"), 14);
		Gdiplus::SolidBrush* brush = new Gdiplus::SolidBrush(Gdiplus::Color::White);
		Gdiplus::SolidBrush* brush1 = new Gdiplus::SolidBrush(Gdiplus::Color::Black);
		g->FillRectangle(brush1, rect);
		g->DrawString(text.c_str(), (INT)text.length(), font, point, brush);
		delete g;
		delete brush;
		delete brush1;
		delete font;

		HBITMAP hbitmap;
		Gdiplus::Color c(0, 0, 0);
		gdibitmap->GetHBITMAP(c, &hbitmap);
		delete gdibitmap;
		Gdiplus::GdiplusShutdown(m_gdiplusToken);

		BITMAP bitmap;
		GetObject(hbitmap, sizeof(bitmap), (LPVOID)&bitmap);
		BYTE* data = (BYTE*)bitmap.bmBits;

		ID3D11Texture2D *tex = nullptr;
		D3D11_TEXTURE2D_DESC tdesc;
		D3D11_SUBRESOURCE_DATA tbsd;

		tbsd.pSysMem = (void *)data;
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

		DeleteObject(hbitmap);

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