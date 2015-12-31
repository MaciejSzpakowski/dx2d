#pragma once

namespace Viva
{
	class Dynamic
	{
	protected:
		wstring name;
		int vertexCount;
		RenderTarget* renderTarget;
		vector<XMVECTOR> vertices;
		vector<XMVECTOR> transformedVertices;
		int index;
		ID3D11Buffer* vertexBuffer;
		void* extraBufferPSdata;
		XMMATRIX world;
		XMVECTOR position;
		XMVECTOR rotation;
		XMVECTOR velocity;
		XMVECTOR acceleration;
		XMVECTOR angularVelocity;
		XMVECTOR angularAcceleration;
		XMVECTOR absolutePosition;
		XMVECTOR absoluteRotation;
		Dynamic* parent;
		vector<Dynamic*> children;
		bool underCursor;
		bool pickable;
		float size;
		float sizeVelocity;
		float sizeAcceleration;
		XMFLOAT2 origin;
		bool visible;
		Color color;
		Rect uv;
		bool transformVertices; // normally vertices are not transformed until VS
		                        // you want them transformed for example for collisions

	public:
		Dynamic();
		
		virtual void Destroy();
		
		// Updates PVAJ etc.
		void _Update();
		void _Transform();
		virtual XMMATRIX _GetScaleMatrix() = 0;
		virtual void _CheckForCursor(XMMATRIX transform) {}
		virtual void _TransformVertices();
		virtual void _Draw() = 0;
		ID3D11Buffer* _GetVertexBuffer() const { return vertexBuffer; }
		int _GetIndex() const { return index; }
		void _SetIndex(int i) { index = i; }
		void _SetRenderTarget(RenderTarget* target) { renderTarget = target; }
		const XMVECTOR& _GetPositionVector() { return position; }
		const XMVECTOR& _GetRotationVector() { return rotation; }

		int GetVertexCount() const { return vertexCount; }
		const vector<XMVECTOR>& GetVertices() const { return vertices; }
		const vector<XMVECTOR>& GetVerticesTransformed() const { return transformedVertices; }
		const vector<Dynamic*>& GetChildren() const { return children; }
		XMMATRIX GetWorldMatrix() const { return world; }
		bool IsUnderCursor() { return underCursor; }
		// Send data to pixel shader.
		void SetExtraBufferPS(void* data) { extraBufferPSdata = data; }
		/////////////////////////////void* GetExtraBufferPS() const { return extraBufferPSdata; }
		Dynamic* GetParent() const { return parent; }
		void SetParent(Dynamic* _parent);
		RenderTarget* GetRenderTarget() { return renderTarget; }
		void SetColor(const Color& c) { color = c; }
		void SetColor(Color&& c) { color = c; }
		void SetColorR(float r) { color.r = r; }
		void SetColorG(float g) { color.g = g; }
		void SetColorB(float b) { color.b = b; }
		void SetColorA(float a) { color.a = a; }
		Color GetColor() const { return color; }
		wstring GetName() const { return name; }
		void SetName(const wstring& _name) { name = _name; }
		// Is pickable by cursor.
		bool IsPickable() const { return pickable; }
		// Set pickable by cursor.
		void SetPickable(bool flag) { pickable = flag; }
		bool IsVisible() const { return visible; }
		void SetVisible(bool v) { visible = v; }
		Rect GetUv() const { return uv; }
		void SetUv(const Rect& _uv) { uv = _uv; }
		void SetUv(Rect&& _uv) { uv = _uv; }
		// Enable or disable vertices transformation.
		void TransformVertices(bool flag) { transformVertices = flag; }

