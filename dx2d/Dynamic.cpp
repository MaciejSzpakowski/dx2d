#include "Private.h"

namespace dx2d
{
	void AddFloat3(XMFLOAT3* src, XMFLOAT3* dst);

	CDynamic::CDynamic()
	{
		zPosition = XMVectorZero();
		zRotation = XMVectorZero();
		zVelocity = XMVectorZero();
		zAcceleration = XMVectorZero();
		zAngularAcc = XMVectorZero();
		zAngularVel = XMVectorZero();
		Origin = { 0, 0 };
		SizeAcc = 0;
		SizeVel = 0;
		Size = 1;

		zParent = nullptr;
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
		XMMATRIX origin = XMMatrixTranslation(-Origin.x, -Origin.y, 0);
		XMMATRIX norigin = XMMatrixTranslation(Origin.x, Origin.y, 0);
		XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(zRotation);		
		XMMATRIX loc = XMMatrixTranslationFromVector(zPosition);
		zWorld = origin * scale * rot * loc * norigin;
		if (zParent != nullptr)
		{
			XMMATRIX parentLoc = XMMatrixRotationRollPitchYawFromVector(zParent->zRotation);
			XMMATRIX parentRot = XMMatrixTranslationFromVector(zParent->zPosition);
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
		XMVECTOR a = zAcceleration * (float)Core->GetFrameTime();
		XMVECTOR v = zVelocity * (float)Core->GetFrameTime();
		zVelocity += a;
		zPosition += v;
		XMVECTOR ra = zAngularAcc * (float)Core->GetFrameTime();
		XMVECTOR rv = zAngularVel * (float)Core->GetFrameTime();
		zAngularVel += ra;
		zRotation += rv;
		float sa = SizeAcc * (float)Core->GetFrameTime();
		SizeVel += sa;
	}

	bool CDynamic::IsUnderCursor()
	{
		return zUnderCursor;
	}

	CDynamic* CDynamic::GetParent()
	{
		return zParent;
	}

	void CDynamic::SetParent(CDynamic* parent)
	{		
		//add this to parent's children if applicable
		if(parent != zParent && parent != nullptr)
			parent->zChildren.push_back(this);

		//remove this from parents children if applicable
		else if (zParent != parent && zParent != nullptr)
		{
			for (int i = 0; i < (int)zParent->zChildren.size(); i++)
			{
				if (zParent->zChildren[i] == this)
				{
					zParent->zChildren.erase(zParent->zChildren.begin() + i);
					break;
				}
			}
		}

		zParent = parent;
	}

	vector<CDynamic*> CDynamic::GetChildren()
	{
		return zChildren;
	}

	XMMATRIX CDynamic::GetWorldMatrix()
	{
		return zWorld;
	}

	CDynamic::~CDynamic()
	{
		zCbBufferVS->Release();
	}
}