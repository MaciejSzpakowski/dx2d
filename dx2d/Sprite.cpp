#include "Private.h"

/* HOW TO GET ID3D11TEXTURE2D and D3D11_TEXTURE2D_DESC from ID3D11ShaderResourceView
ID3D11Texture2D *pTextureInterface = 0;
ID3D11Resource *res;
zShaderResource->GetResource(&res);
res->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
D3D11_TEXTURE2D_DESC desc;
pTextureInterface->GetDesc(&desc);
*/

namespace dx2d
{
	void CreateStandardSampler(ID3D11SamplerState** sampler);

	CSprite::CSprite()
	{
		FlipHorizontally = false;
		FlipVertically = false;
		Scale = XMFLOAT2(1, 1);
		Color = XMFLOAT4(1, 1, 1, 1);
		TexFilter = DrawManager->TexFilterCreationMode;
	}

	CSprite::CSprite(const WCHAR* file)
	{
		FlipHorizontally = false;
		FlipVertically = false;
		zVertexBuffer = nullptr;
		Scale = XMFLOAT2(1, 1);
		Color = XMFLOAT4(1, 1, 1, 1);
		TexFilter = DrawManager->TexFilterCreationMode;
		zTexture = Functions::GetCachedTextureFromFile(file);
		zShaderResource = zTexture->zShaderResource;
	}

	CSprite::CSprite(CTexture* texture)
	{
		FlipHorizontally = false;
		FlipVertically = false;
		zVertexBuffer = nullptr;
		Scale = XMFLOAT2(1, 1);
		Color = XMFLOAT4(1, 1, 1, 1);
		TexFilter = DrawManager->TexFilterCreationMode;
		zTexture = texture;
		zShaderResource = texture->zShaderResource;
	}

	void CSprite::zDraw()
	{
		//color
		Core->zContext->UpdateSubresource(zCbBufferPS, 0, NULL, &Color, 0, 0);
		Core->zContext->PSSetConstantBuffers(0, 1, &zCbBufferPS);
		//uv
		UV _uv;
		_uv.left = FlipHorizontally ? uv.right : uv.left;
		_uv.right = FlipHorizontally ? uv.left : uv.right;
		_uv.top = FlipVertically ? uv.bottom : uv.top;
		_uv.bottom = FlipVertically ? uv.top : uv.bottom;
		Core->zContext->UpdateSubresource(zCbBufferUV, 0, NULL, &_uv, 0, 0);
		Core->zContext->VSSetConstantBuffers(1, 1, &zCbBufferUV);
		//tex
		Core->zContext->PSSetShaderResources(0, 1, &zShaderResource);
		//draw
		Core->zContext->DrawIndexed(6, 0, 0);
	}

	XMMATRIX CSprite::zGetScaleMatrix()
	{
		return XMMatrixScaling(Scale.x, Scale.y, 1);
	}

	void CSprite::SetNaturalScale()
	{
		CTexture* tex = GetTexture();
		SetPixelScale(tex->zWidth, tex->zHeight);
	}

	void CSprite::SetPixelScale(int width, int height)
	{
		POINTF frustum = Camera->GetFrustumSize(GetPosition().z);
		RECT client;
		GetClientRect(Core->GetWindowHandle(), &client);
		POINTF clientSize = { (float)client.right - client.left,
			(float)client.bottom - client.top };
		POINTF unitsPerPixel = { frustum.x / clientSize.x, frustum.y / clientSize.y };
		Scale.x = unitsPerPixel.x * width / 2;
		Scale.y = unitsPerPixel.y * height / 2;
	}

	void CSprite::zCheckForCursor(XMMATRIX transform)
	{
		XMFLOAT3 pf = Camera->GetCursorWorldPos(GetPosition().z);
		XMVECTOR A = XMVectorSet(-1.0f, -1.0f, 0, 0);
		XMVECTOR B = XMVectorSet(1.0f, -1.0f, 0, 0);
		XMVECTOR C = XMVectorSet(1.0f, 1.0f, 0, 0);
		XMVECTOR D = XMVectorSet(-1.0f, 1.0f, 0, 0);		
		A = XMVector3Transform(A, transform);
		B = XMVector3Transform(B, transform);
		C = XMVector3Transform(C, transform);
		D = XMVector3Transform(D, transform);

		XMVECTOR origin = XMVectorSet(pf.x, pf.y, Camera->GetPosition().z, 0);
		XMVECTOR dir = XMVectorSet(0, 0, 1, 0);
		float dist;
		//ray - triangle collision
		zUnderCursor = TriangleTests::Intersects(origin, dir, A, B, C, dist);
		//if cursor is not over 1st triangle, try the second one
		if(!zUnderCursor)
			zUnderCursor = TriangleTests::Intersects(origin, dir, A, C, D, dist);
	}

	void CSprite::Destroy()
	{
		DrawManager->RemoveSprite(this);
		if (!zTexture->zCached)
			zTexture->Destroy();
		delete this;
	}

	CAnimation::CAnimation(const WCHAR* file, int x, int y) : CSprite(file)
	{
		zFrameCount = x*y;
		Start = 0;
		Finish = zFrameCount - 1;
		Frame = 0;
		Speed = 1;
		zFrameChanged = true;
		zIndicator = 0;
		zUvTable.reserve(zFrameCount);
		for (int i = 0; i < y; i++)
			for (int j = 0; j < x; j++)
				zUvTable.push_back(UV(1.0f / x*j, 1.0f / y*i, 1.0f / 
				x*(j + 1), 1.0f / y*(i + 1)));
		uv = zUvTable[Frame];
	}

	void CAnimation::zPlay()
	{
		if (Speed != 0)
		{
			zIndicator += Speed * Core->GetFrameTime();
			zFrameChanged = false;
			if (zIndicator > 1)
			{
				zIndicator = 0;
				zFrameChanged = true;
				NextFrame();
			}
			else if (zIndicator < 0)
			{
				zIndicator = 1;
				zFrameChanged = true;
				PreviousFrame();
			}
		}
		uv = zUvTable[Frame];
	}

	void CAnimation::SetOrder(int* order)
	{
		vector<UV> newUvtable;
		newUvtable.reserve(zFrameCount);
		for (int i = 0; i < zFrameCount; i++)
			newUvtable[i] = zUvTable[order[i]];
		zUvTable = newUvtable;
	}

	int CAnimation::GetFrameCount()
	{
		return zFrameCount;
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

	bool CAnimation::FrameChanged()
	{
		return zFrameChanged;
	}
}