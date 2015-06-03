#include "Private.h"

namespace dx2d
{
	CDrawManager::CDrawManager()
	{
		D3D11_RASTERIZER_DESC rd;
		ZeroMemory(&rd, sizeof(rd));
		rd.FillMode = D3D11_FILL_WIREFRAME;
		Device->CreateRasterizerState(&rd, &wireframe);
		rd.FillMode = D3D11_FILL_SOLID;
		Device->CreateRasterizerState(&rd, &solid);
	}

	Polygon* CDrawManager::AddPoly(XMFLOAT2 points[], int n)
	{
		Polygon* newPoly = new Polygon(points, n);
		Polygons.push_back(newPoly);
		newPoly->index = Polygons.size() - 1;
		return newPoly;
	}

	void CDrawManager::DrawAll()
	{
		Context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//Context->RSSetState(wireframe);
		for (Polygon* p : Polygons)
			if(p->Visible)
				p->Draw();
	}

	void CDrawManager::Destroy()
	{
		for (int i = Polygons.size() - 1; i >= 0; i++)
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