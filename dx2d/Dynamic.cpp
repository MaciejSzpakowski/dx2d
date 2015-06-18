#include "Private.h"

namespace dx2d
{
	void AddFloat3(XMFLOAT3* src, XMFLOAT3* dst);

	CDynamic::CDynamic()
	{
		Position = XMFLOAT3(0, 0, 0);
		Rotation = XMFLOAT3(0, 0, 0);
		Velocity = XMFLOAT3(0, 0, 0);
		Acceleration = XMFLOAT3(0, 0, 0);
		AngularVel = XMFLOAT3(0, 0, 0);
		AngularAcc = XMFLOAT3(0, 0, 0);
		Parent = nullptr;

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
		XMMATRIX scale = GetScaleMatrix();
		XMMATRIX rot = DirectX::XMMatrixRotationRollPitchYaw(Rotation.x,Rotation.y,Rotation.z);
		XMMATRIX loc = DirectX::XMMatrixTranslation(Position.x, Position.y, Position.z);
		XMMATRIX world = scale * rot * loc;
		XMMATRIX worldViewProj = world * Camera->view * Camera->proj;
		if (Parent != nullptr)
		{
			XMMATRIX parentLoc = DirectX::XMMatrixRotationRollPitchYaw(0, 0, Parent->Rotation.z);
			XMMATRIX parentRot = DirectX::XMMatrixTranslation(Parent->Position.x, Parent->Position.y, Parent->Position.z);
			worldViewProj = world * parentLoc * parentRot * Camera->view * Camera->proj;
		}
		worldViewProj = DirectX::XMMatrixTranspose(worldViewProj);
		GetContext()->UpdateSubresource(cbBufferVS, 0, NULL, &worldViewProj, 0, 0);
		GetContext()->VSSetConstantBuffers(0, 1, &cbBufferVS);
	}

	void CDynamic::Update()
	{
		AddFloat3(&Acceleration, &Velocity);
		AddFloat3(&Velocity, &Position);
		AddFloat3(&AngularAcc, &AngularVel);
		AddFloat3(&AngularVel, &Rotation);
	}

	CDynamic::~CDynamic()
	{
		cbBufferVS->Release();
	}
}