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
		zUnderCursor = false;

		//Create the buffer to send to the cbuffer in effect file
		D3D11_BUFFER_DESC cbbd;
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = sizeof(XMMATRIX);
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;

		Core->zDevice->CreateBuffer(&cbbd, NULL, &zCbBufferVS);
	}

	void CDynamic::zTransform()
	{
		XMMATRIX scale = zGetScaleMatrix();
		XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(zRotation);
		XMMATRIX loc = XMMatrixTranslationFromVector(zPosition);
		zWorld = scale * rot * loc;		
		if (Parent != nullptr)
		{
			XMMATRIX parentLoc = XMMatrixRotationRollPitchYawFromVector(Parent->zRotation);
			XMMATRIX parentRot = XMMatrixTranslationFromVector(Parent->zPosition);
			zWorld = zWorld * parentLoc * parentRot;
		}
		XMMATRIX worldViewProj = zWorld * Camera->zView * Camera->zProj;
		//check for cursor
		if (Pickable)
			zCheckForCursor(zWorld);
		worldViewProj = XMMatrixTranspose(worldViewProj);
		Core->zContext->UpdateSubresource(zCbBufferVS, 0, NULL, &worldViewProj, 0, 0);
		Core->zContext->VSSetConstantBuffers(0, 1, &zCbBufferVS);
	}

	void CDynamic::zUpdate()
	{
		XMVECTOR a = XMVectorScale(zAcceleration, (float)Core->GetFrameTime());
		XMVECTOR v = XMVectorScale(zVelocity, (float)Core->GetFrameTime());
		zVelocity = XMVectorAdd(a,zVelocity);
		zPosition = XMVectorAdd(v, zPosition);
		XMVECTOR ra = XMVectorScale(zAngularAcc, (float)Core->GetFrameTime());
		XMVECTOR rv = XMVectorScale(zAngularVel, (float)Core->GetFrameTime());
		zAngularVel = XMVectorAdd(ra, zAngularVel);
		zRotation = XMVectorAdd(rv, zRotation);
	}

	bool CDynamic::IsUnderCursor()
	{
		return zUnderCursor;
	}

	CDynamic::~CDynamic()
	{
		zCbBufferVS->Release();
	}
}