#include "Private.h"

namespace dx2d
{
	void CreateSampler(TEX_FILTER mode, ID3D11SamplerState** sampler)
	{
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = mode == TEX_FILTER::POINT ?
		D3D11_FILTER_MIN_MAG_MIP_POINT : D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		Core->zDevice->CreateSamplerState(&sampDesc, sampler);
	}

	CDrawManager::CDrawManager() : 
		SupressDuplicateWarning(false),
		TexFilterCreationMode (TEX_FILTER::POINT)
	{
		D3D11_RASTERIZER_DESC rd;
		ZeroMemory(&rd, sizeof(rd));
		rd.FillMode = D3D11_FILL_WIREFRAME;
		rd.CullMode = D3D11_CULL_NONE;
		hr = Core->zDevice->CreateRasterizerState(&rd, &zWireframe); CHECKHR();
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_FRONT;
		hr = Core->zDevice->CreateRasterizerState(&rd, &zSolid); CHECKHR();

		//sprite shared buffer
		Vertex v[] =
		{
			{ -1.0f, -1.0f, 0, 0, 0, 0, 0, 1 },
			{ 1.0f, -1.0f, 0, 0, 0, 0, 1, 1 },
			{ 1.0f, 1.0f, 0, 0, 0, 0, 1, 0 },
			{ -1.0f, 1.0f, 0, 0, 0, 0, 0, 0 }
		};
		int indices[] = { 0, 1, 2, 0, 2, 3, };

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(int) * 6;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA srd;
		srd.pSysMem = indices;
		Core->zDevice->CreateBuffer(&indexBufferDesc, &srd, &zIndexBufferSprite);

		//so far the only indexed buffer in the engine
		Core->zContext->IASetIndexBuffer(zIndexBufferSprite, DXGI_FORMAT_R32_UINT, 0);

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = v;
		Core->zDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &zVertexBufferSprite);

		//default white texture used by non textured objects
		BYTE fourOnes[4] = { 1, 1, 1, 1 };
		ID3D11Texture2D* tex = Functions::CreateTexture2DFromBytes(fourOnes, 1, 1);
		Core->zDevice->CreateShaderResourceView(tex, 0, &zWhiteRes);
		tex->Release();		
		
