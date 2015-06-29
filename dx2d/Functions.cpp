#include "Private.h"
#include <ctime>
#include <sstream>

namespace dx2d
{
	//globals
	CCore* Core;
	CKey Key;
	CDrawManager* DrawManager;
	CCamera* Camera;
	CInput* Input;
	CEventManager* EventManager;
	CResourceManager* ResourceManager;
	CDebugManager* DebugManager;

	HRESULT hr;

	void Render(CCore* d3d)
	{		
		Camera->zCamTransform();
		d3d->zContext->ClearRenderTargetView(d3d->zBackBuffer, d3d->zBackBufferColor);
		DrawManager->zDrawAll();		
		d3d->zSwapChain->Present(0, 0);
		Input->zActivity();
		EventManager->zActivity();
		d3d->zUpdateGameTime();
	}	

	void CreateSampler(TEX_FILTER mode, ID3D11SamplerState** sampler)
	{
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = mode == TEX_FILTER::POINT ? 
			D3D11_FILTER_MIN_MAG_MIP_POINT : D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		Core->zDevice->CreateSamplerState(&sampDesc, sampler);
	}

	namespace Functions
	{
		CCore* NewCore(int sizex, int sizey, std::function<void()> worker, int style)
		{
			CCore* core = new CCore(sizex, sizey, worker, style);
			return core;
		}

		ID3D11Texture2D* CreateTexture2DFromBytes(BYTE* data, int width, int height)
		{
			ID3D11Texture2D *tex;
			D3D11_TEXTURE2D_DESC tdesc;
			D3D11_SUBRESOURCE_DATA tbsd;

			tbsd.pSysMem = (void *)data;
			tbsd.SysMemPitch = width * 4;
			tbsd.SysMemSlicePitch = height*width * 4;

			tdesc.Width = width;
			tdesc.Height = height;
			tdesc.MipLevels = 1;
			tdesc.ArraySize = 1;

			tdesc.SampleDesc.Count = 1;
			tdesc.SampleDesc.Quality = 0;
			tdesc.Usage = D3D11_USAGE_DEFAULT;
			tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

			tdesc.CPUAccessFlags = 0;
			tdesc.MiscFlags = 0;

			Core->zDevice->CreateTexture2D(&tdesc, &tbsd, &tex);

			return tex;
		}

		ID3D11Texture2D* CreateTexture2DFromGdibitmap(Gdiplus::Bitmap* gdibitmap)
		{
			UINT h = gdibitmap->GetHeight();
			UINT w = gdibitmap->GetWidth();

			HBITMAP hbitmap;
			Gdiplus::Color c(0, 0, 0);
			gdibitmap->GetHBITMAP(c, &hbitmap);

			BITMAP bitmap;
			GetObject(hbitmap, sizeof(bitmap), (LPVOID)&bitmap);
			BYTE* data = (BYTE*)bitmap.bmBits;
			//have to swap red and blue because bitmap has pixels as integers
			//and shader will read in byte by byte instead of int by int
			//this for loop is ~10% of the function
			for (int i = 0; i < (int)(h*w * 4); i += 4)
			{
				BYTE temp = data[i];
				data[i] = data[i + 2];
				data[i + 2] = temp;
			}

			ID3D11Texture2D* tex = CreateTexture2DFromBytes(data, w, h);
			DeleteObject(hbitmap);
			return tex;
			
			/*Gdiplus::BitmapData* bitmapData = new Gdiplus::BitmapData;
			Gdiplus::Rect rect(0, 0, w, h);

			// Lock a 5x3 rectangular portion of the bitmap for reading.
			gdibitmap->LockBits(
				&rect,
				Gdiplus::ImageLockModeRead,
				PixelFormat32bppARGB,
				bitmapData);

			// Display the hexadecimal value of each pixel in the 5x3 rectangle.
			UINT* pixels = (UINT*)bitmapData->Scan0;

			gdibitmap->UnlockBits(bitmapData);

			delete bitmapData;
			ID3D11Texture2D* tex = CreateTexture2DFromBytes((BYTE*)pixels, w, h);
			return tex;/**/
		}

		ID3D11Texture2D* CreateTexture2DFromFile(const WCHAR* file)
		{
			ULONG_PTR m_gdiplusToken;
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

			Gdiplus::Bitmap* gdibitmap = new Gdiplus::Bitmap(file);
			if (gdibitmap->GetLastStatus() != 0)
			{
				std::wstringstream msg;
				msg << L"Could not open " << file;
				MessageBoxW(0, msg.str().c_str(), L"File error", MB_ICONEXCLAMATION);
				delete gdibitmap;
				Gdiplus::GdiplusShutdown(m_gdiplusToken);
				return nullptr;
			}
			ID3D11Texture2D* tex = CreateTexture2DFromGdibitmap(gdibitmap);
			delete gdibitmap;
			Gdiplus::GdiplusShutdown(m_gdiplusToken);
			return tex;
		}

