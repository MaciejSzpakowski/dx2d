#include "Viva.h"

namespace Viva
{
	void CreateSampler(TextureFilter mode, ID3D11SamplerState** sampler)
	{
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = mode == TextureFilter::Point ?
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
		TexFilterCreationMode (TextureFilter::Point)
	{
		D3D11_RASTERIZER_DESC rd;
		ZeroMemory(&rd, sizeof(rd));
		rd.FillMode = D3D11_FILL_WIREFRAME;
		rd.CullMode = D3D11_CULL_NONE;
		HRESULT hr = Core->zDevice->CreateRasterizerState(&rd, &zWireframe); CHECKHR();
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

		//shared sprite vertex buffer
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

		//shared vertex shader buffer
		D3D11_BUFFER_DESC cbbd;
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = sizeof(XMMATRIX);
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;
		Core->zDevice->CreateBuffer(&cbbd, NULL, &zCbBufferVS);
		Core->zContext->VSSetConstantBuffers(0, 1, &zCbBufferVS);

		//shared vertex shader 2nd buffer and pixel shader buffer
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = sizeof(Color);
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;
		Core->zDevice->CreateBuffer(&cbbd, NULL, &zCbBufferPS);
		Core->zDevice->CreateBuffer(&cbbd, NULL, &zCbBufferUV); //has the same size
		Core->zContext->PSSetConstantBuffers(0, 1, &zCbBufferPS);
		Core->zContext->VSSetConstantBuffers(1, 1, &zCbBufferUV);

		zCbBufferPSExtra = nullptr;

		//default white texture used by non textured objects
		Pixel whitePixel[1] = { Pixel(255,255,255,255) };
		CTexture* tex = new CTexture(whitePixel, Size(1, 1), L"");
		zWhiteRes = tex->zShaderResource;
		delete tex;
		
		CreateSampler(TextureFilter::Point, &zPointSampler);
		CreateSampler(TextureFilter::Linear, &zLineSampler);
	}

	void CDrawManager::zInit()
	{
		zDefaultRenderTarget = AddRenderTarget();
	}

	//when adding drawable to drawmanager, its index should be -1
	//if it's not it probably means that it's there already
	bool CDrawManager::zHasObject(CDynamic* d)
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

	CPolygon* CDrawManager::AddPoly(const vector<XMFLOAT2>& points, CRenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		CPolygon* newPoly = new CPolygon(points);
		target->zPolygons.push_back(newPoly);
		newPoly->zRenderTarget = target;
		newPoly->zIndex = (int)target->zPolygons.size() - 1;
		return newPoly;
	}

	CRectangle* CDrawManager::AddRect(const Size& size, CRenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		CRectangle* newRect = new CRectangle(size);
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
		newSprite = new CSprite(file);
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

	Animation* CDrawManager::AddAnimation(LPCWSTR file, int x, int y, CRenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		Animation* newAnimation;
		newAnimation = new Animation(file, x, y);
		target->zSprites.push_back(newAnimation);
		newAnimation->zRenderTarget = target;
		newAnimation->zIndex = (int)target->zSprites.size() - 1;
		return newAnimation;
	}

	void CDrawManager::AddAnimation(Animation* a, CRenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		AddSprite(a, target);
	}

	void CDrawManager::CreateExtraBuffer(UINT size)
	{
		D3D11_BUFFER_DESC cbbd;
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = size;
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;
		Core->zDevice->CreateBuffer(&cbbd, NULL, &zCbBufferPSExtra);
		Core->zContext->PSSetConstantBuffers(1, 1, &zCbBufferPSExtra);
	}

	void CDrawManager::zRenderTargetTransform(int i)
	{
		float scalex = (20.0f - i*0.001f) * tan(Camera->zFovAngle / 2) * Camera->zAspectRatio;
		float scaley = (20.0f - i*0.001f) * tan(Camera->zFovAngle / 2);
		//move every target above previous one
		XMMATRIX transform = XMMatrixScaling(scalex, scaley, 1) * 
			XMMatrixTranslation(0, 0, -i*0.001f) * zRenderTargetMatrix;

		transform = XMMatrixTranspose(transform);
		Core->zContext->UpdateSubresource(zCbBufferVS, 0, NULL, &transform, 0, 0);
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
			if (zRenderTargets[i]->zExtraBufferPSdata != nullptr)
				Core->zContext->UpdateSubresource(DrawManager->zCbBufferPSExtra, 0, 0, 
					zRenderTargets[i]->zExtraBufferPSdata, 0, 0);
			zRenderTargets[i]->zSprite->zDraw();
		}

		//debug text
		Core->zContext->PSSetShader(Core->zDefaultPS, 0, 0);
		DebugManager->Flush();
		if(DebugManager->zDebugText->GetText() != L"")
			DebugManager->zDebugText->zDraw();

		Core->zSwapChain->Present(0, 0);
	}

	void CDrawManager::Destroy()
	{
		for (int i = (int)zRenderTargets.size() - 1; i >= 0; i--)
			zRenderTargets[i]->Destroy();
		if (zCbBufferPSExtra != nullptr)
			zCbBufferPSExtra->Release();
		zCbBufferPS->Release();
		zCbBufferUV->Release();
		zCbBufferVS->Release();
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

	void CDrawManager::RemoveAnimation(Animation* a)
	{
		RemoveSprite(a);
	}

	BitmapText* CDrawManager::AddBitmapText(BitmapFont* font, CRenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		if (font == nullptr)
			return nullptr;
		BitmapText* newText = new BitmapText(font);
		target->zTexts.push_back(newText);
		newText->zRenderTarget = target;
		newText->zIndex = (int)target->zTexts.size() - 1;
		return newText;
	}

	void CDrawManager::AddBitmapText(BitmapText* text, CRenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		if (zHasObject(text))
			return;
		target->zTexts.push_back(text);
		text->zRenderTarget = target;
		text->zIndex = (int)target->zTexts.size() - 1;
	}

	void CDrawManager::RemoveBitmapText(BitmapText* t)
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

	BitmapFont* CDrawManager::GetDefaultFont()
	{
		return zDefaultFont;
	}

	CRenderTarget* CDrawManager::AddRenderTarget()
	{
		zRenderTargets.push_back(new CRenderTarget());
		return *(zRenderTargets.end() - 1);
	}

	void CDrawManager::RemoveRenderTarget(CRenderTarget* target)
	{
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
		
		CTexture* tex1 = new CTexture((Pixel*)rc_font, Size(300, 105),L"");
		DrawManager->zDefaultFont = new BitmapFont(tex1,Size(15,21),20);
		DebugManager->Init(DrawManager->GetDefaultFont());
	}

	CTexture* CDrawManager::GetTexture(const wchar_t* filename) const
	{
		CTexture* tex;

		if (ResourceManager->PeekResource(filename, (Resource**)&tex)) // WTF casting ???
			return tex;
		else
		{
			tex = new CTexture(filename);
			ResourceManager->AddResource(tex);
			return tex;
		}
	}
}