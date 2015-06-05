#include "Private.h"

namespace dx2d
{
	Sprite::Sprite(const char* texture)
	{
		HRESULT r1;
		/*index = -1;
		shaderResource = nullptr;
		//experiment
		unsigned char *texArray = new unsigned char[4 * 32 * 32];

		for (int i = 0; i < 32 * 32; i+=4)
		{
			texArray[i] = 255;
			texArray[i+1] = 255;
			texArray[i+2] = 255;
			texArray[i+3] = 255;
		}			

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = 1;
		desc.Height = 1;
		desc.MipLevels = desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		ID3D11Texture2D *pTexture1 = NULL;

		int ia = -1;
		D3D11_SUBRESOURCE_DATA boxTexInitData;
		ZeroMemory(&boxTexInitData, sizeof(D3D11_SUBRESOURCE_DATA));
		boxTexInitData.pSysMem = &ia;
		
		HRESULT r1 = Device->CreateTexture2D(&desc, &boxTexInitData, &pTexture1);
		delete[] texArray;*/

		ID3D11Texture2D *tex;
		D3D11_TEXTURE2D_DESC tdesc;
		D3D11_SUBRESOURCE_DATA tbsd;

		int *buf = new int[32*32];

		for (int i = 0; i<32; i++)
			for (int j = 0; j<32; j++)
			{
				if ((i & 32) == (j & 32))
					buf[i*32 + j] = 0x00000000;
				else
					buf[i*32 + j] = 0xffffffff;
			}

		tbsd.pSysMem = (void *)buf;
		tbsd.SysMemPitch = 32 * 4;
		tbsd.SysMemSlicePitch = 32*32 * 4; // Not needed since this is a 2d texture

		tdesc.Width = 32;
		tdesc.Height = 32;
		tdesc.MipLevels = 1;
		tdesc.ArraySize = 1;

		tdesc.SampleDesc.Count = 1;
		tdesc.SampleDesc.Quality = 0;
		tdesc.Usage = D3D11_USAGE_DEFAULT;
		tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		tdesc.CPUAccessFlags = 0;
		tdesc.MiscFlags = 0;

		r1 = Device->CreateTexture2D(&tdesc, &tbsd, &tex);


		delete[] buf;

		r1 = Device->CreateShaderResourceView(tex, 0, &shaderResource);
		
		//ends here
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