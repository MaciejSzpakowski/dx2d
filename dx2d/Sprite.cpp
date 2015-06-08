#include "Private.h"

namespace dx2d
{
	Sprite::Sprite(const WCHAR* textureFile)
	{
		vertexBuffer = nullptr;
		Scale = XMFLOAT2(1, 1);
		Color = SColor(1, 1, 1, 1);
		Color.x1 = 1;
		Color.x2 = 1;
		Color.x3 = 1;
		Color.x4 = 1;		

		auto tex = Global->CreateTexture2D(textureFile);
		Device->CreateShaderResourceView(tex, 0, &shaderResource);
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
		Device->CreateSamplerState(&sampDesc, &samplerState);
	}

	void Sprite::Draw()
	{
		Context->UpdateSubresource(cbBufferPS, 0, NULL, &Color, 0, 0);
		Context->PSSetConstantBuffers(0, 1, &cbBufferPS);		
		Context->PSSetShaderResources(0, 1, &shaderResource);
		Context->PSSetSamplers(0, 1, &samplerState);
		Context->DrawIndexed(6, 0, 0);
	}

	XMMATRIX Sprite::GetScaleMatrix()
	{
		return DirectX::XMMatrixScaling(Scale.x, Scale.y, 1);
	}

	void Sprite::Destroy()
	{
		DrawManager->RemoveSprite(this);
		shaderResource->Release();
		samplerState->Release();
		delete this;
	}

	Text::Text(std::wstring text)
	{
		Scale = XMFLOAT2(1, 1);
		Color = SColor(1, 1, 1, 1);
		Color.x1 = 1;
		Color.x2 = 1;
		Color.x3 = 1;
		Color.x4 = 1;
		
		auto tex = Global->CreateTexture2DFromText(text);
		Device->CreateShaderResourceView(tex, 0, &shaderResource);
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
		Device->CreateSamplerState(&sampDesc, &samplerState);
	}
}