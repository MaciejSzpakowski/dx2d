#include "Private.h"

namespace dx2d
{
	Polygon::Polygon()
	{}

	Polygon::Polygon(XMFLOAT2 points[], int n)
	{
		vertexCount = n;

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
		bd.ByteWidth = sizeof(VERTEX) * n;	           // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = 0;		                   // CPU does nothing

		VERTEX* vertices = new VERTEX[n];
		for (int i = 0; i < n; i++)
			vertices[i] = { points[i].x, points[i].y, 0.0f, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = vertices;                   //Memory in CPU to copy in to GPU

		Device->CreateBuffer(&bd, &sd, &vertexBuffer);
		delete[] vertices;//*/
	}

	Rectangle::Rectangle(float scalex, float scaley)
	{
		vertexCount = 5;
		//method 2
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;				   // GPU writes and reads
		bd.ByteWidth = sizeof(VERTEX) * 5;	           // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = 0;		                   // CPU does nothing

		VERTEX vertices[] = 
		{
			{ -0.1f, -0.1f, 0.0f, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
			{ 0.1f, -0.1f, 0.0f, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
			{ 0.1f, 0.1f, 0.0f, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
			{ -0.1f, 0.1f, 0.0f, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
			{ -0.1f, -0.1f, 0.0f, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) }
		};

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = &vertices;                   //Memory in CPU to copy in to GPU

		Device->CreateBuffer(&bd, &sd, &vertexBuffer);
	}

	void Polygon::Draw()
	{
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		Context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);		
		Context->Draw(vertexCount, 0);
	}

	void Polygon::Destroy()
	{
		DrawManager->Remove(this);
		vertexBuffer->Release();
		delete this;
	}
}