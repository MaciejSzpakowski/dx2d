#include "Private.h"
#include <gdiplus.h>
#pragma comment(lib,"Gdiplus.lib")
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

	HRESULT hr;

	void Render(CCore* d3d)
	{		
		Camera->CamTransform();
		d3d->context->ClearRenderTargetView(d3d->backBuffer, d3d->backBufferColor);		
		DrawManager->DrawAll();
		d3d->swapChain->Present(0, 0);
		Input->Activity();
		EventManager->Activity();
		d3d->UpdateGameTime();
	}

	ID3D11Device* GetDevice()
	{
		return Core->device;
	}

	ID3D11DeviceContext* GetContext()
	{
		return Core->context;
	}

	//applies time factor
	void AddFloat3(XMFLOAT3* src, XMFLOAT3* dst)
	{
		dst->x += (src->x * float(Core->frameTime));
		dst->y += (src->y * float(Core->frameTime));
		dst->z += (src->z * float(Core->frameTime));
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
		GetDevice()->CreateSamplerState(&sampDesc, sampler);
	}

	namespace Functions
	{
		CCore* NewCore(int sizex, int sizey, std::function<void()> worker, int style)
		{
			CCore* core = new CCore(sizex, sizey, worker, style);
			return core;
		}	

		ID3D11Texture2D* CreateTexture2D(BYTE* data, int width, int height)
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

			GetDevice()->CreateTexture2D(&tdesc, &tbsd, &tex);

			return tex;
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
			//ID3D11Texture2D and BITMAP have red and blue swapped
			//swap it back
			//this for loop is ~10% of the function
			for (int i = 0; i < (int)(h*w * 4); i += 4)
			{
				BYTE temp = data[i];
				data[i] = data[i + 2];
				data[i + 2] = temp;
			}

			ID3D11Texture2D* tex = CreateTexture2D(data, w, h);
			DeleteObject(hbitmap);
			return tex;
		}

		//loads texture to shader resource, checks cache first
		//if it's not in cache then load and store
		void LoadCachedTexture(const WCHAR* textureFile, ID3D11ShaderResourceView*& shader)
		{
			CTexture* res = ResourceManager->GetTexture(textureFile);
			if (res != nullptr)
			{
				shader = res->shaderResource;
			}
			else
			{
				D3D11_TEXTURE2D_DESC desc;
				auto tex = Functions::CreateTexture2DFromFile(textureFile);
				if (tex == nullptr)
					throw 0;
				tex->GetDesc(&desc);
				GetDevice()->CreateShaderResourceView(tex, 0, &shader);
				tex->Release();
				//add to resources
				CTexture* newRes = new CTexture;
				newRes->fileName = textureFile;
				newRes->Height = desc.Height;
				newRes->Width = desc.Width;
				newRes->shaderResource = shader;
				ResourceManager->AddTexture(newRes);
			}
		}

		void Checkhr(const char* file, int line)
		{
			if (hr == 0)
				return;
			char str[128];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0,
				hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
				str, 128, 0);
			std::stringstream message;
			message << file << " line: " << line << "\n" << str;
			
			MessageBox(0, message.str().c_str(), "HRESULT error", MB_ICONERROR);
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
			int p1 = rand();
			int p2 = rand() << 15;
			int p3 = rand() << 30;
			int t = p1 | p2 | p3;
			return t % (max - min) + min;
		}		
	}
}
