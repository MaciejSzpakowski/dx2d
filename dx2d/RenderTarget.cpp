#include "Viva.h"

namespace Viva
{
	CRenderTarget::CRenderTarget()
	{
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

		ID3D11RenderTargetView* rtv;
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

		PixelShader = Core->zDefaultPost;
		zTexture = tex;
		zTargetView = rtv;
		zSprite = new CSprite();
		zSprite->FlipVertically = true;
		zSprite->Pickable = false;
		zSprite->zTexture = nullptr;
		zSprite->PixelShader = nullptr;
		zSprite->zShaderResource = srv;
	}

	int myfunc(CSprite* s1, CSprite* s2)
	{
		return s1->GetPosition().z > s2->GetPosition().z;
	}

	void CRenderTarget::zDraw()
	{
		Core->zContext->ClearDepthStencilView(Core->zDepthStencilView,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		if (Core->EnableAlpha)
			Core->zContext->OMSetBlendState(Core->zBlendState, 0, 0xffffffff);

		Core->zContext->OMSetRenderTargets(1, &zTargetView, Core->zDepthStencilView);
		float four0[4] = { 0, 0, 0, 0 };
		Core->zContext->ClearRenderTargetView(zTargetView, four0);

		//polys
		Core->zContext->PSSetShader(Core->zDefaultPS, 0, 0);
		Core->zContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
		Core->zContext->RSSetState(DrawManager->zWireframe);
		for (CPolygon* p : zPolygons)
		{
			p->zUpdate();
			p->zTransform();
			if (p->Visible)
			{
				p->zDraw();
			}
		}

		//sprites, animations
		Core->zContext->RSSetState(DrawManager->zSolid);
		Core->zContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		Core->zContext->IASetVertexBuffers(0, 1, &DrawManager->zVertexBufferSprite, &stride, &offset);
		if (Core->EnableAlpha)
			std::sort(zSprites.begin(), zSprites.end(), myfunc);
		for (CSprite* s : zSprites)
		{
			s->zUpdate();
			s->zSpriteUpdate();
			s->zPlay();
			s->zTransform();
			if (s->Visible)
			{
				if (s->TexFilter == TextureFilter::Linear)
					Core->zContext->PSSetSamplers(0, 1, &DrawManager->zLineSampler);
				else
					Core->zContext->PSSetSamplers(0, 1, &DrawManager->zPointSampler);
				s->zDraw();
			}
		}
		//bitmap text
		for (CBitmapText* t : zTexts)
		{
			t->zUpdate();
			if (t->Visible)
			{
				if (t->TexFilter == TextureFilter::Linear)
					Core->zContext->PSSetSamplers(0, 1, &DrawManager->zLineSampler);
				else
					Core->zContext->PSSetSamplers(0, 1, &DrawManager->zPointSampler);
				t->zDraw();
			}
		}
	}

	void CRenderTarget::MoveToTop()
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

	void CRenderTarget::MoveToBottom()
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

	void CRenderTarget::MoveUp()
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

	void CRenderTarget::MoveDown()
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

	void CRenderTarget::Destroy()
	{
		for (int i = (int)zPolygons.size() - 1; i >= 0; i--)
			zPolygons[i]->Destroy();
		for (int i = (int)zSprites.size() - 1; i >= 0; i--)
			zSprites[i]->Destroy();
		for (int i = (int)zTexts.size() - 1; i >= 0; i--)
			zTexts[i]->Destroy();
		zSprite->zShaderResource->Release();
		zTexture->Release();
		zTargetView->Release();
		zSprite->Destroy();
		delete this;
	}
}