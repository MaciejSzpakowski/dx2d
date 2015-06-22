#include "Private.h"

namespace dx2d
{
	void AddFloat3(XMFLOAT3* src, XMFLOAT3* dst);

	CCamera::CCamera()
	{
		nearPlane = 0.1f;
		farPlane = 1000.0f;
		fovAngle = 0.4f*3.14f; //72 deg
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
		aspectRatio = (float)(rect.right - rect.left) / (rect.bottom - rect.top);
		proj = DirectX::XMMatrixPerspectiveFovLH(fovAngle, aspectRatio, nearPlane, farPlane);
	}

	XMMATRIX CCamera::GetScaleMatrix()
	{
		return DirectX::XMMatrixIdentity();
	}

	void CCamera::CamTransform()
	{
		Update();
		XMVECTOR position = DirectX::XMLoadFloat3(&Position);
		XMVECTOR target = XMVectorSet(Position.x, Position.y, Position.z + 20, 0);
		XMMATRIX rotz = DirectX::XMMatrixRotationZ(Rotation.z);
		view = DirectX::XMMatrixLookAtLH(position, target, up) * rotz;
	}

	XMMATRIX CCamera::GetViewMatrix()
	{
		return view;
	}

	XMMATRIX CCamera::GetProjMatrix()
	{
		return proj;
	}

	POINTF CCamera::GetCursorWorldPos(float z)
	{
		POINT p = Core->GetCursorPos();
		//formula to convert z distance from camera to z in z-buffer
		//(20.0f + z) because by default camera is 20 from point (0,0,0)
		float zbuffer = ((farPlane + nearPlane) / (farPlane - nearPlane) + ((-2.0f * farPlane * nearPlane) /
			(farPlane - nearPlane)) / (20.0f + z) + 1.0f) / 2.0f;
		XMVECTOR pos3 = DirectX::XMVectorSet((float)p.x, (float)p.y, zbuffer, 1);
		RECT r1;
		GetClientRect(Core->GetWindowHandle(), &r1);
		XMVECTOR trans = DirectX::XMVector3Unproject(pos3, 0.0f, 0.0f, (float)r1.right - r1.left,
			(float)r1.bottom - r1.top, 0.0f, 1.0f, proj,
			view, DirectX::XMMatrixIdentity());
		XMFLOAT3 f3t;
		DirectX::XMStoreFloat3(&f3t, trans);
		return { f3t.x, f3t.y };
	}

	POINTF CCamera::GetFrustumSize(float z)
	{
		POINTF res;
		res.x = (20 + z) * tan(fovAngle/2) * aspectRatio * 2;
		res.y = (20 + z) * tan(fovAngle/2) * 2;
		return res;
	}

	void CCamera::Destroy()
	{
		delete this;
	}
}