#include "Private.h"

namespace dx2d
{
	void AddFloat3(XMFLOAT3* src, XMFLOAT3* dst);

	CCamera::CCamera()
	{
		//Camera information
		Position = XMFLOAT3(0, 0, -20);
		XMVECTOR position = DirectX::XMLoadFloat3(&Position);
		XMVECTOR target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		//Set the View matrix
		view = DirectX::XMMatrixLookAtLH(position, target, up);

		//Set the Projection matrix
		RECT rect;
		GetWindowRect(Core->GetWindowHandle(), &rect);
		float aspectRatio = (float)(rect.right-rect.left)/(rect.bottom-rect.top);
		proj = DirectX::XMMatrixPerspectiveFovLH(0.4f*3.14f, aspectRatio, 0.1f, 1000.0f);
	}

	XMMATRIX CCamera::GetScaleMatrix()
	{
		return DirectX::XMMatrixIdentity();
	}

	void CCamera::CamTransform()
	{
		AddFloat3(&Acceleration, &Velocity);
		AddFloat3(&Velocity, &Position);
		AddFloat3(&Spin, &Rotation);
		XMVECTOR position = DirectX::XMLoadFloat3(&Position);
		XMVECTOR target = XMVectorSet(Position.x, Position.y, Position.z + 20, 0);
		XMMATRIX rotz = DirectX::XMMatrixRotationZ(Rotation.z);
		view = DirectX::XMMatrixLookAtLH(position, target, up) * rotz;
	}

	void CCamera::Destroy()
	{
	}
}