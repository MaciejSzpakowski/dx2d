#include "Viva.h"

namespace Viva
{
	ID3D11PixelShader* CCore::CreatePixelShaderFromFile(const wchar_t* filepath, const char* entryPoint, const char* target)
	{
		ID3D11PixelShader* result;
		ID3D10Blob *ps;
		HRESULT hr = D3DCompileFromFile(filepath, 0, 0, entryPoint, target, 0, 0, &ps, 0); CHECKHR();
		//D3DCompile
		hr = zDevice->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), 0,
			&result); CHECKHR();
		ps->Release();
		return result;
	}

	// create pixel shader from const char*
	ID3D11PixelShader* CCore::CreatePixelShaderFromString(const char* str, const char* entryPoint, const char* target)
	{
		ID3D11PixelShader* result;

		ID3D10Blob *ps;
		HRESULT hr = D3DCompile(str, strlen(str), 0, 0, 0, entryPoint, target, 0, 0, &ps, 0); CHECKHR();
		//D3DCompile
		hr = zDevice->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), 0,
			&result); CHECKHR();
		ps->Release();
		return result;
	}

	// create render target
	CRenderTarget* CCore::CreateRenderTarget()
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

		ID3D11Texture2D* tex;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = (UINT)clientSize.width;
		textureDesc.Height = (UINT)clientSize.height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		HRESULT hr = zDevice->CreateTexture2D(&textureDesc, NULL, &tex); CHECKHR();

		ID3D11RenderTargetView* rtv;
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		hr = zDevice->CreateRenderTargetView(tex,
			&renderTargetViewDesc, &rtv); CHECKHR();

		ID3D11ShaderResourceView* srv;
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		hr = zDevice->CreateShaderResourceView(tex,
			&shaderResourceViewDesc, &srv); CHECKHR();

		CRenderTarget* target = new CRenderTarget(zDefaultPost);
		target->zTexture = tex;
		target->zTargetView = rtv;
		target->zSprite = new CSprite();
		target->zSprite->FlipVertically = true;
		target->zSprite->Pickable = false;
		target->zSprite->zTexture = nullptr;
		target->zSprite->PixelShader = nullptr;
		target->zSprite->zShaderResource = srv;

		return target;
	}

	// create texture from file
	CTexture* CCore::CreateTexture(const wchar_t* filepath)
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
	CTexture* CCore::CreateTexture(const Color data[], const Size& size, const wchar_t* name)
	{
		ID3D11Texture2D *tex;
		D3D11_TEXTURE2D_DESC tdesc;
		D3D11_SUBRESOURCE_DATA tbsd;

		tbsd.pSysMem = (void *)data;
		tbsd.SysMemPitch = (UINT)size.width * 4;
		tbsd.SysMemSlicePitch = (UINT)size.height*(UINT)size.width * 4;

		tdesc.Width = (UINT)size.width;
		tdesc.Height = (UINT)size.height;
		tdesc.MipLevels = 1;
		tdesc.ArraySize = 1;

		tdesc.SampleDesc.Count = 1;
		tdesc.SampleDesc.Quality = 0;
		tdesc.Usage = D3D11_USAGE_DEFAULT;
		tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		tdesc.CPUAccessFlags = 0;
		tdesc.MiscFlags = 0;

		HRESULT hr = zDevice->CreateTexture2D(&tdesc, &tbsd, &tex); CHECKHR();

		D3D11_TEXTURE2D_DESC desc;
		tex->GetDesc(&desc);
		ID3D11ShaderResourceView* srv;
		hr = zDevice->CreateShaderResourceView(tex, 0, &srv); CHECKHR();
		tex->Release();

		return new CTexture(false, size, name, srv);
	}

	//// create sprite from file
	CSprite* CCore::CreateSprite(const wchar_t* filepath)
	{
		return nullptr;
	}

	//// create sprite from texture
	CSprite* CCore::CreateSprite(CTexture* texture)
	{
		return nullptr;
	}

	// create polygon from list of points
	CPolygon* CCore::CreatePolygon(const vector<Point>& points)
	{
		return nullptr;
	}
}