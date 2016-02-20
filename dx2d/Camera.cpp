#include "Viva.h"

namespace Viva
{
	Camera::Camera()
	{
		nearPlane = 0.1f;
		farPlane = 1000.0f;
		fovAngle = 0.4f*3.14f; //72 deg
		//Camera information
		SetPosition(0, 0, -20);
		XMVECTOR target = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		//Set the View matrix
		view = DirectX::XMMatrixLookAtLH(position, target, up);

		//Set the Projection matrix
		aspectRatio = (float)Core->GetClientSize().width / Core->GetClientSize().height;
		proj = DirectX::XMMatrixPerspectiveFovLH(fovAngle, aspectRatio, nearPlane, farPlane);
	}

	void Camera::_CamTransform()
	{
		_Update();
		XMVECTOR target = DirectX::XMVectorAdd(position, DirectX::XMVectorSet(0, 0, 20, 1));
		XMMATRIX rot = DirectX::XMMatrixRotationRollPitchYawFromVector(rotation);
		view = DirectX::XMMatrixLookAtLH(position, target, up) * rot;
	}

	XMFLOAT3 Camera::GetCursorWorldPos(float z) const
	{
		POINT p = InputManager->GetCursorClientPos();
		//formula to convert z distance from camera to z in z-buffer
		//(20.0f + z) because by default camera is 20 from point (0,0,0)
		float zbuffer = ((farPlane + nearPlane) / (farPlane - nearPlane) + 
			((-2.0f * farPlane * nearPlane) /
			(farPlane - nearPlane)) / (20.0f + z) + 1.0f) / 2.0f;
		XMVECTOR pos3 = DirectX::XMVectorSet((float)p.x, (float)p.y, zbuffer, 1);
		RECT r1;
		GetClientRect(Core->GetWindowHandle(), &r1);
		XMVECTOR trans = XMVector3Unproject(pos3, 0.0f, 0.0f, (float)r1.right - r1.left,
			(float)r1.bottom - r1.top, 0.0f, 1.0f, proj,
			view, DirectX::XMMatrixIdentity());
		XMFLOAT3 f3t;
		XMStoreFloat3(&f3t, trans);
		return { f3t.x, f3t.y, z };
	}

	XMFLOAT2 Camera::GetFrustumSize(float z) const
	{
		XMFLOAT2 res;
		res.x = (20 + z) * tan(fovAngle/2) * aspectRatio * 2;
		res.y = (20 + z) * tan(fovAngle/2) * 2;
		return res;
	}

	XMFLOAT2 Camera::GetUnitsPerPixel(float z) const
	{
		XMFLOAT2 frustum = GetFrustumSize(z);
		RECT client;
		GetClientRect(Core->GetWindowHandle(), &client);
		XMFLOAT2 clientSize = { (float)client.right - client.left,
			(float)client.bottom - client.top };
		return XMFLOAT2( frustum.x / clientSize.x, frustum.y / clientSize.y );
	}

	void Camera::Destroy()
	{
		delete this;
	}
}