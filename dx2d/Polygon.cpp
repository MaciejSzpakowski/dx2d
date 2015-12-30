#include "Viva.h"

namespace Viva
{
	Polygon::Polygon(const vector<XMFLOAT2>& points)
	{
		color = XMFLOAT4(0, 0, 0, 0);
		zVertexCount = (int)points.size();
		span = 0;

		/*/method 1
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;                // CPU writes, GPU reads
		bd.ByteWidth = sizeof(VERTEX) * 3;             // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU writes
		Device->CreateBuffer(&bd, NULL, &vertexBuffer);

		VERTEX* vertices = new VERTEX[n];
		for (int i = 0; i < n; i++)
			vertices[i] = { points[i].x, points[i].y, 0.0f, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };

		D3D11_MAPPED_SUBRESOURCE ms;
		Context->Map(vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, vertices, sizeof(VERTEX)*n);
		Context->Unmap(vertexBuffer, NULL);
		delete[] vertices;//*/

		//method 2
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;				   // GPU writes and reads
		bd.ByteWidth = (UINT)(sizeof(Vertex) * points.size()); // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = 0;		                   // CPU does nothing

		Vertex* vertices = new Vertex[points.size()];
		for (int i = 0; i < points.size(); i++)
		{
			float distFromOrigin = sqrtf(points[i].x*points[i].x + points[i].y*points[i].y);
			if (distFromOrigin > span)
				span = distFromOrigin;
			zVertices.push_back(DirectX::XMVectorSet(points[i].x, points[i].y,0,0));
			vertices[i] = { points[i].x, points[i].y, 0, 1, 1, 1, 0, 0 };
		}
		zTransformedVertices = zVertices;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = vertices;                   //Memory in CPU to copy in to GPU

		Core->zDevice->CreateBuffer(&bd, &sd, &zVertexBuffer);
		delete[] vertices;//*/
	}

	Polygon* Polygon::Clone()
	{
		Polygon* newPoly = new Polygon(*this);
		newPoly->zIndex = -1;
		newPoly->zChildren.clear();
		DrawManager->AddPoly(newPoly, newPoly->zRenderTarget);
		return newPoly;
	}

	XMMATRIX Polygon::_GetScaleMatrix()
	{
		return DirectX::XMMatrixIdentity();
	}

	Rectangle::Rectangle(const Size& size)
	{
		zVertexCount = 5;
		scale.x = (float)size.width;
		scale.y = (float)size.height;
		//method 2
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;				   // GPU writes and reads
		bd.ByteWidth = sizeof(Vertex) * zVertexCount;  // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = 0;		                   // CPU does nothing

		Vertex vertices[] =
		{
			{ -0.5f, -0.5f, 0, 1, 1, 1, 0, 0 },
			{ 0.5f, -0.5f, 0, 1, 1,1, 0, 0 },
			{ 0.5f, 0.5f, 0, 1, 1,1, 0, 0 },
			{ -0.5f, 0.5f, 0, 1, 1,1, 0, 0 },
			{ -0.5f, -0.5f, 0, 1, 1,1, 0, 0 }
		};
		zVertices.push_back(DirectX::XMVectorSet(-0.5f, -0.5f, 0, 0));
		zVertices.push_back(DirectX::XMVectorSet(0.5f, -0.5f, 0, 0));
		zVertices.push_back(DirectX::XMVectorSet(0.5f, 0.5f, 0, 0));
		zVertices.push_back(DirectX::XMVectorSet(-0.5f, 0.5f, 0, 0));
		zVertices.push_back(DirectX::XMVectorSet(-0.5f, -0.5f, 0, 0));
		zTransformedVertices = zVertices;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = &vertices;                   //Memory in CPU to copy in to GPU

		Core->zDevice->CreateBuffer(&bd, &sd, &zVertexBuffer);
	}

	XMMATRIX Rectangle::_GetScaleMatrix()
	{
		return DirectX::XMMatrixScaling(scale.x, scale.y, 1);
	}

	Circle::Circle(float _radius, size_t resolution)
	{
		zVertexCount = (int)resolution + 1;
		radius = _radius;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;				   // GPU writes and reads
		bd.ByteWidth = sizeof(Vertex) * zVertexCount;	           // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = 0;		                   // CPU does nothing

		Vertex* vertices = new Vertex[zVertexCount];
		float angle = 0;
		float delta = DirectX::XM_2PI / (zVertexCount - 1);
		for (int i = 0; i < zVertexCount; i++)
		{
			vertices[i] = { cos(angle)*radius, sin(angle)*radius, 0, 1, 1, 1, 0, 0 };
			zVertices.push_back(DirectX::XMVectorSet(vertices[i].X, vertices[i].Y, 0, 0));
			angle += delta;
		}
		zTransformedVertices = zVertices;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = vertices;                   //Memory in CPU to copy in to GPU

		Core->zDevice->CreateBuffer(&bd, &sd, &zVertexBuffer);
		delete[] vertices;
	}

	XMMATRIX Circle::_GetScaleMatrix()
	{
		return DirectX::XMMatrixScaling(radius, radius, 1);
	}

	void Polygon::_Draw()
	{		
		Core->zContext->UpdateSubresource(DrawManager->zCbBufferPS, 0, NULL, &color, 0, 0);		
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		Core->zContext->IASetVertexBuffers(0, 1, &zVertexBuffer, &stride, &offset);
		Core->zContext->Draw(zVertexCount, 0);
	}

	void Polygon::Destroy()
	{
		CDynamic::Destroy();
		if (zIndex != -1)
			DrawManager->RemovePoly(this);
		zVertexBuffer->Release();
		delete this;
	}	
}