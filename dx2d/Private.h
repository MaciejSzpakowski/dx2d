#pragma once
#include <Windows.h>
#include <functional>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <cmath>
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
	using DirectX::XM_PI;
	using DirectX::XM_2PI;
	using std::vector;
	
	//prototypes
	class Core;
	class Object;
	class Drawable;
	class Polygon;
	class Rectangle;
	class Circle;
	class CDrawManager;
	class CCamera;
	void Render(Core* d3d);
	Core* NewCore(int sizex, int sizey, std::function<void()> worker);
	void AddFloat3(XMFLOAT3* src, XMFLOAT3* dst);

	//externals
	extern Core* Global;

	//defines
#define Device Global->GetDevice()
#define Context Global->GetContext()
#define DrawManager Global->GetDrawManager()
#define Camera Global->GetCamera()
	
	struct VERTEX
	{
		FLOAT X, Y, Z;
		XMFLOAT4 Color;
	};

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
		Core(int sizex, int sizey, std::function<void()> worker);
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

	class Dynamic
	{
	protected:
		ID3D11Buffer* cbPerObjectBuffer16;
		void Transform();
		virtual XMMATRIX GetScaleMatrix() = 0;
		friend class CDrawManager;
	public:
		XMFLOAT3 Position;
		XMFLOAT3 Rotation;
		XMFLOAT3 Velocity;
		XMFLOAT3 Acceleration;
		XMFLOAT3 Spin;
		Dynamic();
		~Dynamic();
	};

	class Drawable
	{
	protected:
		int vertexCount;
		int index;
		ID3D11Buffer* cbPerObjectBuffer4;
		friend class CDrawManager;
	public:
		Drawable();
		bool Visible;
		XMFLOAT4 Color;
		virtual void Draw() = 0;
		~Drawable();
	};

	class Polygon : public Drawable,public Dynamic
	{
	protected:
		ID3D11Buffer* vertexBuffer;
		XMMATRIX GetScaleMatrix() override;
	public:
		Polygon();
		Polygon(XMFLOAT2 points[], int n);		
		void Draw() override;
		void Destroy();
	};

	class Rectangle : public Polygon
	{
	protected:
		XMMATRIX GetScaleMatrix() override;
	public:
		float ScaleX;
		float ScaleY;
		Rectangle(float scalex, float scaley);		
	};

	class Circle : public Polygon
	{
	protected:
		XMMATRIX GetScaleMatrix() override;
	public:
		float Radius;
		Circle(float radius, unsigned char resolution);		
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
		Circle* AddCircle(float radius, unsigned char resolution);
		void DrawAll();
		void Destroy();
		void Remove(Polygon* p);
		void Add(Polygon* p);
	};	

	class CCamera : public Dynamic
	{
	private:		
		XMMATRIX view;
		XMMATRIX proj;
		XMVECTOR target;
		XMVECTOR up;		
		friend void Render(Core* d3d);
		friend class Dynamic;
		XMMATRIX GetScaleMatrix() override;
	public:
		CCamera();
		void Destroy();
	};
}