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
		SizeAcceleration = 0;
		SizeVelocity = 0;
		Size = 1;

		zParent = nullptr;
		Pickable = false;		
		zUnderCursor = false;
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
		Core->zContext->UpdateSubresource(DrawManager->zCbBufferVS, 0, NULL, &worldViewProj, 0, 0);
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
		float sa = SizeAcceleration * (float)Core->GetFrameTime();
		SizeVelocity += sa;
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
		//set all childrens parent to nullptr
		for (CDynamic* d : zChildren)
			d->zParent = nullptr;		
	}
}