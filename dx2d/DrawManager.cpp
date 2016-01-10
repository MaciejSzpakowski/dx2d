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
		Core->_GetDevice()->CreateSamplerState(&sampDesc, sampler);
	}

	CDrawManager::CDrawManager()
	{
		defaultTextureFilter = TextureFilter::Point;
		defaultRenderTarget = new RenderTarget();
		renderTargets.push_back(defaultRenderTarget);

		D3D11_RASTERIZER_DESC rd;
		ZeroMemory(&rd, sizeof(rd));
		rd.FillMode = D3D11_FILL_WIREFRAME;
		rd.CullMode = D3D11_CULL_NONE;
		HRESULT hr = Core->_GetDevice()->CreateRasterizerState(&rd, &wireframe); CHECKHR();
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_FRONT;
		hr = Core->_GetDevice()->CreateRasterizerState(&rd, &solid); CHECKHR();

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
		Core->_GetDevice()->CreateBuffer(&indexBufferDesc, &srd, &indexBufferSprite);

		//so far the only indexed buffer in the engine
		Core->_GetContext()->IASetIndexBuffer(indexBufferSprite, DXGI_FORMAT_R32_UINT, 0);

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
		Core->_GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBufferSprite);

		//shared vertex shader buffer
		D3D11_BUFFER_DESC cbbd;
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = sizeof(XMMATRIX);
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;
		Core->_GetDevice()->CreateBuffer(&cbbd, NULL, &constantBufferVS);
		Core->_GetContext()->VSSetConstantBuffers(0, 1, &constantBufferVS);

		//shared vertex shader 2nd buffer and pixel shader buffer
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = sizeof(Color);
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;
		Core->_GetDevice()->CreateBuffer(&cbbd, NULL, &constantBufferPS);
		Core->_GetDevice()->CreateBuffer(&cbbd, NULL, &constantBufferUV); //has the same size
		Core->_GetContext()->PSSetConstantBuffers(0, 1, &constantBufferPS);
		Core->_GetContext()->VSSetConstantBuffers(1, 1, &constantBufferUV);
		Core->_GetDevice()->CreateBuffer(&cbbd, NULL, &constantBufferPSExtra); // also same size
		Core->_GetContext()->PSSetConstantBuffers(1, 1, &constantBufferPSExtra);
				
		CreateSampler(TextureFilter::Point, &pointSampler);
		CreateSampler(TextureFilter::Linear, &lineSampler);

		_InitDefaultFont();
	}

	void CDrawManager::AddPoly(Polygon* p, RenderTarget* target)
	{
		if (target == nullptr)
			target = defaultRenderTarget;
		if (p->_GetIndex() != -1)
			throw VIVA_ERROR("This poly has a render target already");
		target->AddPoly(p);
	}

	Polygon* CDrawManager::AddPoly(const vector<XMFLOAT2>& points, RenderTarget* target)
	{
		if (target == nullptr)
			target = defaultRenderTarget;
		Polygon* newPoly = new Polygon(points);
		target->AddPoly(newPoly);
		newPoly->_SetRenderTarget(target);
		return newPoly;
	}

	Rectangle* CDrawManager::AddRect(const Size& size, RenderTarget* target)
	{
		if (target == nullptr)
			target = defaultRenderTarget;
		Rectangle* newRect = new Rectangle(size);
		target->AddPoly(newRect);
		return newRect;
	}

	Circle* CDrawManager::AddCircle(float radius, unsigned char resolution, RenderTarget* target)
	{
		if (target == nullptr)
			target = defaultRenderTarget;
		Circle* newCircle = new Circle(radius, resolution);
		target->AddPoly(newCircle);
		return newCircle;
	}

	Sprite* CDrawManager::AddSprite(const wchar_t* filename, RenderTarget* target)
	{
		if (target == nullptr)
			target = defaultRenderTarget;
		Sprite* newSprite = new Sprite(filename);
		target->AddSprite(newSprite);
		return newSprite;
	}

	void CDrawManager::AddSprite(Sprite* s, RenderTarget* target)
	{
		if (target == nullptr)
			target = defaultRenderTarget;
		if (s->_GetIndex() != -1)
			throw VIVA_ERROR("This sprite has a render target already");
		target->AddSprite(s);
	}

	Animation* CDrawManager::AddAnimation(LPCWSTR file, int x, int y, RenderTarget* target)
	{
		if (target == nullptr)
			target = defaultRenderTarget;
		Animation* newAnimation = new Animation(file, x, y);
		target->AddSprite(newAnimation);
		return newAnimation;
	}

	void CDrawManager::AddAnimation(Animation* a, RenderTarget* target)
	{
		if (target == nullptr)
			target = defaultRenderTarget;
		AddSprite(a, target);
	}

	void CDrawManager::SetExtraBufferSize(UINT size)
	{
		if (size % 16 != 0 || size == 0)
			throw VIVA_ERROR("Buffer size is not multiple of 16 or it's 0");

		D3D11_BUFFER_DESC cbbd;
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = size;
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;
		constantBufferPSExtra->Release();
		Core->_GetDevice()->CreateBuffer(&cbbd, NULL, &constantBufferPSExtra);
		Core->_GetContext()->PSSetConstantBuffers(1, 1, &constantBufferPSExtra);
	}

	void CDrawManager::_RenderTargetTransform(int i)
	{
		float scalex = (20.0f - i*0.001f) * tan(Core->GetCamera()->GetFovAngle() / 2) 
			* Core->GetCamera()->GetAspectRatio();
		float scaley = (20.0f - i*0.001f) * tan(Core->GetCamera()->GetFovAngle() / 2);
		//move every target above previous one
		XMMATRIX transform = DirectX::XMMatrixScaling(scalex, scaley, 1) *
			DirectX::XMMatrixTranslation(0, 0, -i*0.001f) * renderTargetMatrix;

		transform = XMMatrixTranspose(transform);
		Core->_GetContext()->UpdateSubresource(constantBufferVS, 0, NULL, &transform, 0, 0);
	}

	void CDrawManager::_DrawAll()
	{
		for (int i = 0; i < (int)renderTargets.size(); i++)
			renderTargets[i]->_DrawObjects();

		Core->_GetContext()->ClearRenderTargetView(Core->_GetBackBuffer(), Core->_GetBackBufferColor());
		Core->_GetContext()->ClearDepthStencilView(Core->_GetDepthStencilView(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		Core->_GetContext()->OMSetRenderTargets(1, Core->_GetBackBufferAddress(), Core->_GetDepthStencilView());
		
		for (int i = 0; i < (int)renderTargets.size(); i++)
		{
			_RenderTargetTransform(i);
			Core->_GetContext()->PSSetShader(renderTargets[i]->GetPixelShader(), 0, 0);
			if (renderTargets[i]->GetExtraBufferPS() != nullptr)
				Core->_GetContext()->UpdateSubresource(DrawManager->constantBufferPSExtra, 
					0, 0, renderTargets[i]->GetExtraBufferPS(), 0, 0);
			renderTargets[i]->_DrawTarget();
		}

		//debug text
		Core->_GetContext()->PSSetShader(Core->_GetDefaultPS(), 0, 0);
		DebugManager->_Flush();
		if(DebugManager->_GetText()->GetText().length() > 0)
			DebugManager->_GetText()->_Draw();

		Core->_GetSwapChain()->Present(0, 0);
	}

	void CDrawManager::Destroy()
	{
		for (int i = (int)renderTargets.size() - 1; i >= 0; i--)
			renderTargets[i]->Destroy();
		constantBufferPSExtra->Release();
		constantBufferPS->Release();
		constantBufferUV->Release();
		constantBufferVS->Release();
		lineSampler->Release();
		pointSampler->Release();
		indexBufferSprite->Release();
		vertexBufferSprite->Release();
		wireframe->Release();
		solid->Release();
		delete this;
	}

	void CDrawManager::RemovePoly(Polygon* p)
	{		
		p->GetRenderTarget()->RemovePoly(p);
	}

	void CDrawManager::RemoveSprite(Sprite* s)
	{
		s->GetRenderTarget()->RemoveSprite(s);
	}

	void CDrawManager::RemoveAnimation(Animation* a)
	{
		a->GetRenderTarget()->RemoveSprite(a);
	}

	BitmapText* CDrawManager::AddBitmapText(BitmapFont* font, RenderTarget* target)
	{
		if (target == nullptr)
			target = defaultRenderTarget;
		if (font == nullptr)
			throw VIVA_ERROR("Font is null");
		BitmapText* newText = new BitmapText(font);
		target->AddText(newText);
		return newText;
	}

	void CDrawManager::AddBitmapText(BitmapText* text, RenderTarget* target)
	{
		if (target == nullptr)
			target = defaultRenderTarget;
		if (text->_GetIndex() != -1)
			throw VIVA_ERROR("This sprite has a render target already");
		target->AddText(text);
	}

	void CDrawManager::RemoveBitmapText(BitmapText* t)
	{
		t->GetRenderTarget()->RemoveText(t);
	}

	RenderTarget* CDrawManager::AddRenderTarget()
	{
		RenderTarget* r = new RenderTarget();
		renderTargets.push_back(r);
		return r;
	}

	void CDrawManager::AddRenderTarget(RenderTarget* r)
	{
		renderTargets.push_back(r);
	}

	void CDrawManager::RemoveRenderTarget(RenderTarget* target)
	{
		for (int i = 0; i < (int)renderTargets.size(); i++)
		{
			if (target == renderTargets[i])
			{
				renderTargets.erase(renderTargets.begin() + i);
				return;
			}
		}

		throw VIVA_ERROR("Render target not found");
	}

	void CDrawManager::ClearRenderTargets()
	{
		for (int i = (int)renderTargets.size() - 1; i >= 0;i--)
			if (renderTargets[i] != defaultRenderTarget)
				renderTargets[i]->Destroy();
		renderTargets.clear();
		renderTargets.push_back(defaultRenderTarget);
	}

	extern const int rc_font[];
	void CDrawManager::_InitDefaultFont()
	{
		//default font
		//10x19 one char
		//19x5 all chars

		Texture* tex1 = new Texture((Pixel*)rc_font, Size(190, 95),L"defaultFont");
		defaultFont = new BitmapFont(tex1,Size(10,19),19);
	}

	Texture* CDrawManager::GetTexture(const wchar_t* filename) const
	{
		Texture* tex;

		if (ResourceManager->PeekResource(filename, (Resource**)&tex)) // WTF casting ???
			return tex;
		else
		{
			tex = new Texture(filename);
			ResourceManager->AddResource(tex);
			return tex;
		}
	}

	void CDrawManager::MoveRenderTargetToBottom(RenderTarget* t)
	{
		int size = (int)renderTargets.size();
		if (size < 2)
			return;
		int index = -1;
		for (int i = 0; i<size; i++)
			if (renderTargets[i] == t)
			{
				index = i;
				break;
			}
		if (index == -1)
			throw VIVA_ERROR("Render target not in draw manager");
		renderTargets.erase(renderTargets.begin() + index);
		renderTargets.insert(renderTargets.begin(), t);
	}

	void CDrawManager::MoveRenderTargetToTop(RenderTarget* t)
	{
		int size = (int)renderTargets.size();
		if (size < 2)
			return;
		int index = -1;
		for (int i = 0; i<size; i++)
			if (renderTargets[i] == t)
			{
				index = i;
				break;
			}
		if (index == -1)
			throw VIVA_ERROR("Render target not in draw manager");
		renderTargets.erase(renderTargets.begin() + index);
		renderTargets.push_back(t);
	}

	void CDrawManager::MoveRenderTargetUp(RenderTarget* t)
	{
		int size = (int)renderTargets.size();
		if (size < 2)
			return;
		int index = -1;
		for (int i = 0; i<size; i++)
			if (renderTargets[i] == t)
			{
				index = i;
				break;
			}
		if (index == -1)
			throw VIVA_ERROR("Render target not in draw manager");
		if (index == size - 1)
			return;
		renderTargets[index] = renderTargets[index + 1];
		renderTargets[index + 1] = t;
	}

	void CDrawManager::MoveRenderTargetDown(RenderTarget* t)
	{
		int size = (int)renderTargets.size();
		if (size < 2)
			return;
		int index = -1;
		for (int i = 0; i<size; i++)
			if (renderTargets[i] == t)
			{
				index = i;
				break;
			}
		if (index == -1)
			throw VIVA_ERROR("Render target not in draw manager");
		if (index == 0)
			return;
		renderTargets[index] = renderTargets[index - 1];
		renderTargets[index - 1] = t;
	}
}