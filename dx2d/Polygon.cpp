#include "Viva.h"

namespace Viva
{
	Polygon::Polygon(const vector<XMFLOAT2>& points)
	{
		color = XMFLOAT4(0, 0, 0, 0);
		vertexCount = (int)points.size();
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

		Vertex* temp = new Vertex[points.size()];
		for (int i = 0; i < points.size(); i++)
		{
			float distFromOrigin = sqrtf(points[i].x*points[i].x + points[i].y*points[i].y);
			if (distFromOrigin > span)
				span = distFromOrigin;
			vertices.push_back(DirectX::XMVectorSet(points[i].x, points[i].y,0,0));
			temp[i] = { points[i].x, points[i].y, 0, 1, 1, 1, 0, 0 };
		}
		transformedVertices = vertices;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = temp;                   //Memory in CPU to copy in to GPU

		Core->_GetDevice()->CreateBuffer(&bd, &sd, &vertexBuffer);
		delete[] temp;//*/
	}

	Polygon* Polygon::Clone()
	{
		Polygon* newPoly = new Polygon(*this);
		newPoly->index = -1;
		newPoly->children.clear();
		DrawManager->AddPoly(newPoly, newPoly->renderTarget);
		return newPoly;
	}

	XMMATRIX Polygon::_GetScaleMatrix()
	{
		return DirectX::XMMatrixIdentity();
	}

	Rectangle::Rectangle(const Size& size)
	{
		vertexCount = 5;
		scale.x = (float)size.width;
		scale.y = (float)size.height;
		//method 2
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;				   // GPU writes and reads
		bd.ByteWidth = sizeof(Vertex) * vertexCount;  // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = 0;		                   // CPU does nothing

		Vertex temp[] =
		{
			{ -0.5f, -0.5f, 0, 1, 1, 1, 0, 0 },
			{ 0.5f, -0.5f, 0, 1, 1,1, 0, 0 },
			{ 0.5f, 0.5f, 0, 1, 1,1, 0, 0 },
			{ -0.5f, 0.5f, 0, 1, 1,1, 0, 0 },
			{ -0.5f, -0.5f, 0, 1, 1,1, 0, 0 }
		};
		vertices.push_back(DirectX::XMVectorSet(-0.5f, -0.5f, 0, 0));
		vertices.push_back(DirectX::XMVectorSet(0.5f, -0.5f, 0, 0));
		vertices.push_back(DirectX::XMVectorSet(0.5f, 0.5f, 0, 0));
		vertices.push_back(DirectX::XMVectorSet(-0.5f, 0.5f, 0, 0));
		vertices.push_back(DirectX::XMVectorSet(-0.5f, -0.5f, 0, 0));
		transformedVertices = vertices;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = &temp;                   //Memory in CPU to copy in to GPU

		Core->_GetDevice()->CreateBuffer(&bd, &sd, &vertexBuffer);
	}

	XMMATRIX Rectangle::_GetScaleMatrix()
	{
		return DirectX::XMMatrixScaling(scale.x, scale.y, 1);
	}

	Circle::Circle(float _radius, size_t resolution)
	{
		vertexCount = (int)resolution + 1;
		radius = _radius;
		span = _radius*10;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;				   // GPU writes and reads
		bd.ByteWidth = sizeof(Vertex) * vertexCount;	           // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = 0;		                   // CPU does nothing

		Vertex* temp = new Vertex[vertexCount];
		float angle = 0;
		float delta = DirectX::XM_2PI / (vertexCount - 1);
		for (int i = 0; i < vertexCount; i++)
		{
			temp[i] = { cos(angle)*radius, sin(angle)*radius, 0, 1, 1, 1, 0, 0 };
			vertices.push_back(DirectX::XMVectorSet(temp[i].X, temp[i].Y, 0, 0));
			angle += delta;
		}
		transformedVertices = vertices;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = temp;                   //Memory in CPU to copy in to GPU

		Core->_GetDevice()->CreateBuffer(&bd, &sd, &vertexBuffer);
		delete[] temp;
	}

	XMMATRIX Circle::_GetScaleMatrix()
	{
		return DirectX::XMMatrixScaling(radius, radius, 1);
	}

	void Polygon::_Draw()
	{		
		Core->_GetContext()->UpdateSubresource(DrawManager->_GetConstantBufferPS(), 0, NULL, &color, 0, 0);		
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		Core->_GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		Core->_GetContext()->Draw(vertexCount, 0);
	}

	void Polygon::Destroy()
	{
		Dynamic::Destroy();
		if (index != -1)
			DrawManager->RemovePoly(this);
		vertexBuffer->Release();
		delete this;
	}	
}