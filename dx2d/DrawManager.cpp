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
		zDefaultRenderTarget = AddRenderTarget();
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

	void CDrawManager::AddPoly(CPolygon* p, CRenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		if (zHasObject(p))
			return;
		target->zPolygons.push_back(p);
		p->zRenderTarget = target;
		p->zIndex = (int)target->zPolygons.size() - 1;
	}

	CPolygon* CDrawManager::AddPoly(XMFLOAT2 points[], int n, CRenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		CPolygon* newPoly = new CPolygon(points, n);
		target->zPolygons.push_back(newPoly);
		newPoly->zRenderTarget = target;
		newPoly->zIndex = (int)target->zPolygons.size() - 1;
		return newPoly;
	}

	CRectangle* CDrawManager::AddRect(float sizex, float sizey, CRenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		CRectangle* newRect = new CRectangle(sizex,sizey);
		target->zPolygons.push_back(newRect);
		newRect->zRenderTarget = target;
		newRect->zIndex = (int)target->zPolygons.size() - 1;
		return newRect;
	}

	CCircle* CDrawManager::AddCircle(float radius, unsigned char resolution, CRenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		CCircle* newCircle = new CCircle(radius, resolution);
		target->zPolygons.push_back(newCircle);
		newCircle->zRenderTarget = target;
		newCircle->zIndex = (int)target->zPolygons.size() - 1;
		return newCircle;
	}

	CSprite* CDrawManager::AddSprite(LPCWSTR file, CRenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		CSprite* newSprite;
		try
		{
			newSprite = new CSprite(file);
		}
		catch (int)
		{
			return nullptr;
		}
		target->zSprites.push_back(newSprite);
		newSprite->zRenderTarget = target;
		newSprite->zIndex = (int)target->zSprites.size() - 1;
		return newSprite;
	}

	void CDrawManager::AddSprite(CSprite* s, CRenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		if (zHasObject(s))
			return;
		target->zSprites.push_back(s);
		s->zRenderTarget = target;
		s->zIndex = (int)target->zSprites.size() - 1;
	}

	CAnimation* CDrawManager::AddAnimation(LPCWSTR file, int x, int y, CRenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		CAnimation* newAnimation;
		try
		{
			newAnimation = new CAnimation(file, x, y);
		}
		catch (int)
		{
			return nullptr;
		}
		target->zSprites.push_back(newAnimation);
		newAnimation->zRenderTarget = target;
		newAnimation->zIndex = (int)target->zSprites.size() - 1;
		return newAnimation;
	}

	void CDrawManager::AddAnimation(CAnimation* a, CRenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		AddSprite(a, target);
	}

	void CDrawManager::zRenderTargetTransform(int i)
	{
		float scalex = (20.0f - i*0.001f) * tan(Camera->zFovAngle / 2) * Camera->zAspectRatio;
		float scaley = (20.0f - i*0.001f) * tan(Camera->zFovAngle / 2);
		//move every target above previous one
		XMMATRIX transform = XMMatrixScaling(scalex, scaley, 1) *
			XMMatrixTranslation(0, 0, -i*0.001f) *  zRenderTargetMatrix;

		transform = XMMatrixTranspose(transform);
		Core->zContext->UpdateSubresource(zRenderTargets[0]->zSprite->zCbBufferVS, 0, NULL, &transform, 0, 0);
		Core->zContext->VSSetConstantBuffers(0, 1, &zRenderTargets[0]->zSprite->zCbBufferVS);
	}

	void CDrawManager::zDrawAll()
	{
		for (int i = 0; i < (int)zRenderTargets.size(); i++)
			zRenderTargets[i]->zDraw();

		Core->zContext->ClearRenderTargetView(Core->zBackBuffer, Core->zBackBufferColor);
		Core->zContext->ClearDepthStencilView(Core->zDepthStencilView,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		Core->zContext->OMSetRenderTargets(1, &Core->zBackBuffer, Core->zDepthStencilView);
		
		for (int i = 0; i < (int)zRenderTargets.size(); i++)
		{
			zRenderTargetTransform(i);
			Core->zContext->PSSetShader(zRenderTargets[i]->PixelShader, 0, 0);
			zRenderTargets[i]->zSprite->zDraw();
		}

		//debug text
		Core->zContext->PSSetShader(Core->zDefaultPS, 0, 0);
		DebugManager->Flush();
		if(DebugManager->zDebugText->Text != L"")
			DebugManager->zDebugText->zDraw();

		Core->zSwapChain->Present(0, 0);
	}

	void CDrawManager::Destroy()
	{
		for (int i = (int)zRenderTargets.size() - 1; i >= 0; i--)
			zRenderTargets[i]->Destroy();
		zLineSampler->Release();
		zPointSampler->Release();
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
		auto polygons = &(p->zRenderTarget->zPolygons);
		if (polygons->size() == 0 || (*polygons)[p->zIndex] != p)
		{
			MessageBox(0, L"This polygon is not in its target polygons collection", L"Error", MB_ICONERROR);
			return;
		}
		else if (p->zIndex == polygons->size() - 1)
		{			
			polygons->pop_back();
		}
		else
		{
			//move end to where p is and update index
			(*polygons)[p->zIndex] = polygons->back();
			(*polygons)[p->zIndex]->zIndex = p->zIndex;
			polygons->pop_back();
		}
		p->zRenderTarget = nullptr;
		p->zIndex = -1;
	}

	void CDrawManager::RemoveSprite(CSprite* s)
	{
		if (s->zIndex == -1)
			return;
		auto sprites = &(s->zRenderTarget->zSprites);
		if (s->zIndex >= sprites->size() || (*sprites)[s->zIndex] != s)
		{
			MessageBox(0, L"This sprite is not in its target sprites collection", L"Error", MB_ICONERROR);
			return;
		}
		else if (s->zIndex == sprites->size() - 1)
		{
			sprites->pop_back();
		}
		else
		{
			//move end to where p is and update index
			(*sprites)[s->zIndex] = sprites->back();
			(*sprites)[s->zIndex]->zIndex = s->zIndex;
			sprites->pop_back();
		}
		s->zRenderTarget = nullptr;
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

	CBitmapFont* CDrawManager::AddBitmapFont(LPCWSTR file, int width, int height, int charsPerRow)
	{
		CBitmapFont* newFont = ResourceManager->GetBitmapFont(file);
		if (newFont != nullptr)
			return newFont;
		try
		{
			newFont = new CBitmapFont(file, width, height, charsPerRow);
		}
		catch (int)
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

	CBitmapText* CDrawManager::AddBitmapText(CBitmapFont* font, CRenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		if (font == nullptr)
			return nullptr;
		CBitmapText* newText = new CBitmapText(font);
		target->zTexts.push_back(newText);
		newText->zRenderTarget = target;
		newText->zIndex = (int)target->zTexts.size() - 1;
		return newText;
	}

	void CDrawManager::AddBitmapText(CBitmapText* text, CRenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		if (zHasObject(text))
			return;
		target->zTexts.push_back(text);
		text->zRenderTarget = target;
		text->zIndex = (int)target->zTexts.size() - 1;
	}

	void CDrawManager::RemoveBitmapFont(CBitmapFont* font)
	{
		MessageBox(0, L"CDrawManager::RemoveBitmapFont(CBitmapFont* font)\nnot implemented", 0, 0);
	}

	void CDrawManager::RemoveBitmapText(CBitmapText* t)
	{
		if (t->zIndex == -1)
			return;
		auto texts = &(t->zRenderTarget->zTexts);
		if (t->zIndex >= texts->size() || (*texts)[t->zIndex] != t)
		{
			MessageBox(0, L"This text is not in target texts", L"Error", MB_ICONERROR);
			return;
		}
		else if (t->zIndex == texts->size() - 1)
		{
			texts->pop_back();
		}
		else
		{
			//move end to where p is and update index
			(*texts)[t->zIndex] = texts->back();
			(*texts)[t->zIndex]->zIndex = t->zIndex;
			texts->pop_back();
		}
		t->zRenderTarget = nullptr;
		t->zIndex = -1;
	}

	CBitmapFont* CDrawManager::GetDefaultFont()
	{
		return zDefaultFont;
	}

	CRenderTarget* CDrawManager::AddRenderTarget()
	{		
		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

		ID3D11Texture2D* tex;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = Core->zClientSize.x;
		textureDesc.Height = Core->zClientSize.y;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		hr = Core->zDevice->CreateTexture2D(&textureDesc, NULL, &tex); CHECKHR();

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

		CRenderTarget* target = new CRenderTarget;
		target->zTexture = tex;
		target->zTargetView = rtv;
		target->zSprite = new CSprite();
		target->zSprite->FlipVertically = true;
		target->zSprite->Pickable = false;
		target->zSprite->zTexture = nullptr;
		target->zSprite->PixelShader = nullptr;
		target->zSprite->zShaderResource = srv;

		zRenderTargets.push_back(target);
		return target;
	}

	void CDrawManager::DestroyRenderTarget(CRenderTarget* target)
	{
		target->Destroy();
		for (int i = 0; i < (int)zRenderTargets.size(); i++)
		{
			if (target == zRenderTargets[i])
			{
				zRenderTargets.erase(zRenderTargets.begin() + i);
				return;
			}
		}
	}

	void CDrawManager::ClearRenderTargets()
	{
		for (int i = 0; i < (int)zRenderTargets.size(); i++)
			if (zRenderTargets[i] != zDefaultRenderTarget)
				zRenderTargets[i]->Destroy();
		zRenderTargets.clear();
		zRenderTargets.push_back(zDefaultRenderTarget);
	}

	extern const unsigned char rc_font[];
	void CDrawManager::InitDefaultFont()
	{
		//defalut font
		//15x21 one char
		//20x5 all chars
		vector<Rect> chars1;
		for (int i = 0; i<5; i++)
			for (int j = 0; j < 20; j++)
			{
				chars1.push_back(Rect(15.0f / 300.0f*j, 21.0f / 105.0f*i, 15.0f /
					300.0f*(j + 1), 21 / 105.0f*(i + 1)));
			}
		
		CTexture* tex1 = Functions::GetUncachedTextureFromBytes((BYTE*)rc_font, 300, 105);
		DrawManager->zDefaultFont = new CBitmapFont(tex1,15,21,20);
		DrawManager->AddBitmapFont(DrawManager->zDefaultFont);
		DebugManager->Init(DrawManager->GetDefaultFont());
	}

	void CRenderTarget::zDraw()
	{
		Core->zContext->ClearDepthStencilView(Core->zDepthStencilView,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		//GetContext()->OMSetBlendState(Core->blendState, 0, 0xffffffff);

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