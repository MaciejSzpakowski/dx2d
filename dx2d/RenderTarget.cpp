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
		textureDesc.Width = (UINT)Core->clientSize.width;
		textureDesc.Height = (UINT)Core->clientSize.height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		HRESULT hr = Core->zDevice->CreateTexture2D(&textureDesc, NULL, &tex); CHECKHR();

		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		hr = Core->zDevice->CreateRenderTargetView(tex,
			&renderTargetViewDesc, &rtv); CHECKHR();

		ID3D11ShaderResourceView* srv;
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		hr = Core->zDevice->CreateShaderResourceView(tex,
			&shaderResourceViewDesc, &srv); CHECKHR();

		pixelShader = Core->zDefaultPost;
		texture = tex;
		sprite = new Sprite(new CTexture(srv,L"renderTargetTexture"));
		sprite->SetFlipVertically(true);
		sprite->Pickable = false;
		sprite->SetPixelShader(nullptr);
	}

	void RenderTarget::_DrawObjects()
	{
		Core->zContext->ClearDepthStencilView(Core->zDepthStencilView,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		if (Core->EnableAlpha)
			Core->zContext->OMSetBlendState(Core->zBlendState, 0, 0xffffffff);

		Core->zContext->OMSetRenderTargets(1, &rtv, Core->zDepthStencilView);
		float four0[4] = { 0, 0, 0, 0 };
		Core->zContext->ClearRenderTargetView(rtv, four0);

		//polys
		Core->zContext->PSSetShader(Core->zDefaultPS, 0, 0);
		Core->zContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
		Core->zContext->RSSetState(DrawManager->zWireframe);
		for (Polygon* p : polygons)
		{
			p->zUpdate();
			p->zTransform();
			if (p->Visible)
			{
				p->_Draw();
			}
		}

		//sprites, animations
		Core->zContext->RSSetState(DrawManager->zSolid);
		Core->zContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		Core->zContext->IASetVertexBuffers(0, 1, &DrawManager->zVertexBufferSprite, &stride, &offset);
		if (Core->EnableAlpha)
			std::sort(sprites.begin(), sprites.end(), [](Sprite* s1,Sprite* s2) {return s1->GetPosition().z > s2->GetPosition().z; });
		for (Sprite* s : sprites)
		{
			s->zUpdate();
			s->_SpriteUpdate();
			s->_Play();
			s->zTransform();
			if (s->Visible)
			{
				if (s->GetTextureFilter() == TextureFilter::Linear)
					Core->zContext->PSSetSamplers(0, 1, &DrawManager->zLineSampler);
				else
					Core->zContext->PSSetSamplers(0, 1, &DrawManager->zPointSampler);
				s->_Draw();
			}
		}
		//bitmap text
		for (BitmapText* t : texts)
		{
			t->zUpdate();
			if (t->Visible)
			{
				if (t->GetTexFilter() == TextureFilter::Linear)
					Core->zContext->PSSetSamplers(0, 1, &DrawManager->zLineSampler);
				else
					Core->zContext->PSSetSamplers(0, 1, &DrawManager->zPointSampler);
				t->_Draw();
			}
		}
	}

	void RenderTarget::MoveToBottom()
	{
		int size = (int)DrawManager->zRenderTargets.size();
		if (size < 2)
			return;
		int index = -1;
		for (int i = 0; i<size; i++)
			if (DrawManager->zRenderTargets[i] == this)
			{
				index = i;
				break;
			}
		if (index == -1)
			return;
		DrawManager->zRenderTargets.erase(DrawManager->zRenderTargets.begin() + index);
		DrawManager->zRenderTargets.insert(DrawManager->zRenderTargets.begin(), this);
	}

	void RenderTarget::MoveToTop()
	{
		int size = (int)DrawManager->zRenderTargets.size();
		if (size < 2)
			return;
		int index = -1;
		for (int i = 0; i<size; i++)
			if (DrawManager->zRenderTargets[i] == this)
			{
				index = i;
				break;
			}
		if (index == -1)
			return;
		DrawManager->zRenderTargets.erase(DrawManager->zRenderTargets.begin() + index);
		DrawManager->zRenderTargets.push_back(this);
	}

	void RenderTarget::MoveUp()
	{
		int size = (int)DrawManager->zRenderTargets.size();
		if (size < 2)
			return;
		int index = -1;
		for (int i = 0; i<size; i++)
			if (DrawManager->zRenderTargets[i] == this)
			{
				index = i;
				break;
			}
		if (index == -1 || index == size - 1)
			return;
		DrawManager->zRenderTargets[index] = DrawManager->zRenderTargets[index + 1];
		DrawManager->zRenderTargets[index + 1] = this;
	}

	void RenderTarget::MoveDown()
	{
		int size = (int)DrawManager->zRenderTargets.size();
		if (size < 2)
			return;
		int index = -1;
		for (int i = 0; i<size; i++)
			if (DrawManager->zRenderTargets[i] == this)
			{
				index = i;
				break;
			}
		if (index == -1 || index == 0)
			return;
		DrawManager->zRenderTargets[index] = DrawManager->zRenderTargets[index - 1];
		DrawManager->zRenderTargets[index - 1] = this;
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
		if (p->zIndex == -1)
			throw VIVA_ERROR("This poly has no render target");

		if (polygons.size() <= p->zIndex || polygons[p->zIndex] != p)
			throw VIVA_ERROR("Render target index mismatch");
		else if (p->zIndex == polygons.size() - 1)
			polygons.pop_back();
		else
		{
			//swap with end and pop end
			polygons[p->zIndex] = polygons.back();
			polygons[p->zIndex]->zIndex = p->zIndex;
			polygons.pop_back();
		}

		p->zRenderTarget = nullptr;
		p->zIndex = -1;
	}

	void RenderTarget::RemoveText(BitmapText* t)
	{
		if (t->zIndex == -1)
			throw VIVA_ERROR("This text has no render target");

		if (texts.size() <= t->zIndex|| texts[t->zIndex] != t)
			throw VIVA_ERROR("Render target index mismatch");
		else if (t->zIndex == texts.size() - 1)
		{
			texts.pop_back();
		}
		else
		{
			//swap with end and pop end
			texts[t->zIndex] = texts.back();
			texts[t->zIndex]->zIndex = t->zIndex;
			texts.pop_back();
		}
		t->zRenderTarget = nullptr;
		t->zIndex = -1;
	}

	void RenderTarget::RemoveSprite(Sprite* s)
	{
		if (s->zIndex == -1)
			throw VIVA_ERROR("This sprite has no render target");

		if (sprites.size() <= s->zIndex || sprites[s->zIndex] != s)
			throw VIVA_ERROR("Render target index mismatch");
		else if (s->zIndex == sprites.size() - 1)
			sprites.pop_back();
		else
		{
			//swap with end and pop end
			sprites[s->zIndex] = sprites.back();
			sprites[s->zIndex]->zIndex = s->zIndex;
			sprites.pop_back();
		}

		s->zRenderTarget = nullptr;
		s->zIndex = -1;
	}
}