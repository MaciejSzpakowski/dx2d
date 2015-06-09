#include "Private.h"

namespace dx2d
{
	CDynamic::CDynamic()
	{
		Position = XMFLOAT3(0, 0, 0);
		Rotation = XMFLOAT3(0, 0, 0);
		Velocity = XMFLOAT3(0, 0, 0);
		Acceleration = XMFLOAT3(0, 0, 0);
		Spin = XMFLOAT3(0, 0, 0);

		//Create the buffer to send to the cbuffer in effect file
		D3D11_BUFFER_DESC cbbd;
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = sizeof(XMMATRIX);
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;

		GetDevice()->CreateBuffer(&cbbd, NULL, &cbBufferVS);
	}

	void CDynamic::Transform()
	{
		Functions::AddFloat3(&Acceleration, &Velocity);
		Functions::AddFloat3(&Velocity, &Position);
		Functions::AddFloat3(&Spin, &Rotation);
		XMMATRIX scale = GetScaleMatrix();
		XMMATRIX rot = DirectX::XMMatrixRotationRollPitchYaw(Rotation.x,Rotation.y,Rotation.z);
		XMMATRIX loc = DirectX::XMMatrixTranslation(Position.x, Position.y, Position.z);
		XMMATRIX world = scale * rot * loc;
		XMMATRIX worldViewProj = world * Camera->view * Camera->proj;
		worldViewProj = DirectX::XMMatrixTranspose(worldViewProj);
		GetContext()->UpdateSubresource(cbBufferVS, 0, NULL, &worldViewProj, 0, 0);
		GetContext()->VSSetConstantBuffers(0, 1, &cbBufferVS);
	}

	CDynamic::~CDynamic()
	{
		cbBufferVS->Release();
	}
}