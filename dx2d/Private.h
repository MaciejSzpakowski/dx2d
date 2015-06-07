#pragma once
#include <Windows.h>
#include <functional>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <cmath>
#include <vector>
#include <string>
#include "Keys.h"
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
	class CInputManager;
	class Sprite;
	class Text;
	void Render(Core* d3d);
	Core* NewCore(int sizex, int sizey, std::function<void()> worker);
	void AddFloat3(XMFLOAT3* src, XMFLOAT3* dst);

	//externals
	extern Core* Global;
	extern CKey Key;

	//defines
#define Device Global->GetDevice()
#define Context Global->GetContext()
#define DrawManager Global->GetDrawManager()
#define Camera Global->GetCamera()
#define InputManager Global->GetInputManager()
	
	struct VERTEX
	{
		FLOAT X, Y, Z;
		FLOAT R, G, B;
		FLOAT U, V;
	};

	struct SColor
	{
		float r, g, b, a;
		int x1, x2, x3, x4;
		SColor(){}
		SColor(float _r,float _g,float _b,float _a);
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
		ID3D11InputLayout* layout; //vertex input layout pos:float[3] uv:float[2]
		ID3D11DepthStencilView* depthStencilView;
		ID3D11Texture2D* depthStencilBuffer;		
		CDrawManager* drawManager;
		CCamera* camera;
		CInputManager* inputManager;
		float backBufferColor[4];
		friend void Render(Core* d3d);
	public:
		ID3D11BlendState* blendState;
		Core(int sizex, int sizey, std::function<void()> worker);
		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetContext();
		CDrawManager* GetDrawManager();
		HWND GetWindowHandle();
		CCamera* GetCamera();
		CInputManager* GetInputManager();
		void SetWindowTitle(const char* title);
		void SetBackBufferColor(float color[4]);
		int Run();
		ID3D11Texture2D* CreateTexture2D(const WCHAR* file);
		ID3D11Texture2D* CreateTexture2DFromText(std::wstring text);
		void Destroy();
	};	

	class Dynamic
	{
	protected:
		ID3D11Buffer* cbBufferVS;
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
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* cbBufferPS;
		friend class CDrawManager;
	public:
		Drawable();
		bool Visible;
		SColor Color;
		virtual void Draw() = 0;
		~Drawable();
	};

	class Polygon : public Drawable,public Dynamic
	{
	protected:		
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
		XMFLOAT2 Scale;
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
		vector<Sprite*> Sprites;
		ID3D11RasterizerState* wireframe;
		ID3D11RasterizerState* solid;
		friend void Render(Core* d3d);
	public:
		CDrawManager();
		Polygon* AddPoly(XMFLOAT2 points[], int n);
		Rectangle* AddRect(float sizex, float sizey);
		Circle* AddCircle(float radius, unsigned char resolution);
		void AddPoly(Polygon* p);
		void DrawAll();
		void Destroy();
		void RemovePoly(Polygon* p);		
		//add sprite from file
		//supported file formats: BMP, GIF, JPEG, PNG, TIFF, Exif, WMF, EMF
		Sprite* AddSprite(const WCHAR* texture);
		void AddSprite(Sprite* s);
		Text* AddText(std::wstring text);
		void AddText(Text* t);
		void RemoveSprite(Sprite* s);
		void RemoveText(Text* t);
	};	

	class CCamera : public Dynamic
	{
	private:		
		XMMATRIX view;
		XMMATRIX proj;
		XMVECTOR up;		
		friend void Render(Core* d3d);
		friend class Dynamic;
		XMMATRIX GetScaleMatrix() override;
		void CamTransform();
	public:
		CCamera();
		void Destroy();
	};

	class CInputManager
	{
	private:
		int keyCount;
		bool* curState;
		bool* prevState;
		POINT* curMouse;
		POINT* prevMouse;
	public:
		CInputManager();
		bool IsKeyDown(int vkey);
		bool IsKeyPressed(int vkey);
		bool IsKeyReleased(int vkey);
		bool IsAnyKeyDown();
		void GetCursorDelta(POINT* pos);
		char GetChar();
		//offset, where to start looking
		char GetKey(int offset);
		void ResetKey(int vkey);
		void Activity();
		void Destroy();
	};

	class Sprite : public Drawable, public Dynamic
	{
	protected:
		ID3D11Buffer* indexBuffer;
		ID3D11ShaderResourceView* shaderResource;
		ID3D11SamplerState* samplerState;
		XMMATRIX GetScaleMatrix() override;
	public:
		XMFLOAT2 Scale;
		Sprite(){}
		Sprite(const WCHAR* texture);
		void Draw() override;
		void Destroy();
	};

	class Text : public Sprite
	{
	protected:
	public:
		Text(std::wstring text);
	};
}