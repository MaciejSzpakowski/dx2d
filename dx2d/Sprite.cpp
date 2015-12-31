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
		:Sprite(DrawManager->GetTexture(filename), DrawManager->GetDefaultTextureFilter()) {}

	Sprite::Sprite(Texture* _texture) 
		:Sprite(_texture, DrawManager->GetDefaultTextureFilter()) {}

	Sprite::Sprite(Texture* _texture, TextureFilter _texFilter)
	{
		pixelShader = Core->_GetDefaultPS();
		flipHorizontally = false;
		flipVertically = false;
		scale = XMFLOAT2(1, 1);
		color = Color(1, 1, 1, 1);
		texFilter = _texFilter;
		texture = _texture;
	}

	void Sprite::_Draw()
	{
		//ps
		if(pixelShader != nullptr)
			Core->_GetContext()->PSSetShader(pixelShader, 0, 0);
		//color		
		Core->_GetContext()->UpdateSubresource(DrawManager->_GetConstantBufferPS(), 0, 0, &color, 0, 0);
		//uv
		Rect finaluv;
		finaluv.left = flipHorizontally ? uv.right : uv.left;
		finaluv.right = flipHorizontally ? uv.left : uv.right;
		finaluv.top = flipVertically ? uv.bottom : uv.top;
		finaluv.bottom = flipVertically ? uv.top : uv.bottom;
		Core->_GetContext()->UpdateSubresource(DrawManager->_GetConstantBufferUV(), 0, 0, &finaluv, 0, 0);
		//extra buffer
		if(extraBufferPSdata != nullptr)
			Core->_GetContext()->UpdateSubresource(DrawManager->_GetConstantBufferPSExtra(), 0, 0, extraBufferPSdata, 0, 0);
		
		//tex
		Core->_GetContext()->PSSetShaderResources(0, 1, texture->_GetShaderResourceAddress());
		//draw
		Core->_GetContext()->DrawIndexed(6, 0, 0);
	}

	void Sprite::_SpriteUpdate()
	{
		size += sizeVelocity * (float)Core->GetFrameTime();
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
		underCursor = DirectX::TriangleTests::Intersects(origin, dir, A, B, C, dist);
		//if cursor is not over 1st triangle, try the second one
		if(!underCursor)
			underCursor = DirectX::TriangleTests::Intersects(origin, dir, A, C, D, dist);
	}

	void Sprite::Destroy()
	{
		Dynamic::Destroy();
		if(index != -1)
			DrawManager->RemoveSprite(this);
		delete this;
	}
}