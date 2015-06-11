#include "Private.h"

/* HOW TO GET ID3D11TEXTURE2D and D3D11_TEXTURE2D_DESC from ID3D11ShaderResourceView
ID3D11Texture2D *pTextureInterface = 0;
ID3D11Resource *res;
shaderResource->GetResource(&res);
res->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
D3D11_TEXTURE2D_DESC desc;
pTextureInterface->GetDesc(&desc);
*/

namespace dx2d
{
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();
	void CreateStandardSampler(ID3D11SamplerState** sampler);

	CSprite::CSprite()
	{
		uncachedTex = false;
		Scale = XMFLOAT2(1, 1);
		Color = SColor(1, 1, 1, 1);
		TexFilter = DrawManager->TexFilterCreationMode;
	}

	CSprite::CSprite(const WCHAR* textureFile)
	{
		uncachedTex = false;
		vertexBuffer = nullptr;
		Scale = XMFLOAT2(1, 1);
		Color = SColor(1, 1, 1, 1);
		TexFilter = DrawManager->TexFilterCreationMode;

		//check resources first
		CTexture* res = DrawManager->resourceManager->GetTexture(textureFile);
		if (res != nullptr)
		{
			shaderResource = res->shaderResource;
		}
		else
		{
			D3D11_TEXTURE2D_DESC desc;
			auto tex = Functions::CreateTexture2DFromFile(textureFile);
			tex->GetDesc(&desc);
			GetDevice()->CreateShaderResourceView(tex, 0, &shaderResource);
			tex->Release();
			//add to resources
			CTexture* newRes = new CTexture;
			newRes->fileName = textureFile;
			newRes->Height = desc.Height;
			newRes->Width = desc.Width;
			newRes->shaderResource = shaderResource;
			DrawManager->resourceManager->AddTexture(newRes);
		}
	}

	void CSprite::Draw()
	{
		//color
		GetContext()->UpdateSubresource(cbBufferPS, 0, NULL, &Color, 0, 0);
		GetContext()->PSSetConstantBuffers(0, 1, &cbBufferPS);
		//uv
		GetContext()->UpdateSubresource(cbBufferUV, 0, NULL, uv, 0, 0);
		GetContext()->VSSetConstantBuffers(1, 1, &cbBufferUV);
		//tex
		GetContext()->PSSetShaderResources(0, 1, &shaderResource);
		//draw
		GetContext()->DrawIndexed(6, 0, 0);
	}

	XMMATRIX CSprite::GetScaleMatrix()
	{
		return DirectX::XMMatrixScaling(Scale.x, Scale.y, 1);
	}

	void CSprite::Destroy()
	{
		DrawManager->RemoveSprite(this);
		if (uncachedTex)
			shaderResource->Release();
		delete this;
	}

	CText::CText(std::wstring text)
	{		
		auto tex = Functions::CreateTexture2DFromText(text);
		GetDevice()->CreateShaderResourceView(tex, 0, &shaderResource);
		tex->Release();
	}

	CAnimation::CAnimation(const WCHAR* file, int x, int y) : CSprite(file)
	{
		frameCount = x*y;
		Start = 0;
		Finish = frameCount - 1;
		Frame = 0;
		Speed = 1;
		FrameChanged = true;
		indicator = 0;
		uvTable = new UV[frameCount];
		for (int i = 0; i < y; i++)
			for (int j = 0; j < x; j++)
				uvTable[i*x + j] = { 1.0f / x*j, 1.0f / y*i, 1.0f / x*(j + 1), 1.0f / y*(i + 1) };
		delete uv;
		uv = uvTable + Frame;
	}

	void CAnimation::Play()
	{
		if (Speed != 0)
		{
			indicator += Speed * Core->GetFrameTime();
			FrameChanged = false;
			if (indicator > 1)
			{
				indicator = 0;
				FrameChanged = true;
				NextFrame();
			}
			else if (indicator < 0)
			{
				indicator = 1;
				FrameChanged = true;
				PreviousFrame();
			}
		}
		uv = uvTable + Frame;
	}

	void CAnimation::SetOrder(int* order)
	{
		UV* newOrder = new UV[frameCount];
		for (int i = 0; i < frameCount; i++)
			newOrder[i] = uvTable[order[i]];
		delete[] uvTable;
		uvTable = newOrder;
	}

	int CAnimation::GetFrameCount()
	{
		return frameCount;
	}

	void CAnimation::NextFrame()
	{
		Frame++;
		if (Frame > Finish)
			Frame = Start;
	}

	void CAnimation::PreviousFrame()
	{
		Frame--;
		if (Frame < Start)
			Frame = Finish;
	}

	CAnimation::~CAnimation()
	{
		uv = nullptr;
		delete[] uvTable;
	}
}