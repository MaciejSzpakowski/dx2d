#include "Private.h"

ID3D11Device* GetDevice();
ID3D11DeviceContext* GetContext();

namespace dx2d
{
	CSprite::CSprite(const WCHAR* textureFile)
	{
		vertexBuffer = nullptr;
		Scale = XMFLOAT2(1, 1);
		Color = SColor(1, 1, 1, 1);

		auto tex = Functions::CreateTexture2D(textureFile);
		GetDevice()->CreateShaderResourceView(tex, 0, &shaderResource);
		tex->Release();

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		GetDevice()->CreateSamplerState(&sampDesc, &samplerState);
	}

	void CSprite::Draw()
	{
		GetContext()->UpdateSubresource(cbBufferPS, 0, NULL, &Color, 0, 0);
		GetContext()->PSSetConstantBuffers(0, 1, &cbBufferPS);
		GetContext()->PSSetShaderResources(0, 1, &shaderResource);
		GetContext()->PSSetSamplers(0, 1, &samplerState);
		GetContext()->DrawIndexed(6, 0, 0);
	}

	XMMATRIX CSprite::GetScaleMatrix()
	{
		return DirectX::XMMatrixScaling(Scale.x, Scale.y, 1);
	}

	void CSprite::Destroy()
	{
		DrawManager->RemoveSprite(this);
		shaderResource->Release();
		samplerState->Release();
		delete this;
	}

	CText::CText(std::wstring text)
	{
		Scale = XMFLOAT2(1, 1);
		Color = SColor(1, 1, 1, 1);
		
		auto tex = Functions::CreateTexture2DFromText(text);
		GetDevice()->CreateShaderResourceView(tex, 0, &shaderResource);
		tex->Release();

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		GetDevice()->CreateSamplerState(&sampDesc, &samplerState);
	}
}