#include "Private.h"

ID3D11Device* GetDevice();
ID3D11DeviceContext* GetContext();

namespace dx2d
{
	CDrawable::CDrawable()
	{
		Visible = true;
		uv = UV(0, 0, 1, 1);

		D3D11_BUFFER_DESC cbbd;
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = sizeof(Color);
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;
		GetDevice()->CreateBuffer(&cbbd, NULL, &cbBufferPS);
		GetDevice()->CreateBuffer(&cbbd, NULL, &cbBufferUV); //has the same size
	}

	CDrawable::~CDrawable()
	{
		cbBufferPS->Release();
		cbBufferUV->Release();
	}

	CPolygon::CPolygon()
	{}

	CPolygon::CPolygon(XMFLOAT2 points[], int n)
	{
		index = -1;
		Color = SColor(0, 0, 0, 0);
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
		bd.ByteWidth = sizeof(Vertex) * n;	           // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = 0;		                   // CPU does nothing

		Vertex* vertices = new Vertex[n];
		for (int i = 0; i < n; i++)
			vertices[i] = { points[i].x, points[i].y, 0, 1, 1, 1, 0, 0 };

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = vertices;                   //Memory in CPU to copy in to GPU

		GetDevice()->CreateBuffer(&bd, &sd, &vertexBuffer);
		delete[] vertices;//*/
	}

	XMMATRIX CPolygon::GetScaleMatrix()
	{
		return DirectX::XMMatrixIdentity();
	}

	CRectangle::CRectangle(float scalex, float scaley)
	{
		vertexCount = 5;
		Scale.x = scalex;
		Scale.y = scaley;
		//method 2
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;				   // GPU writes and reads
		bd.ByteWidth = sizeof(Vertex) * vertexCount;	           // size is the VERTEX struct * 3
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

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = &vertices;                   //Memory in CPU to copy in to GPU

		GetDevice()->CreateBuffer(&bd, &sd, &vertexBuffer);
	}

	XMMATRIX CRectangle::GetScaleMatrix()
	{
		return DirectX::XMMatrixScaling(Scale.x, Scale.y, 1);
	}

	CCircle::CCircle(float radius, unsigned char resolution)
	{
		vertexCount = resolution + 1;
		Radius = radius;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;				   // GPU writes and reads
		bd.ByteWidth = sizeof(Vertex) * vertexCount;	           // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = 0;		                   // CPU does nothing

		Vertex* vertices = new Vertex[vertexCount];
		float angle = 0;
		float delta = XM_2PI / (vertexCount - 1);
		for (int i = 0; i < vertexCount; i++)
		{
			vertices[i] = { cos(angle)*radius, sin(angle)*radius, 0, 1, 1, 1, 0, 0 };
			angle += delta;
		}

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = vertices;                   //Memory in CPU to copy in to GPU

		GetDevice()->CreateBuffer(&bd, &sd, &vertexBuffer);
		delete[] vertices;
	}

	XMMATRIX CCircle::GetScaleMatrix()
	{
		return DirectX::XMMatrixScaling(Radius, Radius, 1);
	}

	void CPolygon::Draw()
	{		
		GetContext()->UpdateSubresource(cbBufferPS, 0, NULL, &Color, 0, 0);
		GetContext()->PSSetConstantBuffers(0, 1, &cbBufferPS);
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		GetContext()->Draw(vertexCount, 0);
	}

	void CPolygon::Destroy()
	{
		DrawManager->RemovePoly(this);
		vertexBuffer->Release();
		delete this;
	}
}