#include "Viva.h"

namespace Viva
{
	ID3D11ShaderResourceView* SrvFromColorArray(const Pixel data[], const Size& _size)
	{
		ID3D11Texture2D *tex;
		D3D11_TEXTURE2D_DESC desc;
		D3D11_SUBRESOURCE_DATA sub;

		sub.pSysMem = (void *)data;
		sub.SysMemPitch = (UINT)_size.width * 4;
		sub.SysMemSlicePitch = (UINT)_size.height*(UINT)_size.width * 4;

		desc.Width = (UINT)_size.width;
		desc.Height = (UINT)_size.height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;

		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		HRESULT hr = Core->zDevice->CreateTexture2D(&desc, &sub, &tex); CHECKHR();

		D3D11_TEXTURE2D_DESC desc2;
		tex->GetDesc(&desc2);
		ID3D11ShaderResourceView* srv;
		hr = Core->zDevice->CreateShaderResourceView(tex, 0, &srv); CHECKHR();
		tex->Release();

		return srv;
	}

	CTexture::CTexture(const wchar_t* filepath):Resource(filepath)
	{
		int a;

		ULONG_PTR m_gdiplusToken;
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

		Gdiplus::Bitmap* gdibitmap = new Gdiplus::Bitmap(filepath);
		if (gdibitmap->GetLastStatus() != 0)
		{
			std::stringstream msg;
			wstring file = filepath;
			msg << "Could not open " << std::string(file.begin(),file.end());
			delete gdibitmap;
			Gdiplus::GdiplusShutdown(m_gdiplusToken);
			throw VIVA_ERROR(msg.str().c_str());
		}

		UINT h = gdibitmap->GetHeight();
		UINT w = gdibitmap->GetWidth();

		HBITMAP hbitmap;
		Gdiplus::Color c(0, 0, 0);
		gdibitmap->GetHBITMAP(c, &hbitmap);

		a = gdibitmap->GetLastStatus();

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

		size = Size(w, h);
		shaderResource = SrvFromColorArray((Pixel*)data, size);

		DeleteObject(hbitmap);
		delete gdibitmap;
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
	}

	// create texture from Color array
	CTexture::CTexture(const Pixel data[], const Size& _size, const wchar_t* _name) :Resource(_name)
	{
		size = _size;
		shaderResource = SrvFromColorArray(data, _size);
	}
}