#include "Private.h"

namespace dx2d
{
	Sprite::Sprite(const char* texture)
	{
		index = -1;
		//D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"braynzar.jpg",
		//	NULL, NULL, &CubesTexture, NULL);
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

		vertexCount = 4;
		Scale.x = 1;
		Scale.y = 1;
		//method 2
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;				   // GPU writes and reads
		bd.ByteWidth = sizeof(VERTEX) * vertexCount;   // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = 0;		                   // CPU does nothing

		VERTEX vertices[] =
		{
			{ -0.5f, -0.5f, 1, 1, 1, 0, 1 },
			{ 0.5f, -0.5f, 1, 1, 1, 1, 1 },
			{ -0.5f, 0.5f, 1, 1, 1, 0, 0 },
			{ 0.5f, 0.5f, 1, 1, 1, 1, 0 }
		};

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = &vertices;                   //Memory in CPU to copy in to GPU

		Device->CreateBuffer(&bd, &sd, &vertexBuffer);
	}

	void Sprite::Draw()
	{
		Context->UpdateSubresource(cbPerObjectBuffer4, 0, NULL, &Color, 0, 0);
		Context->PSSetConstantBuffers(0, 1, &cbPerObjectBuffer4);
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		Context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		Context->PSSetShaderResources(0, 1, &shaderResource);
		Context->PSSetSamplers(0, 1, &samplerState);
		Context->Draw(vertexCount, 0);
	}

	XMMATRIX Sprite::GetScaleMatrix()
	{
		return DirectX::XMMatrixScaling(Scale.x, Scale.y, 1);
	}

	void Sprite::Destroy()
	{
		DrawManager->Remove(this);
		if(shaderResource != nullptr)
			shaderResource->Release();
		samplerState->Release();
		delete this;
	}
}