#include "Private.h"

namespace dx2d
{
	void AddFloat3(XMFLOAT3* src, XMFLOAT3* dst);

	CCamera::CCamera()
	{
		zNearPlane = 0.1f;
		zFarPlane = 1000.0f;
		zFovAngle = 0.4f*3.14f; //72 deg
		//Camera information
		SetPosition(0, 0, -20);
		XMVECTOR target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		zUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		//Set the View matrix
		zView = XMMatrixLookAtLH(zPosition, target, zUp);

		//Set the Projection matrix
		zAspectRatio = (float)Core->zClientSize.x / Core->zClientSize.y;
		zProj = XMMatrixPerspectiveFovLH(zFovAngle, zAspectRatio, zNearPlane, zFarPlane);
		DrawManager->zRenderTargetMatrix = zView * zProj;
	}

	XMMATRIX CCamera::zGetScaleMatrix()
	{
		return XMMatrixIdentity();
	}

	void CCamera::zCamTransform()
	{
		zUpdate();
		XMVECTOR target = XMVectorAdd(zPosition, XMVectorSet(0, 0, 20, 1));
		XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(zRotation);
		zView = XMMatrixLookAtLH(zPosition, target, zUp) * rot;
	}

	XMMATRIX CCamera::GetViewMatrix()
	{
		return zView;
	}

	XMMATRIX CCamera::GetProjMatrix()
	{
		return zProj;
	}

	XMFLOAT3 CCamera::GetCursorWorldPos(float z)
	{
		POINT p = Core->GetCursorPos();
		//formula to convert z distance from camera to z in z-buffer
		//(20.0f + z) because by default camera is 20 from point (0,0,0)
		float zbuffer = ((zFarPlane + zNearPlane) / (zFarPlane - zNearPlane) + 
			((-2.0f * zFarPlane * zNearPlane) /
			(zFarPlane - zNearPlane)) / (20.0f + z) + 1.0f) / 2.0f;
		XMVECTOR pos3 = XMVectorSet((float)p.x, (float)p.y, zbuffer, 1);
		RECT r1;
		GetClientRect(Core->GetWindowHandle(), &r1);
		XMVECTOR trans = XMVector3Unproject(pos3, 0.0f, 0.0f, (float)r1.right - r1.left,
			(float)r1.bottom - r1.top, 0.0f, 1.0f, zProj,
			zView, XMMatrixIdentity());
		XMFLOAT3 f3t;
		XMStoreFloat3(&f3t, trans);
		return { f3t.x, f3t.y, z };
	}

	XMFLOAT2 CCamera::GetFrustumSize(float z)
	{
		XMFLOAT2 res;
		res.x = (20 + z) * tan(zFovAngle/2) * zAspectRatio * 2;
		res.y = (20 + z) * tan(zFovAngle/2) * 2;
		return res;
	}

	void CCamera::Destroy()
	{
		delete this;
	}
}