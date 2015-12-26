#pragma once

namespace Viva
{
	class CCamera : public CDynamic
	{
	public:
		XMMATRIX zView;
		XMMATRIX zProj;
		XMVECTOR zUp;
		XMMATRIX zGetScaleMatrix() override;
		void zCamTransform();
		void zDraw() override {};
		float zNearPlane;
		float zFarPlane;
		float zFovAngle;
		float zAspectRatio;

		CCamera();
		XMFLOAT3 GetCursorWorldPos(float z);
		XMFLOAT2 GetFrustumSize(float z);
		XMMATRIX GetViewMatrix();
		XMMATRIX GetProjMatrix();
		void Destroy();
	};
}
