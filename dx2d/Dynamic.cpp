#include "Private.h"

namespace dx2d
{
	void AddFloat3(XMFLOAT3* src, XMFLOAT3* dst);

	CDynamic::CDynamic()
	{
		SetPosition(0, 0, 0);
		SetRotation(0, 0, 0);
		SetVelocity(0, 0, 0);
		SetAcceleration(0, 0, 0);
		SetAngularVel(0, 0, 0);
		SetAngularAcc(0, 0, 0);
		Parent = nullptr;
		Pickable = false;
		underCursor = false;

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
		XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(Rotation);
		XMMATRIX loc = XMMatrixTranslationFromVector(Position);
		XMMATRIX world = scale * rot * loc;
		XMMATRIX worldViewProj = world * Camera->view * Camera->proj;
		if (Parent != nullptr)
		{
			XMMATRIX parentLoc = XMMatrixRotationRollPitchYawFromVector(Parent->Rotation);
			XMMATRIX parentRot = XMMatrixTranslationFromVector(Parent->Position);
			worldViewProj = world * parentLoc * parentRot * Camera->view * Camera->proj;
		}
		//check for cursor
		if(Pickable)
			CheckForCursor(worldViewProj);
		worldViewProj = XMMatrixTranspose(worldViewProj);
		GetContext()->UpdateSubresource(cbBufferVS, 0, NULL, &worldViewProj, 0, 0);
		GetContext()->VSSetConstantBuffers(0, 1, &cbBufferVS);
	}

	void CDynamic::Update()
	{
		XMVECTOR a = XMVectorScale(Acceleration, Core->GetFrameTime());
		XMVECTOR v = XMVectorScale(Velocity, Core->GetFrameTime());
		Velocity = XMVectorAdd(a, Velocity);
		Position = XMVectorAdd(v, Position);
		XMVECTOR ra = XMVectorScale(AngularAcc, Core->GetFrameTime());
		XMVECTOR rv = XMVectorScale(AngularVel, Core->GetFrameTime());
		AngularVel = XMVectorAdd(ra, AngularVel);
		Rotation = XMVectorAdd(rv, Rotation);
	}

	bool CDynamic::IsUnderCursor()
	{
		return underCursor;
	}

	CDynamic::~CDynamic()
	{
		cbBufferVS->Release();
	}
}