		ID3D11Texture2D* CreateTexture2DFromResource(int resource)
		{
			ULONG_PTR m_gdiplusToken;
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

			Gdiplus::Bitmap* gdibitmap = Gdiplus::Bitmap::FromResource(GetModuleHandle(0),
				MAKEINTRESOURCE(resource));
			Gdiplus::Status status = gdibitmap->GetLastStatus();
			if (status != 0)
			{
				std::wstringstream msg;
				msg << L"Could not load " << resource;
				MessageBoxW(0, msg.str().c_str(), L"Resource error", MB_ICONEXCLAMATION);
				delete gdibitmap;
				Gdiplus::GdiplusShutdown(m_gdiplusToken);
				return nullptr;
			}
			ID3D11Texture2D* tex = CreateTexture2DFromGdibitmap(gdibitmap);
			delete gdibitmap;
			Gdiplus::GdiplusShutdown(m_gdiplusToken);
			return tex;
		}

		CTexture* GetCachedTextureFromFile(const WCHAR* file)
		{
			CTexture* res = ResourceManager->GetTexture(file);
			if (res != nullptr)
				return res;
			else
			{				
				D3D11_TEXTURE2D_DESC desc;
				auto tex = Functions::CreateTexture2DFromFile(file);
				if (tex == nullptr)
					throw 0;
				tex->GetDesc(&desc);
				ID3D11ShaderResourceView* srv;
				Core->zDevice->CreateShaderResourceView(tex, 0, &srv);
				tex->Release();
				CTexture* newRes = new CTexture(true, desc.Height, desc.Width, file, srv);
				//add to resources
				ResourceManager->AddTexture(newRes);
				return newRes;
			}
		}

		CTexture* GetCachedTextureFromResource(int resource, wstring name)
		{
			CTexture* res = ResourceManager->GetTexture(name);
			if (res != nullptr)
				return res;
			else
			{				
				D3D11_TEXTURE2D_DESC desc;
				auto tex = Functions::CreateTexture2DFromResource(resource);
				if (tex == nullptr)
					throw 0;
				tex->GetDesc(&desc);
				ID3D11ShaderResourceView* srv;
				Core->zDevice->CreateShaderResourceView(tex, 0, &srv);
				tex->Release();
				CTexture* newRes = new CTexture(true, desc.Height, desc.Width, name, srv);
				//add to resources
				ResourceManager->AddTexture(newRes);
				return newRes;
			}
		}

		CTexture* GetUncachedTextureFromBytes(BYTE* data, int width, int height)
		{
			auto tex = Functions::CreateTexture2DFromBytes(data, width, height);
			if (tex == nullptr)
			{
				MessageBox(0, L"GetUncachedTextureFromBytes(BYTE* data, int width, int height) failed",
					L"Error", MB_ICONEXCLAMATION);
				return nullptr;
			}
			ID3D11ShaderResourceView* srv;
			Core->zDevice->CreateShaderResourceView(tex, 0, &srv);
			tex->Release();
			CTexture* newRes = new CTexture(false, height, width, L"custom", srv);
			return newRes;
		}

		void Checkhr(const char* file, int line)
		{
			if (hr == 0)
				return;
			char str[128];
			FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0,
				hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
				str, 128, 0);
			std::wstringstream message;
			message << file << L" line: " << line << L"\n" << str;
			
			MessageBox(0, message.str().c_str(), L"HRESULT error", MB_ICONERROR);
			exit(1);
		}

		double RndDouble()
		{
			unsigned int p1 = rand();
			unsigned int p2 = rand() << 15;
			unsigned int p3 = rand() << 30;
			unsigned int t = p1 | p2 | p3;
			return (double)t / 0xffffffff;
		}

		int RndInt(int min, int max)
		{
			unsigned int p1 = rand();
			unsigned int p2 = rand() << 15;
			unsigned int p3 = rand() << 30;
			unsigned int t = p1 | p2 | p3;
			return t % (max - min) + min;
		}		
	}

	namespace Collision
	{
		bool IsColliding(CCircle* c1, CCircle* c2)
		{
			XMVECTOR v = XMVectorSubtract(c1->GetPositionVector(), c2->GetPositionVector());
			XMVECTOR len = XMVector2Length(v);
			return XMVectorGetX(len) < (c1->Radius + c2->Radius);
		}

		bool IsColliding(CSprite* s1, CSprite* s2)
		{
			XMVECTOR A = XMVectorSet(-1.0f, -1.0f, 0, 0);
			XMVECTOR B = XMVectorSet(1.0f, -1.0f, 0, 0);
			XMVECTOR C = XMVectorSet(1.0f, 1.0f, 0, 0);
			XMVECTOR D = XMVectorSet(-1.0f, 1.0f, 0, 0);
			XMVECTOR A1 = XMVector3Transform(A, s1->zWorld);
			XMVECTOR B1 = XMVector3Transform(B, s1->zWorld);
			XMVECTOR C1 = XMVector3Transform(C, s1->zWorld);
			XMVECTOR D1 = XMVector3Transform(D, s1->zWorld);
			XMVECTOR A2 = XMVector3Transform(A, s2->zWorld);
			XMVECTOR B2 = XMVector3Transform(B, s2->zWorld);
			XMVECTOR C2 = XMVector3Transform(C, s2->zWorld);
			XMVECTOR D2 = XMVector3Transform(D, s2->zWorld);
			bool result;
			result = TriangleTests::Intersects(A1, B1, C1, A2, B2, C2);
			if (result) return true;
			result = TriangleTests::Intersects(A1, B1, C1, A2, C2, D2);
			if (result) return true;
			result = TriangleTests::Intersects(A1, C1, D1, A2, B2, C2);
			if (result) return true;
			result = TriangleTests::Intersects(A1, C1, D1, A2, C2, D2);
			if (result) return true;
			return false;
		}
	}
}
