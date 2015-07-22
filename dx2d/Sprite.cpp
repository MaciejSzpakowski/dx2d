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
	CSprite::CSprite()
	{
		PixelShader = Core->zDefaultPS;
		FlipHorizontally = false;
		FlipVertically = false;
		zVertexBuffer = nullptr;
		Scale = XMFLOAT2(1, 1);
		Color = XMFLOAT4(1, 1, 1, 1);
		TexFilter = DrawManager->TexFilterCreationMode;
	}

	CSprite::CSprite(LPCWSTR file)
	{
		PixelShader = Core->zDefaultPS;
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
		PixelShader = Core->zDefaultPS;
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
		//ps
		if(PixelShader != nullptr)
			Core->zContext->PSSetShader(PixelShader, 0, 0);
		//color		
		Core->zContext->UpdateSubresource(zCbBufferPS, 0, 0, &Color, 0, 0);
		Core->zContext->PSSetConstantBuffers(0, 1, &zCbBufferPS);
		//uv
		Rect uv;
		uv.left = FlipHorizontally ? UV.right : UV.left;
		uv.right = FlipHorizontally ? UV.left : UV.right;
		uv.top = FlipVertically ? UV.bottom : UV.top;
		uv.bottom = FlipVertically ? UV.top : UV.bottom;
		Core->zContext->UpdateSubresource(zCbBufferUV, 0, 0, &uv, 0, 0);
		Core->zContext->VSSetConstantBuffers(1, 1, &zCbBufferUV);
		//tex
		Core->zContext->PSSetShaderResources(0, 1, &zShaderResource);
		//draw
		Core->zContext->DrawIndexed(6, 0, 0);
	}

	void CSprite::zSpriteUpdate()
	{
		Size += SizeVel * (float)Core->GetFrameTime();
	}

	XMMATRIX CSprite::zGetScaleMatrix()
	{
		return XMMatrixScaling(Scale.x * Size, Scale.y * Size, 1);
	}

	void CSprite::SetNaturalScale()
	{
		CTexture* tex = GetTexture();
		SetPixelScale(tex->zWidth, tex->zHeight);
	}

	void CSprite::SetPixelScale(int width, int height)
	{
		XMFLOAT2 frustum = Camera->GetFrustumSize(GetPosition().z);
		RECT client;
		GetClientRect(Core->GetWindowHandle(), &client);
		XMFLOAT2 clientSize = { (float)client.right - client.left,
			(float)client.bottom - client.top };
		XMFLOAT2 unitsPerPixel = { frustum.x / clientSize.x, frustum.y / clientSize.y };
		Scale.x = unitsPerPixel.x * width / 2;
		Scale.y = unitsPerPixel.y * height / 2;
	}

	void CSprite::zCheckForCursor(XMMATRIX transform)
	{
		XMFLOAT3 pf = Camera->GetCursorWorldPos(GetPosition().z);
		XMVECTOR A = XMVectorSet(-1.0f, -1.0f, 0, 1);
		XMVECTOR B = XMVectorSet(1.0f, -1.0f, 0, 1);
		XMVECTOR C = XMVectorSet(1.0f, 1.0f, 0, 1);
		XMVECTOR D = XMVectorSet(-1.0f, 1.0f, 0, 1);		
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
		delete this;
	}

	CAnimation::CAnimation(LPCWSTR file, int x, int y) : CSprite(file)
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
				zUvTable.push_back(Rect(1.0f / x*j, 1.0f / y*i, 1.0f / 
				x*(j + 1), 1.0f / y*(i + 1)));
		UV = zUvTable[Frame];
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
		UV = zUvTable[Frame];
	}

	void CAnimation::SetOrder(int order[])
	{
		vector<Rect> newUvtable;
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