#pragma once

namespace Viva
{
	class CDynamic
	{
	public:
		CRenderTarget* zRenderTarget;
		int zVertexCount;
		vector<XMVECTOR> zVertices;
		vector<XMVECTOR> zTransformedVertices;
		int zIndex;
		ID3D11Buffer* zVertexBuffer;
		void* zExtraBufferPSdata;
		XMMATRIX zWorld;
		XMVECTOR zPosition;
		XMVECTOR zRotation;
		XMVECTOR zVelocity;
		XMVECTOR zAcceleration;
		XMVECTOR zAngularVel;
		XMVECTOR zAngularAcc;
		XMVECTOR zAbsolutePosition;
		XMVECTOR zAbsoluteRotation;
		CDynamic* zParent;
		vector<CDynamic*> zChildren;
		//matrix algebra for
		//produces worldViewProj used by VS
		void zTransform();
		//updates PVAJ etc.
		void zUpdate();
		virtual XMMATRIX zGetScaleMatrix() = 0;
		virtual void zCheckForCursor(XMMATRIX transform) {}
		bool zUnderCursor;
		virtual void zTransformVertices();
		virtual void zDraw() = 0;

		CDynamic* GetParent();
		void SetParent(CDynamic* parent);
		vector<CDynamic*> GetChildren();
		XMMATRIX GetWorldMatrix();
		bool IsUnderCursor();
		//used to send custom data to PS		
		void SetExtraBufferPS(void* data);
		CRenderTarget* GetRenderTarget() { return zRenderTarget; }
		void SetRenderTarget(CRenderTarget* target);
		CDynamic();
		~CDynamic();

		bool Pickable;
		float SizeVelocity;
		float SizeAcceleration;
		float size;
		XMFLOAT2 Origin;
		bool Visible;
		Color color;
		Rect UV;
		bool TransformVertices;

		void SetColor(float r, float g, float b, float a);

		//repetitive code
		void SetPosition(XMFLOAT3 v) { zPosition = XMLoadFloat3(&v); }
		void SetPosition(float x, float y, float z) { zPosition = XMVectorSet(x, y, z, 1); }
		void SetPositionX(float x) { zPosition = XMVectorSetX(zPosition, x); }
		void SetPositionY(float y) { zPosition = XMVectorSetY(zPosition, y); }
		void SetPositionZ(float z) { zPosition = XMVectorSetZ(zPosition, z); }
		XMFLOAT3 GetPosition() { XMFLOAT3 v; XMStoreFloat3(&v, zPosition); return v; }
		XMVECTOR GetPositionVector() { return zPosition; }
		void SetVelocity(XMFLOAT3 v) { zVelocity = XMLoadFloat3(&v); }
		void SetVelocity(float x, float y, float z) { zVelocity = XMVectorSet(x, y, z, 1); }
		void SetVelocityX(float x) { zVelocity = XMVectorSetX(zVelocity, x); }
		void SetVelocityY(float y) { zVelocity = XMVectorSetY(zVelocity, y); }
		void SetVelocityZ(float z) { zVelocity = XMVectorSetZ(zVelocity, z); }
		XMFLOAT3 GetVelocity() { XMFLOAT3 v; XMStoreFloat3(&v, zVelocity); return v; }
		void SetAcceleration(XMFLOAT3 v) { zAcceleration = XMLoadFloat3(&v); }
		void SetAcceleration(float x, float y, float z) { zAcceleration = XMVectorSet(x, y, z, 1); }
		void SetAccelerationX(float x) { zAcceleration = XMVectorSetX(zAcceleration, x); }
		void SetAccelerationY(float y) { zAcceleration = XMVectorSetY(zAcceleration, y); }
		void SetAccelerationZ(float z) { zAcceleration = XMVectorSetZ(zAcceleration, z); }
		XMFLOAT3 GetAcceleration() { XMFLOAT3 v; XMStoreFloat3(&v, zAcceleration); return v; }
		void SetRotation(XMFLOAT3 v) { zRotation = XMLoadFloat3(&v); }
		void SetRotation(float x, float y, float z) { zRotation = XMVectorSet(x, y, z, 1); }
		void SetRotationX(float x) { zRotation = XMVectorSetX(zRotation, x); }
		void SetRotationY(float y) { zRotation = XMVectorSetY(zRotation, y); }
		void SetRotationZ(float z) { zRotation = XMVectorSetZ(zRotation, z); }
		XMFLOAT3 GetRotation() { XMFLOAT3 v; XMStoreFloat3(&v, zRotation); return v; }
		void SetAngularVel(XMFLOAT3 v) { zAngularVel = XMLoadFloat3(&v); }
		void SetAngularVel(float x, float y, float z) { zAngularVel = XMVectorSet(x, y, z, 1); }
		void SetAngularVelX(float x) { zAngularVel = XMVectorSetX(zAngularVel, x); }
		void SetAngularVelY(float y) { zAngularVel = XMVectorSetY(zAngularVel, y); }
		void SetAngularVelZ(float z) { zAngularVel = XMVectorSetZ(zAngularVel, z); }
		XMFLOAT3 GetAngularVel() { XMFLOAT3 v; XMStoreFloat3(&v, zAngularVel); return v; }
		void SetAngularAcc(XMFLOAT3 v) { zAngularAcc = XMLoadFloat3(&v); }
		void SetAngularAcc(float x, float y, float z) { zAngularAcc = XMVectorSet(x, y, z, 1); }
		void SetAngularAccX(float x) { zAngularAcc = XMVectorSetX(zAngularAcc, x); }
		void SetAngularAccY(float y) { zAngularAcc = XMVectorSetY(zAngularAcc, y); }
		void SetAngularAccZ(float z) { zAngularAcc = XMVectorSetZ(zAngularAcc, z); }
		XMFLOAT3 GetAngularAcc() { XMFLOAT3 v; XMStoreFloat3(&v, zAngularAcc); return v; }
		//It's different from position if object has a parent
		XMFLOAT3 GetAbsolutePosition() { XMFLOAT3 v; XMStoreFloat3(&v, zAbsolutePosition); return v; }
	};
}
