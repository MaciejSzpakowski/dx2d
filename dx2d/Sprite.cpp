#include "Private.h"

namespace dx2d
{
	Sprite::Sprite(const WCHAR* textureFile)
	{
		Scale = XMFLOAT2(1, 1);
		Color = SColor(1, 1, 1, 1);
		Color.x1 = 1;
		Color.x2 = 1;
		Color.x3 = 1;
		Color.x4 = 1;
		VERTEX v[] =
		{
			{ -1.0f, -1.0f, 0, 0, 0, 0, 0, 0 },
			{ 1.0f, -1.0f, 0, 0, 0, 0, 1, 0 },
			{ 1.0f, 1.0f, 0, 0, 0, 0, 1, 1 },
			{ -1.0f, 1.0f, 0, 0, 0, 0, 0, 1 },
		};
		int indices[] = { 0, 1, 2, 0, 2, 3, };

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(int) * 6;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA srd;
		srd.pSysMem = indices;
		Device->CreateBuffer(&indexBufferDesc, &srd, &indexBuffer);

		Context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VERTEX) * 4;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = v;
		Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer);
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
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		Context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
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
		VERTEX v[] =
		{
			{ -1.0f, -1.0f, 0, 0, 0, 0, 0, 0 },
			{ 1.0f, -1.0f, 0, 0, 0, 0, 1, 0 },
			{ 1.0f, 1.0f, 0, 0, 0, 0, 1, 1 },
			{ -1.0f, 1.0f, 0, 0, 0, 0, 0, 1 },
		};
		int indices[] = { 0, 1, 2, 0, 2, 3, };

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(int) * 6;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA srd;
		srd.pSysMem = indices;
		Device->CreateBuffer(&indexBufferDesc, &srd, &indexBuffer);

		Context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VERTEX) * 4;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = v;
		Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer);
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