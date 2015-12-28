#include "Viva.h"

namespace Viva
{
	CDynamic::CDynamic()
	{
		zVertexCount = 0;
		zVertexBuffer = nullptr;
		zRenderTarget = nullptr;
		Visible = true;
		UV = Rect(0, 0, 1, 1);
		color = XMFLOAT4(0, 0, 0, 0);
		zIndex = -1;
		zExtraBufferPSdata = nullptr;
		TransformVertices = false;
		zPosition = zAbsolutePosition = XMVectorZero();
		zRotation = zAbsoluteRotation = XMVectorZero();
		zVelocity = XMVectorZero();
		zAcceleration = XMVectorZero();
		zAngularAcc = XMVectorZero();
		zAngularVel = XMVectorZero();
		Origin = { 0, 0 };
		SizeAcceleration = 0;
		SizeVelocity = 0;
		size = 1;
		zParent = nullptr;
		Pickable = false;		
		zUnderCursor = false;
	}

	void CDynamic::SetExtraBufferPS(void* data)
	{
		zExtraBufferPSdata = data;
	}

	void CDynamic::SetRenderTarget(CRenderTarget * target)
	{
		zRenderTarget = target;
	}

	void CDynamic::zTransform()
	{
		if (TransformVertices)
			zTransformVertices();

		zAbsolutePosition = zPosition;
		zAbsoluteRotation = zRotation;		
		XMMATRIX origin = XMMatrixTranslation(-Origin.x, -Origin.y, 0);
		XMMATRIX scale = zGetScaleMatrix();
		XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(zAbsoluteRotation);
		XMMATRIX loc = XMMatrixTranslationFromVector(zAbsolutePosition);
		zWorld = origin * scale * rot * loc;
		if (zParent != nullptr)
		{
			XMMATRIX parentRot = XMMatrixRotationRollPitchYawFromVector(zParent->zAbsoluteRotation);
			XMMATRIX parentLoc = XMMatrixTranslationFromVector(zParent->zAbsolutePosition);
			zAbsolutePosition += zParent->zAbsolutePosition;
			zAbsoluteRotation += zParent->zAbsoluteRotation;
			zAbsolutePosition = XMVector2Transform(zAbsolutePosition, parentRot);
			zWorld = zWorld * parentRot * parentLoc;
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

	void CDynamic::zTransformVertices()
	{
		for (int i = 0; i < zVertexCount; i++)
			zTransformedVertices[i] = XMVector2Transform(zVertices[i], zWorld);
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

	void CDynamic::Destroy()
	{
		//set all childrens parent to nullptr
		for (CDynamic* d : zChildren)
			d->zParent = nullptr;		
	}

	void CDynamic::SetColor(float r, float g, float b, float a)
	{
		color = XMFLOAT4(r, g, b, a);
	}
}