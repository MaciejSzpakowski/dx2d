#include "Private.h"

namespace dx2d
{
	CDrawManager::CDrawManager()
	{
		D3D11_RASTERIZER_DESC rd;
		ZeroMemory(&rd, sizeof(rd));
		rd.FillMode = D3D11_FILL_WIREFRAME;
		rd.CullMode = D3D11_CULL_NONE;
		Device->CreateRasterizerState(&rd, &wireframe);
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_FRONT;
		Device->CreateRasterizerState(&rd, &solid);		
	}

	void CDrawManager::AddPoly(Polygon* p)
	{
		Polygons.push_back(p);
		p->index = (int)Polygons.size() - 1;
	}

	Polygon* CDrawManager::AddPoly(XMFLOAT2 points[], int n)
	{
		Polygon* newPoly = new Polygon(points, n);
		Polygons.push_back(newPoly);
		newPoly->index = (int)Polygons.size() - 1;
		return newPoly;
	}

	Rectangle* CDrawManager::AddRect(float sizex, float sizey)
	{
		Rectangle* newRect = new Rectangle(sizex,sizey);
		Polygons.push_back(newRect);
		newRect->index = (int)Polygons.size() - 1;
		return newRect;
	}

	Circle* CDrawManager::AddCircle(float radius, unsigned char resolution)
	{
		Circle* newCircle = new Circle(radius, resolution);
		Polygons.push_back(newCircle);
		newCircle->index = (int)Polygons.size() - 1;
		return newCircle;
	}

	Sprite* CDrawManager::AddSprite(const WCHAR* textureFile)
	{
		Sprite* newSprite = new Sprite(textureFile);
		Sprites.push_back(newSprite);
		newSprite->index = (int)Sprites.size() - 1;
		return newSprite;
	}

	void CDrawManager::AddSprite(Sprite* s)
	{
		Sprites.push_back(s);
		s->index = (int)Sprites.size() - 1;
	}

	Text* CDrawManager::AddText(std::wstring text)
	{
		Text* newText = new Text(text);
		Sprites.push_back(newText);
		newText->index = (int)Sprites.size() - 1;
		return newText;
	}

	void CDrawManager::AddText(Text* t)
	{
		AddSprite(t);
	}

	void CDrawManager::DrawAll()
	{
		Context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
		Context->RSSetState(wireframe);
		for (Polygon* p : Polygons)
		{
			p->Transform();
			if (p->Visible)
			{				
				p->Draw();
			}
		}
		
		Context->RSSetState(solid);
		Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		for (Sprite* s : Sprites)
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
		wireframe->Release();
		solid->Release();
		delete this;
	}

	void CDrawManager::RemovePoly(Polygon* p)
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

	void CDrawManager::RemoveSprite(Sprite* s)
	{
		if (s->index == -1)
			return;
		if (Sprites.size() == 0 || Sprites[s->index] != s)
		{
			MessageBox(0, "This sprite is not in CDrawManager.Sprites", "Error", MB_ICONERROR);
			return;
		}
		else if (s->index == Polygons.size() - 1)
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

	void CDrawManager::RemoveText(Text* t)
	{
		RemoveSprite(t);
	}
}