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

	CDrawManager::CDrawManager()
	{
		TexFilterCreationMode = TextureFilter::Point;
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
		zWhiteRes = tex->_GetShaderResource();
		delete tex;
		
		CreateSampler(TextureFilter::Point, &zPointSampler);
		CreateSampler(TextureFilter::Linear, &zLineSampler);
	}

	void CDrawManager::zInit()
	{
		zDefaultRenderTarget = AddRenderTarget();
	}

	void CDrawManager::AddPoly(Polygon* p, RenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		if (p->zIndex != -1)
			throw VIVA_ERROR("This poly has a render target already");
		target->AddPoly(p);
	}

	Polygon* CDrawManager::AddPoly(const vector<XMFLOAT2>& points, RenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		Polygon* newPoly = new Polygon(points);
		target->AddPoly(newPoly);
		newPoly->zRenderTarget = target;
		return newPoly;
	}

	Rectangle* CDrawManager::AddRect(const Size& size, RenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		Rectangle* newRect = new Rectangle(size);
		target->AddPoly(newRect);
		return newRect;
	}

	Circle* CDrawManager::AddCircle(float radius, unsigned char resolution, RenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		Circle* newCircle = new Circle(radius, resolution);
		target->AddPoly(newCircle);
		return newCircle;
	}

	Sprite* CDrawManager::AddSprite(const wchar_t* filename, RenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		Sprite* newSprite = new Sprite(filename);
		target->AddSprite(newSprite);
		return newSprite;
	}

	void CDrawManager::AddSprite(Sprite* s, RenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		if (s->zIndex != -1)
			throw VIVA_ERROR("This sprite has a render target already");
		target->AddSprite(s);
	}

	Animation* CDrawManager::AddAnimation(LPCWSTR file, int x, int y, RenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		Animation* newAnimation = new Animation(file, x, y);
		target->AddSprite(newAnimation);
		return newAnimation;
	}

	void CDrawManager::AddAnimation(Animation* a, RenderTarget* target)
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
		float scalex = (20.0f - i*0.001f) * tan(Core->GetCamera()->GetFovAngle() / 2) * Core->GetCamera()->GetAspectRatio();
		float scaley = (20.0f - i*0.001f) * tan(Core->GetCamera()->GetFovAngle() / 2);
		//move every target above previous one
		XMMATRIX transform = DirectX::XMMatrixScaling(scalex, scaley, 1) *
			DirectX::XMMatrixTranslation(0, 0, -i*0.001f) * zRenderTargetMatrix;

		transform = XMMatrixTranspose(transform);
		Core->zContext->UpdateSubresource(zCbBufferVS, 0, NULL, &transform, 0, 0);
	}

	void CDrawManager::zDrawAll()
	{
		for (int i = 0; i < (int)zRenderTargets.size(); i++)
			zRenderTargets[i]->_DrawObjects();

		Core->zContext->ClearRenderTargetView(Core->zBackBuffer, Core->zBackBufferColor);
		Core->zContext->ClearDepthStencilView(Core->zDepthStencilView,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		Core->zContext->OMSetRenderTargets(1, &Core->zBackBuffer, Core->zDepthStencilView);
		
		for (int i = 0; i < (int)zRenderTargets.size(); i++)
		{
			zRenderTargetTransform(i);
			Core->zContext->PSSetShader(zRenderTargets[i]->GetPixelShader(), 0, 0);
			if (zRenderTargets[i]->GetExtraBufferPS() != nullptr)
				Core->zContext->UpdateSubresource(DrawManager->zCbBufferPSExtra, 0, 0, 
					zRenderTargets[i]->GetExtraBufferPS(), 0, 0);
			zRenderTargets[i]->_DrawTarget();
		}

		//debug text
		Core->zContext->PSSetShader(Core->zDefaultPS, 0, 0);
		DebugManager->_Flush();
		if(DebugManager->_GetText()->GetText() != L"")
			DebugManager->_GetText()->_Draw();

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

	void CDrawManager::RemovePoly(Polygon* p)
	{		
		p->zRenderTarget->RemovePoly(p);
	}

	void CDrawManager::RemoveSprite(Sprite* s)
	{
		s->zRenderTarget->RemoveSprite(s);
	}

	void CDrawManager::RemoveAnimation(Animation* a)
	{
		a->zRenderTarget->RemoveSprite(a);
	}

	BitmapText* CDrawManager::AddBitmapText(BitmapFont* font, RenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		if (font == nullptr)
			throw VIVA_ERROR("Font is null");
		BitmapText* newText = new BitmapText(font);
		target->AddText(newText);
		return newText;
	}

	void CDrawManager::AddBitmapText(BitmapText* text, RenderTarget* target)
	{
		if (target == nullptr)
			target = zDefaultRenderTarget;
		if (text->zIndex != -1)
			throw VIVA_ERROR("This sprite has a render target already");
		target->AddText(text);
	}

	void CDrawManager::RemoveBitmapText(BitmapText* t)
	{
		t->zRenderTarget->RemoveText(t);
	}

	BitmapFont* CDrawManager::GetDefaultFont()
	{
		return zDefaultFont;
	}

	RenderTarget* CDrawManager::AddRenderTarget()
	{
		RenderTarget* r = new RenderTarget();
		zRenderTargets.push_back(r);
		return r;
	}

	void CDrawManager::RemoveRenderTarget(RenderTarget* target)
	{
		for (int i = 0; i < (int)zRenderTargets.size(); i++)
		{
			if (target == zRenderTargets[i])
			{
				zRenderTargets.erase(zRenderTargets.begin() + i);
				return;
			}
		}

		throw VIVA_ERROR("Render target not found");
	}

	void CDrawManager::ClearRenderTargets()
	{
		for (int i = (int)zRenderTargets.size() - 1; i >= 0;i--)
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