		// dynamics
		XMFLOAT2 GetOrigin() const { return origin; }
		void SetOrigin(const XMFLOAT2& o) { origin = o; }
		void SetOrigin(XMFLOAT2&& o) { origin = o; }
		float GetSize() const { return size; }
		void SetSize(float s) { size = s; }
		float GetSizeAcceleration() const { return sizeAcceleration; }
		void SetSizeAcceleration(float a) { sizeAcceleration = a; }
		float GetSizeVelocity() const { return sizeVelocity; }
		void SetSizeVelocity(float v) { sizeVelocity = v; }
		void SetPosition(const XMFLOAT3& v) { position = XMLoadFloat3(&v); }
		void SetPosition(float x, float y, float z) { position = DirectX::XMVectorSet(x, y, z, 1); }
		void SetPositionX(float x) { position = DirectX::XMVectorSetX(position, x); }
		void SetPositionY(float y) { position = DirectX::XMVectorSetY(position, y); }
		void SetPositionZ(float z) { position = DirectX::XMVectorSetZ(position, z); }
		XMFLOAT3 GetPosition() { XMFLOAT3 v; XMStoreFloat3(&v, position); return v; }
		void SetVelocity(const XMFLOAT3& v) { velocity = XMLoadFloat3(&v); }
		void SetVelocity(float x, float y, float z) { velocity = DirectX::XMVectorSet(x, y, z, 1); }
		void SetVelocityX(float x) { velocity = DirectX::XMVectorSetX(velocity, x); }
		void SetVelocityY(float y) { velocity = DirectX::XMVectorSetY(velocity, y); }
		void SetVelocityZ(float z) { velocity = DirectX::XMVectorSetZ(velocity, z); }
		XMFLOAT3 GetVelocity() { XMFLOAT3 v; XMStoreFloat3(&v, velocity); return v; }
		void SetAcceleration(const XMFLOAT3& v) { acceleration = XMLoadFloat3(&v); }
		void SetAcceleration(float x, float y, float z) { acceleration = DirectX::XMVectorSet(x, y, z, 1); }
		void SetAccelerationX(float x) { acceleration = DirectX::XMVectorSetX(acceleration, x); }
		void SetAccelerationY(float y) { acceleration = DirectX::XMVectorSetY(acceleration, y); }
		void SetAccelerationZ(float z) { acceleration = DirectX::XMVectorSetZ(acceleration, z); }
		XMFLOAT3 GetAcceleration() { XMFLOAT3 v; XMStoreFloat3(&v, acceleration); return v; }
		void SetRotation(const XMFLOAT3& v) { rotation = XMLoadFloat3(&v); }
		void SetRotation(float x, float y, float z) { rotation = DirectX::XMVectorSet(x, y, z, 1); }
		void SetRotationX(float x) { rotation = DirectX::XMVectorSetX(rotation, x); }
		void SetRotationY(float y) { rotation = DirectX::XMVectorSetY(rotation, y); }
		void SetRotationZ(float z) { rotation = DirectX::XMVectorSetZ(rotation, z); }
		XMFLOAT3 GetRotation() { XMFLOAT3 v; XMStoreFloat3(&v, rotation); return v; }
		void SetAngularVelocity(const XMFLOAT3& v) { angularVelocity = XMLoadFloat3(&v); }
		void SetAngularVelocity(float x, float y, float z) { angularVelocity = DirectX::XMVectorSet(x, y, z, 1); }
		void SetAngularVelocityX(float x) { angularVelocity = DirectX::XMVectorSetX(angularVelocity, x); }
		void SetAngularVelocityY(float y) { angularVelocity = DirectX::XMVectorSetY(angularVelocity, y); }
		void SetAngularVelocityZ(float z) { angularVelocity = DirectX::XMVectorSetZ(angularVelocity, z); }
		XMFLOAT3 GetAngularVelocity() { XMFLOAT3 v; XMStoreFloat3(&v, angularVelocity); return v; }
		void SetAngularAcceleration(const XMFLOAT3& v) { angularAcceleration = XMLoadFloat3(&v); }
		void SetAngularAcceleration(float x, float y, float z) { angularAcceleration = DirectX::XMVectorSet(x, y, z, 1); }
		void SetAngularAccelerationX(float x) { angularAcceleration = DirectX::XMVectorSetX(angularAcceleration, x); }
		void SetAngularAccelerationY(float y) { angularAcceleration = DirectX::XMVectorSetY(angularAcceleration, y); }
		void SetAngularAccelerationZ(float z) { angularAcceleration = DirectX::XMVectorSetZ(angularAcceleration, z); }
		XMFLOAT3 GetAngularAcc() { XMFLOAT3 v; XMStoreFloat3(&v, angularAcceleration); return v; }
		//It's different from position if object has a parent
		XMFLOAT3 GetAbsolutePosition() { XMFLOAT3 v; XMStoreFloat3(&v, absolutePosition); return v; }
	};
}
