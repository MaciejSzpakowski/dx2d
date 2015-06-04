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

	void CDrawManager::Add(Polygon* p)
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

	void CDrawManager::DrawAll()
	{
		Context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
		Context->RSSetState(wireframe);
		for (Polygon* p : Polygons)
			if (p->Visible)
			{
				p->Transform();
				p->Draw();
			}
	}

	void CDrawManager::Destroy()
	{
		for (int i = (int)Polygons.size() - 1; i >= 0; i--)
			Polygons[i]->Destroy();
		wireframe->Release();
		solid->Release();
		delete this;
	}

	void CDrawManager::Remove(Polygon* p)
	{
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
	}
}