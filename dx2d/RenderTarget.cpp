#include "Viva.h"

namespace Viva
{
	RenderTarget::RenderTarget()
	{
		extraBufferPSdata = nullptr;

		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

		ID3D11Texture2D* tex;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = (UINT)Core->GetClientSize().width;
		textureDesc.Height = (UINT)Core->GetClientSize().height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		HRESULT hr = Core->_GetDevice()->CreateTexture2D(&textureDesc, NULL, &tex); CHECKHR();

		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		hr = Core->_GetDevice()->CreateRenderTargetView(tex,
			&renderTargetViewDesc, &rtv); CHECKHR();

		ID3D11ShaderResourceView* srv;
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		hr = Core->_GetDevice()->CreateShaderResourceView(tex,
			&shaderResourceViewDesc, &srv); CHECKHR();

		pixelShader = Core->_GetDefaultPost();
		texture = tex;
		sprite = new Sprite(new Texture(srv,L"renderTargetTexture"),TextureFilter::Point);
		sprite->SetFlipVertically(true);
		sprite->SetPixelShader(nullptr);
	}

	void RenderTarget::_DrawObjects(const XMMATRIX* view, const XMMATRIX* proj)
	{
		Core->_GetContext()->ClearDepthStencilView(Core->_GetDepthStencilView(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		if (Core->IsAlphaEnabled())
			Core->_GetContext()->OMSetBlendState(Core->_GetBlendState(), 0, 0xffffffff);

		Core->_GetContext()->OMSetRenderTargets(1, &rtv, Core->_GetDepthStencilView());
		float four0[4] = { 0, 0, 0, 0 };
		Core->_GetContext()->ClearRenderTargetView(rtv, four0);

		//polys
		Core->_GetContext()->PSSetShader(Core->_GetDefaultPS(), 0, 0);
		Core->_GetContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
		Core->_GetContext()->RSSetState(DrawManager->_GetWireframe());
		for (Polygon* p : polygons)
		{
			p->_Update();
            p->_Transform(view, proj);
			if (p->IsVisible())
				p->_Draw();
		}

		//sprites, animations
		Core->_GetContext()->RSSetState(DrawManager->_GetSolid());
		Core->_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		Core->_GetContext()->IASetVertexBuffers(0, 1, DrawManager->_GetVertexBufferSpriteAddress(), &stride, &offset);
		if (Core->IsAlphaEnabled())
			std::sort(sprites.begin(), sprites.end(), [](Sprite* s1,Sprite* s2) {return s1->GetPosition().z > s2->GetPosition().z; });
		for (Sprite* s : sprites)
		{
			s->_Update();
			s->_SpriteUpdate();
			s->_Play();
			s->_Transform(view,proj);
			if (s->IsVisible())
			{
				if (s->GetTextureFilter() == TextureFilter::Linear)
					Core->_GetContext()->PSSetSamplers(0, 1, DrawManager->_GetLineSamplerAddress());
				else
					Core->_GetContext()->PSSetSamplers(0, 1, DrawManager->_GetPointSamplerAddress());
				s->_Draw();
			}
		}
		//bitmap text
		for (BitmapText* t : texts)
		{
			t->_Update();
			if (t->IsVisible())
			{
				if (t->GetTexFilter() == TextureFilter::Linear)
					Core->_GetContext()->PSSetSamplers(0, 1, DrawManager->_GetLineSamplerAddress());
				else
					Core->_GetContext()->PSSetSamplers(0, 1, DrawManager->_GetPointSamplerAddress());
				t->_Draw();
			}
		}
	}

	void RenderTarget::MoveToBottom()
	{
		DrawManager->MoveRenderTargetToBottom(this);
	}

	void RenderTarget::MoveToTop()
	{
		DrawManager->MoveRenderTargetToTop(this);
	}

	void RenderTarget::MoveUp()
	{
		DrawManager->MoveRenderTargetUp(this);
	}

	void RenderTarget::MoveDown()
	{
		DrawManager->MoveRenderTargetDown(this);
	}

	void RenderTarget::Destroy()
	{
		DrawManager->RemoveRenderTarget(this);

		for (int i = (int)polygons.size() - 1; i >= 0; i--)
			polygons[i]->Destroy();
		for (int i = (int)sprites.size() - 1; i >= 0; i--)
			sprites[i]->Destroy();
		for (int i = (int)texts.size() - 1; i >= 0; i--)
			texts[i]->Destroy();
		texture->Release();
		sprite->GetTexture()->Destroy();
		rtv->Release();
		sprite->Destroy();
		delete this;
	}

	void RenderTarget::RemovePoly(Polygon* p)
	{
		int index = p->_GetIndex();

		if (index == -1)
			throw VIVA_ERROR("This poly has no render target");

		if (polygons.size() <= index || polygons[index] != p)
			throw VIVA_ERROR("Render target index mismatch");
		else if (index == polygons.size() - 1)
			polygons.pop_back();
		else
		{
			//swap with end and pop end
			polygons[index] = polygons.back();
			polygons[index]->_SetIndex(index);
			polygons.pop_back();
		}

		p->_SetRenderTarget(nullptr);
		p->_SetIndex(-1);
	}

	void RenderTarget::RemoveText(BitmapText* t)
	{
		int index = t->_GetIndex();

		if (index == -1)
			throw VIVA_ERROR("This text has no render target");

		if (texts.size() <= index || texts[index] != t)
			throw VIVA_ERROR("Render target index mismatch");
		else if (index == texts.size() - 1)
		{
			texts.pop_back();
		}
		else
		{
			//swap with end and pop end
			texts[index] = texts.back();
			texts[index]->_SetIndex(index);
			texts.pop_back();
		}
		t->_SetRenderTarget(nullptr);
		t->_SetIndex(-1);
	}

	void RenderTarget::RemoveSprite(Sprite* s)
	{
		int index = s->_GetIndex();

		if (index == -1)
			throw VIVA_ERROR("This sprite has no render target");

		if (sprites.size() <= index || sprites[index] != s)
			throw VIVA_ERROR("Render target index mismatch");
		else if (index == sprites.size() - 1)
			sprites.pop_back();
		else
		{
			//swap with end and pop end
			sprites[index] = sprites.back();
			sprites[index]->_SetIndex(index);
			sprites.pop_back();
		}

		s->_SetRenderTarget(nullptr);
		s->_SetIndex(-1);
	}
}