#include "Viva.h"

namespace Viva
{
	ID3D11ShaderResourceView* SrvFromColorArray(const Color data[], const Size& _size)
	{
		ID3D11Texture2D *tex;
		D3D11_TEXTURE2D_DESC tdesc;
		D3D11_SUBRESOURCE_DATA tbsd;

		tbsd.pSysMem = (void *)data;
		tbsd.SysMemPitch = (UINT)_size.width * 4;
		tbsd.SysMemSlicePitch = (UINT)_size.height*(UINT)_size.width * 4;

		tdesc.Width = (UINT)_size.width;
		tdesc.Height = (UINT)_size.height;
		tdesc.MipLevels = 1;
		tdesc.ArraySize = 1;

		tdesc.SampleDesc.Count = 1;
		tdesc.SampleDesc.Quality = 0;
		tdesc.Usage = D3D11_USAGE_DEFAULT;
		tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		tdesc.CPUAccessFlags = 0;
		tdesc.MiscFlags = 0;

		HRESULT hr = Core->zDevice->CreateTexture2D(&tdesc, &tbsd, &tex); CHECKHR();

		D3D11_TEXTURE2D_DESC desc;
		tex->GetDesc(&desc);
		ID3D11ShaderResourceView* srv;
		hr = Core->zDevice->CreateShaderResourceView(tex, 0, &srv); CHECKHR();
		tex->Release();

		return srv;
	}

	CTexture::CTexture(const wchar_t* filepath)
	{
		ULONG_PTR m_gdiplusToken;
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

		Gdiplus::Bitmap* gdibitmap = new Gdiplus::Bitmap(filepath);
		if (gdibitmap->GetLastStatus() != 0)
		{
			std::wstringstream msg;
			msg << L"Could not open " << filepath;
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

		CTexture* tex = CreateTexture((Color*)data, Size(w, h), filepath);
		DeleteObject(hbitmap);
		delete gdibitmap;
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
		return tex;
	}

	// create texture from Color array
	CTexture::CTexture(const Color data[], const Size& _size, const wchar_t* _name)
	{
		

		zCached = false;
		size = _size;
		zName = _name;
		zShaderResource = SrvFromColorArray(data, _size);
	}
}