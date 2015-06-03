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

using DirectX::XMFLOAT4;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT2;

namespace dx2d
{
	//using	
	using std::vector;

	//prototypes
	class Core;
	class Object;
	class Drawable;
	class Polygon;
	class CDrawManager;
	void Render(Core* d3d);	

	//externals
	extern Core* Global;

	//defines
#define Device dx2d::Global->GetDevice()
#define Context dx2d::Global->GetContext()
#define DrawManager dx2d::Global->GetDrawManager()

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
		HWND GetHandle()
		{
			return handle;
		}
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
		float backBufferColor[4];
		friend void Render(Core* d3d);
	public:
		Core(int sizex, int sizey);
		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetContext();
		CDrawManager* GetDrawManager();
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

	class Object
	{
	private:
	public:
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

	class Polygon : public Drawable
	{
	protected:
		ID3D11Buffer* vertexBuffer;
	public:
		Polygon(XMFLOAT2 points[], int n);
		void Draw() override;
		void Destroy();
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
		void DrawAll();
		void Destroy();
		void Remove(Polygon* p);
	};
}