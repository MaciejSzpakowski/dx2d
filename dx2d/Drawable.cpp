#include "Private.h"

namespace dx2d
{
	Polygon::Polygon(XMFLOAT2 points[], int n)
	{
		vertexCount = n;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
		bd.ByteWidth = sizeof(VERTEX) * 3;             // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer
		Device->CreateBuffer(&bd, NULL, &vertexBuffer);// create the buffer

		VERTEX* vertices = new VERTEX[n];
		for (int i = 0; i < n; i++)
			vertices[i] = { points[i].x, points[i].y, 0.0f, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
		D3D11_MAPPED_SUBRESOURCE ms;
		Context->Map(vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, vertices, sizeof(VERTEX)*n);
		Context->Unmap(vertexBuffer, NULL);
		delete[] vertices;
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