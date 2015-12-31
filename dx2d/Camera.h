#pragma once

namespace Viva
{
	class Camera : public Dynamic
	{
	private:
		XMMATRIX view;
		XMMATRIX proj;
		XMVECTOR up;
		float nearPlane;
		float farPlane;
		float fovAngle;
		float aspectRatio;

		XMMATRIX _GetScaleMatrix() override { return DirectX::XMMatrixIdentity(); };
		void _Draw() override {};
	public:
		Camera();

		void _CamTransform();

		// Get cursor position in world cooridnates at speicifc z.
		// Notice that it depends on z value (think about it).
		XMFLOAT3 GetCursorWorldPos(float z) const;

		// Get the size of the camera's frustum at specific z.
		// In other words, client dimensions in world units.
		XMFLOAT2 GetFrustumSize(float z) const;

		// Calculate pixel to world units at specific z.
		// How far it is in world units to move cursor by one pixel.
		XMFLOAT2 GetUnitsPerPixel(float z) const;

		const XMMATRIX& GetViewMatrix() const { return view; }
		const XMMATRIX& GetProjMatrix() const { return proj; }
		float GetNearPlane() const { return nearPlane; }
		void SetNearPlane(float _nearPlane) { nearPlane = _nearPlane; }
		float GetFarPlane() const { return farPlane; }
		void SetFarPlane(float _farPlane) { farPlane = _farPlane; }
		float GetFovAngle() const { return fovAngle; }
		void SetFovAngle(float _fovAngle) { fovAngle = _fovAngle; }
		float GetAspectRatio() const { return aspectRatio; }
		void SetAspectRatio(float _aspectRatio) { aspectRatio = _aspectRatio; }

		void Destroy();
	};
}
