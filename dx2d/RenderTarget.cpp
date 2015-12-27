#include "Viva.h"

namespace Viva
{
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
				if (s->TexFilter == TEX_FILTER::LINEAR)
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
				if (t->TexFilter == TEX_FILTER::LINEAR)
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