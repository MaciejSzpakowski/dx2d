#pragma once
#include <Windows.h>
#include <functional>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3DCompiler.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")

namespace dx2d
{
	//using	
	using DirectX::XMFLOAT4;
	using DirectX::XMFLOAT3;
	using DirectX::XMFLOAT2;
	using DirectX::XMVECTOR;
	using DirectX::XMMATRIX;
	using DirectX::XMVectorSet;
	using std::vector;

	//prototypes
	class Core;
	class Object;
	class Drawable;
	class Polygon;
	class CDrawManager;
	class CCamera;
	void Render(Core* d3d);	

	//externals
	extern Core* Global;

	//defines
#define Device dx2d::Global->GetDevice()
#define Context dx2d::Global->GetContext()
#define DrawManager dx2d::Global->GetDrawManager()
#define Camera dx2d::Global->GetCamera()

	class Window
	{
	private:
		HWND handle;
		MSG Msg;
		std::function<void()> Worker;
		std::function<void(Core* d3d)> Render;
		friend class Core;
	public:
		Window(int sizex, int sizey);
		int Run();
	};

	class Core
	{
	private:
		Window* window;
		IDXGISwapChain* swapChain;
		ID3D11RenderTargetView* backBuffer;
		ID3D11Device* device;
		ID3D11DeviceContext* context;
		ID3D11VertexShader* defaultVS;
		ID3D11PixelShader* defaultPS;
		ID3D11InputLayout* layoutPosCol; //vertex input layout pos:float[3] col:float[4]
		CDrawManager* drawManager;
		CCamera* camera;
		float backBufferColor[4];
		friend void Render(Core* d3d);
	public:
		Core(int sizex, int sizey);
		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetContext();
		CDrawManager* GetDrawManager();
		HWND GetWindowHandle();
		CCamera* GetCamera();
		void SetWindowTitle(const char* title);
		void SetBackBufferColor(float color[4]);
		int Run();
		void Destroy();
	};

	struct VERTEX
	{
		FLOAT X, Y, Z;
		XMFLOAT4 Color;
	};

	class Dynamic
	{
	protected:
		XMVECTOR position;
		XMFLOAT3 rotation;
		XMFLOAT3 velocity;
		XMFLOAT3 acceleration;
		XMFLOAT3 spin;
	public:
		XMFLOAT3 GetPosition();
		void SetPosition(XMFLOAT3 _position);
	};

	class Drawable
	{
	protected:
		int vertexCount;
		int index;
		friend class CDrawManager;
	public:
		bool Visible;
		XMFLOAT4 Color;
		virtual void Draw() = 0;
	};

	class Polygon : public Drawable, Dynamic
	{
	protected:
		ID3D11Buffer* vertexBuffer;
	public:
		Polygon();
		Polygon(XMFLOAT2 points[], int n);
		void Draw() override;
		void Destroy();
	};

	class Rectangle : public Polygon
	{
	public:
		float ScaleX;
		float ScaleY;
		Rectangle(float scalex, float scaley);
	};

	class CDrawManager
	{
	private:
		vector<Polygon*> Polygons;
		ID3D11RasterizerState* wireframe;
		ID3D11RasterizerState* solid;
		friend void Render(Core* d3d);
	public:
		CDrawManager();
		Polygon* AddPoly(XMFLOAT2 points[], int n);
		Rectangle* AddRect(float sizex, float sizey);
		void DrawAll();
		void Destroy();
		void Remove(Polygon* p);
	};

	struct cbPerObject
	{
		XMMATRIX  WorldViewProj;
	};

	class CCamera : public Dynamic
	{
	private:
		ID3D11Buffer* cbPerObjectBuffer;
		XMMATRIX worldViewProj;
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
		XMVECTOR target;
		XMVECTOR up;
		cbPerObject cbPerObj;
		friend void Render(Core* d3d);
	public:
		CCamera();
		void Destroy();
	};
}