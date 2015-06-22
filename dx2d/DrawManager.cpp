#include "Private.h"

namespace dx2d
{
	void CreateSampler(TEX_FILTER mode, ID3D11SamplerState** sampler);

	CDrawManager::CDrawManager()
	{
		D3D11_RASTERIZER_DESC rd;
		ZeroMemory(&rd, sizeof(rd));
		rd.FillMode = D3D11_FILL_WIREFRAME;
		rd.CullMode = D3D11_CULL_NONE;
		hr = GetDevice()->CreateRasterizerState(&rd, &wireframe); CHECKHR();
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_FRONT;
		hr = GetDevice()->CreateRasterizerState(&rd, &solid); CHECKHR();

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
		GetDevice()->CreateBuffer(&indexBufferDesc, &srd, &indexBufferSprite);

		//so far the only indexed buffer in the engine
		GetContext()->IASetIndexBuffer(indexBufferSprite, DXGI_FORMAT_R32_UINT, 0);

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
		GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBufferSprite);

		//default white texture used by non textured objects
		BYTE fourOnes[4] = { 1, 1, 1, 1 };
		ID3D11Texture2D* tex = Functions::CreateTexture2DFromBytes(fourOnes, 1, 1);
		GetDevice()->CreateShaderResourceView(tex, 0, &whiteRes);
		tex->Release();		
		
