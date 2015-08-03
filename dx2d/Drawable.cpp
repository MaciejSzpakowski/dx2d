#include "Private.h"

namespace dx2d
{
	CDrawable::CDrawable()
	{
		zRenderTarget = nullptr;
		Visible = true;
		UV = Rect(0, 0, 1, 1);
		zIndex = -1;
		D3D11_BUFFER_DESC cbbd;
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = sizeof(Color);
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;
		Core->zDevice->CreateBuffer(&cbbd, NULL, &zCbBufferPS);
		Core->zDevice->CreateBuffer(&cbbd, NULL, &zCbBufferUV); //has the same size
		zCbBufferPSExtra = nullptr;
		zExtraBufferData = nullptr;
	}

	void CDrawable::SetConstantBufferPS(void* data, UINT size)
	{
		D3D11_BUFFER_DESC cbbd;
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = size;
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;
		Core->zDevice->CreateBuffer(&cbbd, NULL, &zCbBufferPSExtra);

		zExtraBufferData = data;
	}

	CDrawable::~CDrawable()
	{
		if (zCbBufferPSExtra != nullptr)
			zCbBufferPSExtra->Release();
		zCbBufferPS->Release();
		zCbBufferUV->Release();
	}

	CPolygon::CPolygon()
	{}

	CPolygon::CPolygon(XMFLOAT2 points[], int n)
	{
		Color = XMFLOAT4(0, 0, 0, 0);
		zVertexCount = n;

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
		bd.ByteWidth = sizeof(Vertex) * n;	           // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = 0;		                   // CPU does nothing

		Vertex* vertices = new Vertex[n];
		for (int i = 0; i < n; i++)
		{
			zVertices.push_back(XMVectorSet(points[i].x, points[i].y,0,0));
			vertices[i] = { points[i].x, points[i].y, 0, 1, 1, 1, 0, 0 };
		}

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = vertices;                   //Memory in CPU to copy in to GPU

		Core->zDevice->CreateBuffer(&bd, &sd, &zVertexBuffer);
		delete[] vertices;//*/
	}

	XMMATRIX CPolygon::zGetScaleMatrix()
	{
		return XMMatrixIdentity();
	}

	CRectangle::CRectangle(float scalex, float scaley)
	{
		zVertexCount = 5;
		Scale.x = scalex;
		Scale.y = scaley;
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
		zVertices.push_back(XMVectorSet(-0.5f, -0.5f, 0, 0));
		zVertices.push_back(XMVectorSet(0.5f, -0.5f, 0, 0));
		zVertices.push_back(XMVectorSet(0.5f, 0.5f, 0, 0));
		zVertices.push_back(XMVectorSet(-0.5f, 0.5f, 0, 0));
		zVertices.push_back(XMVectorSet(-0.5f, -0.5f, 0, 0));

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = &vertices;                   //Memory in CPU to copy in to GPU

		Core->zDevice->CreateBuffer(&bd, &sd, &zVertexBuffer);
	}

	XMMATRIX CRectangle::zGetScaleMatrix()
	{
		return XMMatrixScaling(Scale.x, Scale.y, 1);
	}

	CCircle::CCircle(float radius, unsigned char resolution)
	{
		zVertexCount = resolution + 1;
		Radius = radius;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;				   // GPU writes and reads
		bd.ByteWidth = sizeof(Vertex) * zVertexCount;	           // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = 0;		                   // CPU does nothing

		Vertex* vertices = new Vertex[zVertexCount];
		float angle = 0;
		float delta = XM_2PI / (zVertexCount - 1);
		for (int i = 0; i < zVertexCount; i++)
		{
			vertices[i] = { cos(angle)*radius, sin(angle)*radius, 0, 1, 1, 1, 0, 0 };
			angle += delta;
		}

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = vertices;                   //Memory in CPU to copy in to GPU

		Core->zDevice->CreateBuffer(&bd, &sd, &zVertexBuffer);
		delete[] vertices;
	}

	XMMATRIX CCircle::zGetScaleMatrix()
	{
		return XMMatrixScaling(Radius, Radius, 1);
	}

	void CPolygon::zDraw()
	{		
		Core->zContext->UpdateSubresource(zCbBufferPS, 0, NULL, &Color, 0, 0);
		Core->zContext->PSSetConstantBuffers(0, 1, &zCbBufferPS);
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		Core->zContext->IASetVertexBuffers(0, 1, &zVertexBuffer, &stride, &offset);
		Core->zContext->Draw(zVertexCount, 0);
	}

	void CPolygon::Destroy()
	{
		DrawManager->RemovePoly(this);
		zVertexBuffer->Release();
		delete this;
	}	
}