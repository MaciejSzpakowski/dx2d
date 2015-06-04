#include "Private.h"

namespace dx2d
{
	SColor::SColor(float _r, float _g, float _b, float _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	Drawable::Drawable()
	{
		Visible = true;
		//Create the buffer to send to the cbuffer in effect file
		D3D11_BUFFER_DESC cbbd;
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = sizeof(Color);
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;
		Device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer4);
	}

	Drawable::~Drawable()
	{
		cbPerObjectBuffer4->Release();
	}

	Polygon::Polygon()
	{}

	Polygon::Polygon(XMFLOAT2 points[], int n)
	{
		Color = SColor(0, 0, 0, 1);
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
			vertices[i] = { points[i].x, points[i].y, 0.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) };

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = vertices;                   //Memory in CPU to copy in to GPU

		Device->CreateBuffer(&bd, &sd, &vertexBuffer);
		delete[] vertices;//*/
	}

	XMMATRIX Polygon::GetScaleMatrix()
	{
		return DirectX::XMMatrixIdentity();
	}

	Rectangle::Rectangle(float scalex, float scaley)
	{
		vertexCount = 5;
		ScaleX = scalex;
		ScaleY = scaley;
		//method 2
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;				   // GPU writes and reads
		bd.ByteWidth = sizeof(VERTEX) * vertexCount;	           // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = 0;		                   // CPU does nothing

		VERTEX vertices[] = 
		{
			{ -0.5f, -0.5f, 0.0f, XMFLOAT4(1, 1, 1, 1.0f) },
			{ 0.5f, -0.5f, 0.0f, XMFLOAT4(1, 1, 1, 1.0f) },
			{ 0.5f, 0.5f, 0.0f, XMFLOAT4(1, 1, 1, 1.0f) },
			{ -0.5f, 0.5f, 0.0f, XMFLOAT4(1, 1, 1, 1.0f) },
			{ -0.5f, -0.5f, 0.0f, XMFLOAT4(1, 1, 1, 1.0f) }
		};

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = &vertices;                   //Memory in CPU to copy in to GPU

		Device->CreateBuffer(&bd, &sd, &vertexBuffer);
	}

	XMMATRIX Rectangle::GetScaleMatrix()
	{
		return DirectX::XMMatrixScaling(ScaleX, ScaleY, 1);
	}

	Circle::Circle(float radius, unsigned char resolution)
	{
		vertexCount = resolution + 1;
		Radius = radius;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;				   // GPU writes and reads
		bd.ByteWidth = sizeof(VERTEX) * vertexCount;	           // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = 0;		                   // CPU does nothing

		VERTEX* vertices = new VERTEX[vertexCount];
		float angle = 0;
		float delta = XM_2PI / (vertexCount - 1);
		for (int i = 0; i < vertexCount; i++)
		{
			vertices[i] = { cos(angle)*radius, sin(angle)*radius, 0.0f, XMFLOAT4(1, 1, 1, 1.0f) };
			angle += delta;
		}

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = vertices;                   //Memory in CPU to copy in to GPU

		Device->CreateBuffer(&bd, &sd, &vertexBuffer);
		delete[] vertices;
	}

	XMMATRIX Circle::GetScaleMatrix()
	{
		return DirectX::XMMatrixScaling(Radius, Radius, 1);
	}

	void Polygon::Draw()
	{
		Context->UpdateSubresource(cbPerObjectBuffer4, 0, NULL, &Color, 0, 0);
		Context->PSSetConstantBuffers(0, 1, &cbPerObjectBuffer4);
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