		CreateSampler(TEX_FILTER::POINT,&pointSampler);
		CreateSampler(TEX_FILTER::LINEAR, &lineSampler);
		TexFilterCreationMode = TEX_FILTER::POINT;		
	}

	void CDrawManager::AddPoly(CPolygon* p)
	{
		Polygons.push_back(p);
		p->index = (int)Polygons.size() - 1;
	}

	CPolygon* CDrawManager::AddPoly(XMFLOAT2 points[], int n)
	{
		CPolygon* newPoly = new CPolygon(points, n);
		Polygons.push_back(newPoly);
		newPoly->index = (int)Polygons.size() - 1;
		return newPoly;
	}

	CRectangle* CDrawManager::AddRect(float sizex, float sizey)
	{
		CRectangle* newRect = new CRectangle(sizex,sizey);
		Polygons.push_back(newRect);
		newRect->index = (int)Polygons.size() - 1;
		return newRect;
	}

	CCircle* CDrawManager::AddCircle(float radius, unsigned char resolution)
	{
		CCircle* newCircle = new CCircle(radius, resolution);
		Polygons.push_back(newCircle);
		newCircle->index = (int)Polygons.size() - 1;
		return newCircle;
	}

	CSprite* CDrawManager::AddSprite(const WCHAR* textureFile)
	{
		CSprite* newSprite;
		try
		{
			newSprite = new CSprite(textureFile);
		}
		catch (int)
		{
			return nullptr;
		}
		Sprites.push_back(newSprite);
		newSprite->index = (int)Sprites.size() - 1;
		return newSprite;
	}

	void CDrawManager::AddSprite(CSprite* s)
	{
		Sprites.push_back(s);
		s->index = (int)Sprites.size() - 1;
	}

	CAnimation* CDrawManager::AddAnimation(const WCHAR* texture, int x, int y)
	{
		CAnimation* newAnimation;
		try
		{
			newAnimation = new CAnimation(texture, x, y);
		}
		catch (int)
		{
			return nullptr;
		}
		Sprites.push_back(newAnimation);
		newAnimation->index = (int)Sprites.size() - 1;
		return newAnimation;
	}

	void CDrawManager::AddAnimation(CAnimation* a)
	{
		AddSprite(a);
	}

	void CDrawManager::DrawAll()
	{
		GetContext()->ClearDepthStencilView(Core->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		//GetContext()->OMSetBlendState(Core->blendState, 0, 0xffffffff);

		//polys
		GetContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
		GetContext()->RSSetState(wireframe);
		for (CPolygon* p : Polygons)
		{			
			p->Update();
			if (p->Visible)
			{				
				p->Transform();
				p->Draw();
			}
		}
		
		//sprites, animations
		GetContext()->RSSetState(solid);
		GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		GetContext()->IASetVertexBuffers(0, 1, &vertexBufferSprite, &stride, &offset);
		for (CSprite* s : Sprites)
		{
			s->Update();
			s->Play();
			if (s->Visible)
			{
				if(s->TexFilter == TEX_FILTER::LINEAR)
					GetContext()->PSSetSamplers(0, 1, &lineSampler);
				else
					GetContext()->PSSetSamplers(0, 1, &pointSampler);
				s->Transform();
				s->Draw();
			}
		}
		//flush debug
		DebugManager->Flush();
		//bitmap text
		for (CBitmapText* t : Texts)
		{
			t->Update();
			if (t->Visible)
			{
				if (t->TexFilter == TEX_FILTER::LINEAR)
					GetContext()->PSSetSamplers(0, 1, &lineSampler);
				else
					GetContext()->PSSetSamplers(0, 1, &pointSampler);
				t->Draw();
			}
		}
	}

	void CDrawManager::Destroy()
	{
		lineSampler->Release();
		pointSampler->Release();
		for (int i = (int)Polygons.size() - 1; i >= 0; i--)
			Polygons[i]->Destroy();
		for (int i = (int)Sprites.size() - 1; i >= 0; i--)
			Sprites[i]->Destroy();
		indexBufferSprite->Release();
		vertexBufferSprite->Release();
		wireframe->Release();
		solid->Release();
		delete this;
	}

	void CDrawManager::RemovePoly(CPolygon* p)
	{
		if (p->index == -1)
			return;
		if (Polygons.size() == 0 || Polygons[p->index] != p)
		{
			MessageBox(0, L"This polygon is not in CDrawManager.Polygons", L"Error", MB_ICONERROR);
			return;
		}
		else if (p->index == Polygons.size() - 1)
		{			
			Polygons.pop_back();
		}
		else
		{
			//move end to where p is and update index
			Polygons[p->index] = Polygons.back();
			Polygons[p->index]->index = p->index;
			Polygons.pop_back();
		}
		p->index = -1;
	}

	void CDrawManager::RemoveSprite(CSprite* s)
	{
		if (s->index == -1)
			return;
		if (s->index >= Sprites.size() || Sprites[s->index] != s)
		{
			MessageBox(0, L"This sprite is not in CDrawManager.Sprites", L"Error", MB_ICONERROR);
			return;
		}
		else if (s->index == Sprites.size() - 1)
		{
			Sprites.pop_back();
		}
		else
		{
			//move end to where p is and update index
			Sprites[s->index] = Sprites.back();
			Sprites[s->index]->index = s->index;
			Sprites.pop_back();
		}
		s->index = -1;
	}

	void CDrawManager::RemoveAnimation(CAnimation* a)
	{
		RemoveSprite(a);
	}

	CBitmapFont* CDrawManager::AddBitmapFont(const WCHAR* file, vector<UV> chars)
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
		ResourceManager->AddBitmapFont(font);
	}

	CBitmapText* CDrawManager::AddBitmapText(CBitmapFont* font)
	{
		if (font == nullptr)
			return nullptr;
		CBitmapText* newText = new CBitmapText(font);
		Texts.push_back(newText);
		newText->index = (int)Texts.size() - 1;
		return newText;
	}

	void CDrawManager::AddBitmapText(CBitmapText* text)
	{
		Texts.push_back(text);
		text->index = (int)Texts.size() - 1;
	}

	void CDrawManager::RemoveBitmapFont(CBitmapFont* font)
	{
		throw 1;
	}

	void CDrawManager::RemoveBitmapText(CBitmapText* text)
	{
		if (text->index == -1)
			return;
		if (text->index >= Texts.size() || Texts[text->index] != text)
		{
			MessageBox(0, L"This text is not in CDrawManager.Texts", L"Error", MB_ICONERROR);
			return;
		}
		else if (text->index == Texts.size() - 1)
		{
			Texts.pop_back();
		}
		else
		{
			//move end to where p is and update index
			Texts[text->index] = Texts.back();
			Texts[text->index]->index = text->index;
			Texts.pop_back();
		}
		text->index = -1;
	}

	CBitmapFont* CDrawManager::DefaultFont()
	{
		return defaultFont;
	}
}