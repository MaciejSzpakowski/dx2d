#include "Private.h"

namespace dx2d
{
	CDrawManager::CDrawManager()
	{
		D3D11_RASTERIZER_DESC rd;
		ZeroMemory(&rd, sizeof(rd));
		rd.FillMode = D3D11_FILL_WIREFRAME;
		rd.CullMode = D3D11_CULL_NONE;
		GetDevice()->CreateRasterizerState(&rd, &wireframe);
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_FRONT;
		GetDevice()->CreateRasterizerState(&rd, &solid);

		//sprite shared buffer
		VERTEX v[] =
		{
			{ -1.0f, -1.0f, 0, 0, 0, 0, 0, 0 },
			{ 1.0f, -1.0f, 0, 0, 0, 0, 1, 0 },
			{ 1.0f, 1.0f, 0, 0, 0, 0, 1, 1 },
			{ -1.0f, 1.0f, 0, 0, 0, 0, 0, 1 },
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

		GetContext()->IASetIndexBuffer(indexBufferSprite, DXGI_FORMAT_R32_UINT, 0);

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VERTEX) * 4;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = v;
		GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBufferSprite);

		//default white texture used by non textured objects
		BYTE fourOnes[4] = { 1, 1, 1, 1 };
		ID3D11Texture2D* tex = Functions::CreateTexture2D(fourOnes, 1, 1);
		GetDevice()->CreateShaderResourceView(tex, 0, &whiteRes);
		tex->Release();

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		GetDevice()->CreateSamplerState(&sampDesc, &whiteSam);
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
		CSprite* newSprite = new CSprite(textureFile);
		Sprites.push_back(newSprite);
		newSprite->index = (int)Sprites.size() - 1;
		return newSprite;
	}

	void CDrawManager::AddSprite(CSprite* s)
	{
		Sprites.push_back(s);
		s->index = (int)Sprites.size() - 1;
	}

	CText* CDrawManager::AddText(std::wstring text)
	{
		CText* newText = new CText(text);
		Sprites.push_back(newText);
		newText->index = (int)Sprites.size() - 1;
		return newText;
	}

	void CDrawManager::AddText(CText* t)
	{
		AddSprite(t);
	}

	void CDrawManager::DrawAll()
	{
		GetContext()->ClearDepthStencilView(Core->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		//GetContext()->OMSetBlendState(Core->blendState, 0, 0xffffffff);
		GetContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
		GetContext()->RSSetState(wireframe);
		for (CPolygon* p : Polygons)
		{
			p->Transform();
			if (p->Visible)
			{				
				p->Draw();
			}
		}
		
		GetContext()->RSSetState(solid);
		GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		GetContext()->IASetVertexBuffers(0, 1, &vertexBufferSprite, &stride, &offset);
		for (CSprite* s : Sprites)
		{
			s->Transform();			
			if (s->Visible)
			{				
				s->Draw();
			}
		}
	}

	void CDrawManager::Destroy()
	{
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
			MessageBox(0, "This polygon is not in CDrawManager.Polygons", "Error", MB_ICONERROR);
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
			MessageBox(0, "This sprite is not in CDrawManager.Sprites", "Error", MB_ICONERROR);
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

	void CDrawManager::RemoveText(CText* t)
	{
		RemoveSprite(t);
	}
}