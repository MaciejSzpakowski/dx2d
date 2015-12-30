#include "Viva.h"

/* HOW TO GET ID3D11TEXTURE2D and D3D11_TEXTURE2D_DESC from ID3D11ShaderResourceView
ID3D11Texture2D *pTextureInterface = 0;
ID3D11Resource *res;
shaderResource->GetResource(&res);
res->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
D3D11_TEXTURE2D_DESC desc;
pTextureInterface->GetDesc(&desc);
*/

namespace Viva
{
	Sprite::Sprite(const wchar_t* filename)
	{
		pixelShader = Core->zDefaultPS;
		flipHorizontally = false;
		flipVertically = false;
		zVertexBuffer = nullptr;
		scale = XMFLOAT2(1, 1);
		color = XMFLOAT4(1, 1, 1, 1);
		texFilter = DrawManager->TexFilterCreationMode;
		texture = DrawManager->GetTexture(filename);
		zRenderTarget = nullptr;
	}

	Sprite::Sprite(CTexture* _texture)
	{
		pixelShader = Core->zDefaultPS;
		flipHorizontally = false;
		flipVertically = false;
		zVertexBuffer = nullptr;
		scale = XMFLOAT2(1, 1);
		color = XMFLOAT4(1, 1, 1, 1);
		texFilter = DrawManager->TexFilterCreationMode;
		texture = _texture;
		zRenderTarget = nullptr;
	}

	void Sprite::_Draw()
	{
		//ps
		if(pixelShader != nullptr)
			Core->zContext->PSSetShader(pixelShader, 0, 0);
		//color		
		Core->zContext->UpdateSubresource(DrawManager->zCbBufferPS, 0, 0, &color, 0, 0);
		//uv
		Rect uv;
		uv.left = flipHorizontally ? UV.right : UV.left;
		uv.right = flipHorizontally ? UV.left : UV.right;
		uv.top = flipVertically ? UV.bottom : UV.top;
		uv.bottom = flipVertically ? UV.top : UV.bottom;
		Core->zContext->UpdateSubresource(DrawManager->zCbBufferUV, 0, 0, &uv, 0, 0);
		//extra buffer
		if(zExtraBufferPSdata != nullptr)
			Core->zContext->UpdateSubresource(DrawManager->zCbBufferPSExtra, 0, 0, zExtraBufferPSdata, 0, 0);
		
		//tex
		Core->zContext->PSSetShaderResources(0, 1, texture->_GetShaderResourceAddress());
		//draw
		Core->zContext->DrawIndexed(6, 0, 0);
	}

	void Sprite::_SpriteUpdate()
	{
		size += SizeVelocity * (float)Core->GetFrameTime();
	}

	XMMATRIX Sprite::_GetScaleMatrix()
	{
		return DirectX::XMMatrixScaling(scale.x * size, scale.y * size, 1);
	}

	void Sprite::SetPixelPerfectScale()
	{		
		SetPixelScale(texture->GetSize());
	}

	void Sprite::SetPixelScale(const Viva::Size& _size)
	{
		XMFLOAT2 frustum = Core->GetCamera()->GetFrustumSize(GetPosition().z);
		RECT client;
		GetClientRect(Core->GetWindowHandle(), &client);
		XMFLOAT2 clientSize = { (float)client.right - client.left,
			(float)client.bottom - client.top };
		XMFLOAT2 unitsPerPixel = { frustum.x / clientSize.x, frustum.y / clientSize.y };
		scale.x = unitsPerPixel.x * _size.width / 2;
		scale.y = unitsPerPixel.y * _size.height / 2;
	}

	void Sprite::_CheckForCursor(XMMATRIX transform)
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

	void Sprite::Destroy()
	{
		CDynamic::Destroy();
		if(zIndex != -1)
			DrawManager->RemoveSprite(this);
		delete this;
	}
}