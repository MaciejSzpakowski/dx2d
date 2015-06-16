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
		FlipHorizontally = false;
		FlipVertically = false;
		uncachedTex = false;
		Scale = XMFLOAT2(1, 1);
		Color = SColor(1, 1, 1, 1);
		TexFilter = DrawManager->TexFilterCreationMode;
	}

	CSprite::CSprite(const WCHAR* textureFile)
	{
		FlipHorizontally = false;
		FlipVertically = false;
		uncachedTex = false;
		vertexBuffer = nullptr;
		Scale = XMFLOAT2(1, 1);
		Color = SColor(1, 1, 1, 1);
		TexFilter = DrawManager->TexFilterCreationMode;

		Functions::LoadCachedTexture(textureFile, shaderResource);
		
	}

	void CSprite::Draw()
	{
		//color
		GetContext()->UpdateSubresource(cbBufferPS, 0, NULL, &Color, 0, 0);
		GetContext()->PSSetConstantBuffers(0, 1, &cbBufferPS);
		//uv
		UV _uv;
		_uv.left = FlipHorizontally ? uv.right : uv.left;
		_uv.right = FlipHorizontally ? uv.left : uv.right;
		_uv.top = FlipVertically ? uv.bottom : uv.top;
		_uv.bottom = FlipVertically ? uv.top : uv.bottom;
		GetContext()->UpdateSubresource(cbBufferUV, 0, NULL, &_uv, 0, 0);
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
		uncachedTex = true;
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
		uvTable.reserve(frameCount);
		for (int i = 0; i < y; i++)
			for (int j = 0; j < x; j++)
				uvTable.push_back(UV(1.0f / x*j, 1.0f / y*i, 1.0f / x*(j + 1), 1.0f / y*(i + 1)));
		uv = uvTable[Frame];
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
		uv = uvTable[Frame];
	}

	void CAnimation::SetOrder(int* order)
	{
		vector<UV> newUvtable;
		newUvtable.reserve(frameCount);
		for (int i = 0; i < frameCount; i++)
			newUvtable[i] = uvTable[order[i]];
		uvTable = newUvtable;
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
}