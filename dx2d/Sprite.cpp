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
		zUncachedTex = false;
		Scale = XMFLOAT2(1, 1);
		Color = XMFLOAT4(1, 1, 1, 1);
		TexFilter = DrawManager->TexFilterCreationMode;
	}

	CSprite::CSprite(const WCHAR* file)
	{
		zResource = file;
		FlipHorizontally = false;
		FlipVertically = false;
		zUncachedTex = false;
		zVertexBuffer = nullptr;
		Scale = XMFLOAT2(1, 1);
		Color = XMFLOAT4(1, 1, 1, 1);
		TexFilter = DrawManager->TexFilterCreationMode;

		Functions::LoadCachedTextureFromFile(file, zShaderResource);		
	}

	CSprite::CSprite(CTexture* texture)
	{
		zResource = texture->zName;
		FlipHorizontally = false;
		FlipVertically = false;
		zUncachedTex = false;
		zVertexBuffer = nullptr;
		Scale = XMFLOAT2(1, 1);
		Color = XMFLOAT4(1, 1, 1, 1);
		TexFilter = DrawManager->TexFilterCreationMode;
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

	CTexture* CSprite::GetTexture()
	{
		return ResourceManager->GetTexture(zResource);
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
		POINTF pf = Camera->GetCursorWorldPos(GetPosition().z);
		XMVECTOR A = XMVectorSet(-1.0f, -1.0f, 0, 1);
		XMVECTOR B = XMVectorSet(1.0f, -1.0f, 0, 1);
		XMVECTOR C = XMVectorSet(1.0f, 1.0f, 0, 1);
		XMVECTOR D = XMVectorSet(-1.0f, 1.0f, 0, 1);
		XMVECTOR P = XMVectorSet(pf.x, pf.y, GetPosition().z, 1);
		A = XMVector3Transform(A, transform);
		B = XMVector3Transform(B, transform);
		C = XMVector3Transform(C, transform);
		D = XMVector3Transform(D, transform);

		// FIRST TRIANGLE
		//// Compute vectors        
		XMVECTOR v0 = XMVectorSubtract(C, A);
		XMVECTOR v1 = XMVectorSubtract(B, A);
		XMVECTOR v2 = XMVectorSubtract(P, A);
		// Compute dot products
		XMVECTOR vdot00 = XMVector3Dot(v0, v0);
		XMVECTOR vdot01 = XMVector3Dot(v0, v1);
		XMVECTOR vdot02 = XMVector3Dot(v0, v2);
		XMVECTOR vdot11 = XMVector3Dot(v1, v1);
		XMVECTOR vdot12 = XMVector3Dot(v1, v2);
		float dot00 = XMVectorGetX(vdot00);
		float dot01 = XMVectorGetX(vdot01);
		float dot02 = XMVectorGetX(vdot02);
		float dot11 = XMVectorGetX(vdot11);
		float dot12 = XMVectorGetX(vdot12);
		// Compute barycentric coordinates
		float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
		float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
		// Check if point is in triangle
		zUnderCursor = (u >= 0) && (v >= 0) && (u + v < 1);
		if (zUnderCursor)
			return;

		// SECOND TRIANGLE
		//// Compute vectors
		//v0 = XMVectorSubtract(C, A);
		v1 = XMVectorSubtract(D, A);
		//v2 = XMVectorSubtract(P, A);
		// Compute dot products
		//vdot00 = XMVector3Dot(v0, v0);
		vdot01 = XMVector3Dot(v0, v1);
		//vdot02 = XMVector3Dot(v0, v2);
		vdot11 = XMVector3Dot(v1, v1);
		vdot12 = XMVector3Dot(v1, v2);
		//dot00 = XMVectorGetX(vdot00);
		dot01 = XMVectorGetX(vdot01);
		//dot02 = XMVectorGetX(vdot02);
		dot11 = XMVectorGetX(vdot11);
		dot12 = XMVectorGetX(vdot12);
		// Compute barycentric coordinates
		invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
		u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		v = (dot00 * dot12 - dot01 * dot02) * invDenom;
		// Check if point is in triangle
		zUnderCursor = (u >= 0) && (v >= 0) && (u + v < 1);
	}

	void CSprite::Destroy()
	{
		DrawManager->RemoveSprite(this);
		if (zUncachedTex)
			zShaderResource->Release();
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