		CreateSampler(TEX_FILTER::POINT,&zPointSampler);
		CreateSampler(TEX_FILTER::LINEAR, &zLineSampler);		
	}

	void CDrawManager::zInit()
	{
		///////////////////////
		//default render target
		///////////////////////
		RenderTarget defaultTarget;
		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = 800;
		textureDesc.Height = 600;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		Core->zDevice->CreateTexture2D(&textureDesc, NULL, &defaultTarget.texture);

		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		Core->zDevice->CreateRenderTargetView(defaultTarget.texture,
			&renderTargetViewDesc, &defaultTarget.targetView);

		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		Core->zDevice->CreateShaderResourceView(defaultTarget.texture,
			&shaderResourceViewDesc, &defaultTarget.shaderResource);

		defaultTarget.sprite = new CSprite();
		defaultTarget.sprite->zTexture = nullptr;
		defaultTarget.sprite->PixelShader = Core->zDefaultPost;
		defaultTarget.sprite->zShaderResource = defaultTarget.shaderResource;
		zRenderTargets.push_back(defaultTarget);
	}

	//when adding drawable to drawmanager, its index should be -1
	//if it's not it probably means that it's there already
	bool CDrawManager::zHasObject(CDrawable* d)
	{
		if ( !SupressDuplicateWarning && d->zIndex != -1)
		{
			MessageBox(0, L"This object has already been added", L"Error", MB_ICONEXCLAMATION);
			return true;
		}
		return false;
	}

	void CDrawManager::AddPoly(CPolygon* p)
	{
		if (zHasObject(p))
			return;
		zPolygons.push_back(p);
		p->zIndex = (int)zPolygons.size() - 1;
	}

	CPolygon* CDrawManager::AddPoly(XMFLOAT2 points[], int n)
	{
		CPolygon* newPoly = new CPolygon(points, n);
		zPolygons.push_back(newPoly);
		newPoly->zIndex = (int)zPolygons.size() - 1;
		return newPoly;
	}

	CRectangle* CDrawManager::AddRect(float sizex, float sizey)
	{
		CRectangle* newRect = new CRectangle(sizex,sizey);
		zPolygons.push_back(newRect);
		newRect->zIndex = (int)zPolygons.size() - 1;
		return newRect;
	}

	CCircle* CDrawManager::AddCircle(float radius, unsigned char resolution)
	{
		CCircle* newCircle = new CCircle(radius, resolution);
		zPolygons.push_back(newCircle);
		newCircle->zIndex = (int)zPolygons.size() - 1;
		return newCircle;
	}

	CSprite* CDrawManager::AddSprite(LPCWSTR file)
	{
		CSprite* newSprite;
		try
		{
			newSprite = new CSprite(file);
		}
		catch (int)
		{
			return nullptr;
		}
		zSprites.push_back(newSprite);
		newSprite->zIndex = (int)zSprites.size() - 1;
		return newSprite;
	}

	void CDrawManager::AddSprite(CSprite* s)
	{
		if (zHasObject(s))
			return;
		zSprites.push_back(s);
		s->zIndex = (int)zSprites.size() - 1;
	}

	CAnimation* CDrawManager::AddAnimation(LPCWSTR file, int x, int y)
	{
		CAnimation* newAnimation;
		try
		{
			newAnimation = new CAnimation(file, x, y);
		}
		catch (int)
		{
			return nullptr;
		}
		zSprites.push_back(newAnimation);
		newAnimation->zIndex = (int)zSprites.size() - 1;
		return newAnimation;
	}

	void CDrawManager::AddAnimation(CAnimation* a)
	{
		AddSprite(a);
	}

	void CDrawManager::zRenderTargetTransform()
	{
		XMMATRIX transform = XMMatrixTranspose(zRenderTargetMatrix);
		Core->zContext->UpdateSubresource(zRenderTargets[0].sprite->zCbBufferVS, 0, NULL, &transform, 0, 0);
		Core->zContext->VSSetConstantBuffers(0, 1, &zRenderTargets[0].sprite->zCbBufferVS);
	}

	void CDrawManager::zDrawAll()
	{
		Core->zContext->ClearDepthStencilView(Core->zDepthStencilView, 
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		//GetContext()->OMSetBlendState(Core->blendState, 0, 0xffffffff);

		Core->zContext->ClearRenderTargetView(zRenderTargets[0].targetView, Core->zBackBufferColor);
		Core->zContext->OMSetRenderTargets(1, &zRenderTargets[0].targetView, Core->zDepthStencilView);

		//polys
		Core->zContext->PSSetShader(Core->zDefaultPS, 0, 0);
		Core->zContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
		Core->zContext->RSSetState(zWireframe);
		for (CPolygon* p : zPolygons)
		{
			p->zUpdate();
			if (p->Visible)
			{
				p->zTransform();
				p->zDraw();
			}
		}
		
		//sprites, animations
		Core->zContext->RSSetState(zSolid);
		Core->zContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		Core->zContext->IASetVertexBuffers(0, 1, &zVertexBufferSprite, &stride, &offset);
		for (CSprite* s : zSprites)
		{
			s->zUpdate();
			s->zPlay();
			if (s->Visible)
			{
				if(s->TexFilter == TEX_FILTER::LINEAR)
					Core->zContext->PSSetSamplers(0, 1, &zLineSampler);
				else
					Core->zContext->PSSetSamplers(0, 1, &zPointSampler);
				s->zTransform();
				s->zDraw();
			}
		}
		//flush debug
		DebugManager->Flush();
		//bitmap text
		for (CBitmapText* t : zTexts)
		{
			t->zUpdate();
			if (t->Visible)
			{
				if (t->TexFilter == TEX_FILTER::LINEAR)
					Core->zContext->PSSetSamplers(0, 1, &zLineSampler);
				else
					Core->zContext->PSSetSamplers(0, 1, &zPointSampler);
				t->zDraw();
			}
		}
		Core->zContext->ClearDepthStencilView(Core->zDepthStencilView,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		Core->zContext->OMSetRenderTargets(1, &Core->zBackBuffer, Core->zDepthStencilView);
		zRenderTargetTransform();
		zRenderTargets[0].sprite->FlipVertically = true;
		zRenderTargets[0].sprite->zDraw();
	}

	void CDrawManager::Destroy()
	{
		for (int i = (int)zRenderTargets.size() - 1; i >= 0; i--)
		{
			zRenderTargets[i].shaderResource->Release();
			zRenderTargets[i].texture->Release();
			zRenderTargets[i].targetView->Release();
			zRenderTargets[i].sprite->Destroy();
		}
		zLineSampler->Release();
		zPointSampler->Release();
		for (int i = (int)zPolygons.size() - 1; i >= 0; i--)
			zPolygons[i]->Destroy();
		for (int i = (int)zSprites.size() - 1; i >= 0; i--)
			zSprites[i]->Destroy();
		zIndexBufferSprite->Release();
		zVertexBufferSprite->Release();
		zWireframe->Release();
		zSolid->Release();
		delete this;
	}

	void CDrawManager::RemovePoly(CPolygon* p)
	{
		if (p->zIndex == -1)
			return;
		if (zPolygons.size() == 0 || zPolygons[p->zIndex] != p)
		{
			MessageBox(0, L"This polygon is not in CDrawManager.Polygons", L"Error", MB_ICONERROR);
			return;
		}
		else if (p->zIndex == zPolygons.size() - 1)
		{			
			zPolygons.pop_back();
		}
		else
		{
			//move end to where p is and update index
			zPolygons[p->zIndex] = zPolygons.back();
			zPolygons[p->zIndex]->zIndex = p->zIndex;
			zPolygons.pop_back();
		}
		p->zIndex = -1;
	}

	void CDrawManager::RemoveSprite(CSprite* s)
	{
		if (s->zIndex == -1)
			return;
		if (s->zIndex >= zSprites.size() || zSprites[s->zIndex] != s)
		{
			MessageBox(0, L"This sprite is not in CDrawManager.Sprites", L"Error", MB_ICONERROR);
			return;
		}
		else if (s->zIndex == zSprites.size() - 1)
		{
			zSprites.pop_back();
		}
		else
		{
			//move end to where p is and update index
			zSprites[s->zIndex] = zSprites.back();
			zSprites[s->zIndex]->zIndex = s->zIndex;
			zSprites.pop_back();
		}
		s->zIndex = -1;
	}

	void CDrawManager::RemoveAnimation(CAnimation* a)
	{
		RemoveSprite(a);
	}

	CBitmapFont* CDrawManager::AddBitmapFont(LPCWSTR file, vector<Rect> chars)
	{
		CBitmapFont* newFont = ResourceManager->GetBitmapFont(file);
		if (newFont != nullptr)
			return newFont;
		try
		{
			newFont = new CBitmapFont(file, chars);
		}
		catch(int)
		{
			return nullptr;
		}
		ResourceManager->AddBitmapFont(newFont);
		return newFont;
	}
	
	void CDrawManager::AddBitmapFont(CBitmapFont* font)
	{
		CBitmapFont* newFont = ResourceManager->GetBitmapFont(font->zTexture->zName);
		if (newFont != nullptr)
			return;
		ResourceManager->AddBitmapFont(font);
	}

	CBitmapText* CDrawManager::AddBitmapText(CBitmapFont* font)
	{
		if (font == nullptr)
			return nullptr;
		CBitmapText* newText = new CBitmapText(font);
		zTexts.push_back(newText);
		newText->zIndex = (int)zTexts.size() - 1;
		return newText;
	}

	void CDrawManager::AddBitmapText(CBitmapText* text)
	{
		if (zHasObject(text))
			return;
		zTexts.push_back(text);
		text->zIndex = (int)zTexts.size() - 1;
	}

	void CDrawManager::RemoveBitmapFont(CBitmapFont* font)
	{
		MessageBox(0, L"CDrawManager::RemoveBitmapFont(CBitmapFont* font)\nnot implemented", 0, 0);
	}

	void CDrawManager::RemoveBitmapText(CBitmapText* text)
	{
		if (text->zIndex == -1)
			return;
		if (text->zIndex >= zTexts.size() || zTexts[text->zIndex] != text)
		{
			MessageBox(0, L"This text is not in CDrawManager.Texts", L"Error", MB_ICONERROR);
			return;
		}
		else if (text->zIndex == zTexts.size() - 1)
		{
			zTexts.pop_back();
		}
		else
		{
			//move end to where p is and update index
			zTexts[text->zIndex] = zTexts.back();
			zTexts[text->zIndex]->zIndex = text->zIndex;
			zTexts.pop_back();
		}
		text->zIndex = -1;
	}

	CBitmapFont* CDrawManager::GetDefaultFont()
	{
		return zDefaultFont;
	}
}