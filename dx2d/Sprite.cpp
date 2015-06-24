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
		Color = XMFLOAT4(1, 1, 1, 1);
		TexFilter = DrawManager->TexFilterCreationMode;
	}

	CSprite::CSprite(const WCHAR* file)
	{
		resource = file;
		FlipHorizontally = false;
		FlipVertically = false;
		uncachedTex = false;
		vertexBuffer = nullptr;
		Scale = XMFLOAT2(1, 1);
		Color = XMFLOAT4(1, 1, 1, 1);
		TexFilter = DrawManager->TexFilterCreationMode;

		Functions::LoadCachedTextureFromFile(file, shaderResource);		
	}

	CSprite::CSprite(CTexture* texture)
	{
		resource = texture->name;
		FlipHorizontally = false;
		FlipVertically = false;
		uncachedTex = false;
		vertexBuffer = nullptr;
		Scale = XMFLOAT2(1, 1);
		Color = XMFLOAT4(1, 1, 1, 1);
		TexFilter = DrawManager->TexFilterCreationMode;
		shaderResource = texture->shaderResource;
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
		return XMMatrixScaling(Scale.x, Scale.y, 1);
	}

	CTexture* CSprite::GetTexture()
	{
		return ResourceManager->GetTexture(resource);
	}

	void CSprite::SetNaturalScale()
	{
		CTexture* tex = GetTexture();
		SetPixelScale(tex->Width, tex->Height);
	}

	void CSprite::SetPixelScale(int width, int height)
	{
		POINTF frustum = Camera->GetFrustumSize(GetPosition().z);
		RECT client;
		GetClientRect(Core->GetWindowHandle(), &client);
		POINTF clientSize = { (float)client.right - client.left, (float)client.bottom - client.top };
		POINTF unitsPerPixel = { frustum.x / clientSize.x, frustum.y / clientSize.y };
		Scale.x = unitsPerPixel.x * width / 2;
		Scale.y = unitsPerPixel.y * height / 2;
	}

	void CSprite::CheckForCursor(XMMATRIX transform)
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
		underCursor = (u >= 0) && (v >= 0) && (u + v < 1);
		if (underCursor)
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
		underCursor = (u >= 0) && (v >= 0) && (u + v < 1);
	}

	void CSprite::Destroy()
	{
		DrawManager->RemoveSprite(this);
		if (uncachedTex)
			shaderResource->Release();
		delete this;
	}

	CAnimation::CAnimation(const WCHAR* file, int x, int y) : CSprite(file)
	{
		frameCount = x*y;
		Start = 0;
		Finish = frameCount - 1;
		Frame = 0;
		Speed = 1;
		frameChanged = true;
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
			frameChanged = false;
			if (indicator > 1)
			{
				indicator = 0;
				frameChanged = true;
				NextFrame();
			}
			else if (indicator < 0)
			{
				indicator = 1;
				frameChanged = true;
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

	bool CAnimation::FrameChanged()
	{
		return frameChanged;
	}
}