#include "Viva.h"

namespace Viva
{
	using namespace DirectX;

	Dynamic::Dynamic()
	{
		name = L"";
		vertexCount = 0;
		renderTarget = nullptr;
		index = -1;
		vertexBuffer = nullptr;
		extraBufferPSdata = nullptr;
		position = XMVectorZero();
		rotation = XMVectorZero();
		velocity = XMVectorZero();
		acceleration = XMVectorZero();
		angularVelocity = XMVectorZero();
		angularAcceleration = XMVectorZero();
		parent = nullptr;
		underCursor = false;
		pickable = false;		
		size = 1;
		sizeVelocity = 0;
		sizeAcceleration = 0;
		origin = { 0, 0 };
		visible = true;
		color = XMFLOAT4(0, 0, 0, 0);
		uv = Rect(0, 0, 1, 1);
		transformVertices = false;		
	}

	void Dynamic::_Transform()
	{
		if (transformVertices)
			_TransformVertices();

		absolutePosition = position;
		absoluteRotation = rotation;

		XMMATRIX originMat = XMMatrixTranslation(-origin.x, -origin.y, 0);
		XMMATRIX scale = _GetScaleMatrix();
		XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(absoluteRotation);
		XMMATRIX loc = XMMatrixTranslationFromVector(absolutePosition);
		world = originMat * scale * rot * loc;

		if (parent != nullptr)
		{
			XMMATRIX parentRot = XMMatrixRotationRollPitchYawFromVector(parent->absoluteRotation);
			XMMATRIX parentLoc = XMMatrixTranslationFromVector(parent->absolutePosition);
			absolutePosition += parent->absolutePosition;
			absoluteRotation += parent->absoluteRotation;
			absolutePosition = XMVector2Transform(absolutePosition, parentRot);
			world = world * parentRot * parentLoc;
		}

		XMMATRIX worldViewProj = world * Core->GetCamera()->GetViewMatrix() * Core->GetCamera()->GetProjMatrix();
		//check for cursor
		if (pickable)
			_CheckForCursor(world);

		worldViewProj = XMMatrixTranspose(worldViewProj);
		Core->_GetContext()->UpdateSubresource(DrawManager->_GetConstantBufferVS(), 0, NULL, &worldViewProj, 0, 0);
	}

	void Dynamic::_Update()
	{
		// v += a
		velocity += acceleration * (float)Core->GetFrameTime();
		// x += v
		position += velocity * (float)Core->GetFrameTime();
		// rv += ra
		angularVelocity += angularAcceleration * (float)Core->GetFrameTime();
		// r = rv
		rotation += angularVelocity * (float)Core->GetFrameTime();
		// sv = sa
		sizeVelocity += sizeAcceleration * (float)Core->GetFrameTime();
	}

	void Dynamic::_TransformVertices()
	{
		for (int i = 0; i < vertexCount; i++)
			transformedVertices[i] = XMVector2Transform(vertices[i], world);
	}

	void Dynamic::SetParent(Dynamic* _parent)
	{		
		//add this to new parent's children if applicable
		if(parent != _parent && _parent != nullptr)
			_parent->children.push_back(this);

		//remove this from old parent's children if applicable
		else if (parent != _parent && parent != nullptr)
		{
			for (int i = 0; i < (int)parent->children.size(); i++)
			{
				if (parent->children[i] == this)
				{
					parent->children.erase(parent->children.begin() + i);
					break;
				}
			}
		}

		parent = _parent;
	}

	void Dynamic::Destroy()
	{
		//set all childrens parent to nullptr
		for (Dynamic* d : children)
			d->parent = nullptr;		
	}
}