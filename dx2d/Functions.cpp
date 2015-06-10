#include "Private.h"
#include <gdiplus.h>

namespace dx2d
{
	CCore* Core;
	CKey Key;
	CDrawManager* DrawManager;
	CCamera* Camera;
	CInput* Input;
	CEventManager* EventManager;

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

	namespace Functions
	{
		CCore* NewCore(int sizex, int sizey, std::function<void()> worker)
		{
			CCore* core = new CCore(sizex, sizey, worker);
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

		ID3D11Texture2D* CreateTexture2DFromText(std::wstring text)
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

			ID3D11Texture2D *tex = CreateTexture2D(data, w, h);
			DeleteObject(hbitmap);
			return tex;
		}
	}
}
