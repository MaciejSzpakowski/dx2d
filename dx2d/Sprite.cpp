#include "Viva.h"

/* HOW TO GET ID3D11TEXTURE2D and D3D11_TEXTURE2D_DESC from ID3D11ShaderResourceView
ID3D11Texture2D *pTextureInterface = 0;
ID3D11Resource *res;
zShaderResource->GetResource(&res);
res->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
D3D11_TEXTURE2D_DESC desc;
pTextureInterface->GetDesc(&desc);
*/

namespace Viva
{
	CSprite::CSprite()
	{
		PixelShader = Core->zDefaultPS;
		FlipHorizontally = false;
		FlipVertically = false;
		zVertexBuffer = nullptr;
		Scale = XMFLOAT2(1, 1);
		color = XMFLOAT4(1, 1, 1, 1);
		TexFilter = DrawManager->TexFilterCreationMode;
		zRenderTarget = nullptr;
	}

	CSprite::CSprite(LPCWSTR file)
	{
		PixelShader = Core->zDefaultPS;
		FlipHorizontally = false;
		FlipVertically = false;
		zVertexBuffer = nullptr;
		Scale = XMFLOAT2(1, 1);
		color = XMFLOAT4(1, 1, 1, 1);
		TexFilter = DrawManager->TexFilterCreationMode;
		zTexture = DrawManager->GetTexture(file);
		zShaderResource = zTexture->zShaderResource;
		zRenderTarget = nullptr;
	}

	CSprite::CSprite(CTexture* texture)
	{
		PixelShader = Core->zDefaultPS;
		FlipHorizontally = false;
		FlipVertically = false;
		zVertexBuffer = nullptr;
		Scale = XMFLOAT2(1, 1);
		color = XMFLOAT4(1, 1, 1, 1);
		TexFilter = DrawManager->TexFilterCreationMode;
		zTexture = texture;
		zShaderResource = texture->zShaderResource;
		zRenderTarget = nullptr;
	}

	void CSprite::_Draw()
	{
		//ps
		if(PixelShader != nullptr)
			Core->zContext->PSSetShader(PixelShader, 0, 0);
		//color		
		Core->zContext->UpdateSubresource(DrawManager->zCbBufferPS, 0, 0, &color, 0, 0);
		//uv
		Rect uv;
		uv.left = FlipHorizontally ? UV.right : UV.left;
		uv.right = FlipHorizontally ? UV.left : UV.right;
		uv.top = FlipVertically ? UV.bottom : UV.top;
		uv.bottom = FlipVertically ? UV.top : UV.bottom;
		Core->zContext->UpdateSubresource(DrawManager->zCbBufferUV, 0, 0, &uv, 0, 0);
		//extra buffer
		if(zExtraBufferPSdata != nullptr)
			Core->zContext->UpdateSubresource(DrawManager->zCbBufferPSExtra, 0, 0, zExtraBufferPSdata, 0, 0);
		
		//tex
		Core->zContext->PSSetShaderResources(0, 1, &zShaderResource);
		//draw
		Core->zContext->DrawIndexed(6, 0, 0);
	}

	void CSprite::zSpriteUpdate()
	{
		size += SizeVelocity * (float)Core->GetFrameTime();
	}

	XMMATRIX CSprite::_GetScaleMatrix()
	{
		return DirectX::XMMatrixScaling(Scale.x * size, Scale.y * size, 1);
	}

	void CSprite::SetNaturalScale()
	{
		CTexture* tex = GetTexture();
		
		SetPixelScale(Viva::Size(tex->size.width, tex->size.height));
	}

	void CSprite::SetPixelScale(const Viva::Size& _size)
	{
		XMFLOAT2 frustum = Core->GetCamera()->GetFrustumSize(GetPosition().z);
		RECT client;
		GetClientRect(Core->GetWindowHandle(), &client);
		XMFLOAT2 clientSize = { (float)client.right - client.left,
			(float)client.bottom - client.top };
		XMFLOAT2 unitsPerPixel = { frustum.x / clientSize.x, frustum.y / clientSize.y };
		Scale.x = unitsPerPixel.x * _size.width / 2;
		Scale.y = unitsPerPixel.y * _size.height / 2;
	}

	void CSprite::zCheckForCursor(XMMATRIX transform)
	{
		XMFLOAT3 pf = Core->GetCamera()->GetCursorWorldPos(GetPosition().z);
		XMVECTOR A = DirectX::XMVectorSet(-1.0f, -1.0f, 0, 1);
		XMVECTOR B = DirectX::XMVectorSet(1.0f, -1.0f, 0, 1);
		XMVECTOR C = DirectX::XMVectorSet(1.0f, 1.0f, 0, 1);
		XMVECTOR D = DirectX::XMVectorSet(-1.0f, 1.0f, 0, 1);
		A = DirectX::XMVector3Transform(A, transform);
		B = DirectX::XMVector3Transform(B, transform);
		C = DirectX::XMVector3Transform(C, transform);
		D = DirectX::XMVector3Transform(D, transform);

		XMVECTOR origin = DirectX::XMVectorSet(pf.x, pf.y, Core->GetCamera()->GetPosition().z, 0);
		XMVECTOR dir = DirectX::XMVectorSet(0, 0, 1, 0);
		float dist;
		//ray - triangle collision
		zUnderCursor = DirectX::TriangleTests::Intersects(origin, dir, A, B, C, dist);
		//if cursor is not over 1st triangle, try the second one
		if(!zUnderCursor)
			zUnderCursor = DirectX::TriangleTests::Intersects(origin, dir, A, C, D, dist);
	}

	void CSprite::Destroy()
	{
		CDynamic::Destroy();
		DrawManager->RemoveSprite(this);
		delete this;
	}
}