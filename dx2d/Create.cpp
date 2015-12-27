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