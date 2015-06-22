#pragma once
#include <Windows.h>
#include <functional>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <gdiplus.h>
#include "Keys.h"
#pragma comment(lib,"Gdiplus.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3DCompiler.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")

namespace dx2d
{
#define CHECKHR() Functions::Checkhr(__FILE__,__LINE__)
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
	using std::wstring;

	//prototypes
	class CCore;
	class CDrawable;
	class CPolygon;
	class CRectangle;
	class CCircle;
	class CDrawManager;
	class CCamera;
	class CInput;
	class CSprite;
	class CEventManager;
	class CResourceManager;
	class CAnimation;
	class CBitmapFont;
	class CBitmapText;
	class CDebugManager;

	//externals
	extern CCore* Core;
	extern CKey Key;
	extern CDrawManager* DrawManager;
	extern CCamera* Camera;
	extern CInput* Input;
	extern CEventManager* EventManager;
	extern CResourceManager* ResourceManager;
	extern CDebugManager* DebugManager;
	extern HRESULT hr;

	struct Vertex
	{
		FLOAT X, Y, Z;
		FLOAT R, G, B;
		FLOAT U, V;
		Vertex(){}
		Vertex(FLOAT x, FLOAT y, FLOAT z, FLOAT r, FLOAT g, FLOAT b, FLOAT u, FLOAT v)
			: X(x), Y(y), Z(z), R(r), G(g), B(b), U(u), V(v){}
	};

	enum class TEX_FILTER { POINT, LINEAR };

	struct Color
	{
		float r, g, b, a;
		Color(){}
		Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a){}
		Color& operator = (const XMFLOAT4& other)
		{
			r = other.x;
			g = other.y;
			b = other.z;
			a = other.w;
			return *this;
		}
	};

	struct UV
	{
		float left;
		float top;
		float right;
		float bottom;
		UV(){}
		UV(float _left, float _top, float _right, float _bottom) :
			left(_left), top(_top), right(_right), bottom(_bottom){}
	};

	struct Event
	{
		double startTime;
		double delay;
		wstring Name;
		std::function<int()> Activity;
	};

	//used to cache texture loaded from file
	class CTexture
	{
	public:
		int Height;
		int Width;
		wstring name;
		ID3D11ShaderResourceView* shaderResource;
		//destructor exclusive for resource manager
		~CTexture()
		{
			shaderResource->Release();
		}
	};

	namespace Functions
	{
		CCore* NewCore(int sizex, int sizey, std::function<void()> worker, int style = WS_OVERLAPPEDWINDOW);
		ID3D11Texture2D* CreateTexture2DFromBytes(BYTE* data, int width, int height);
		ID3D11Texture2D* CreateTexture2DFromGdibitmap(Gdiplus::Bitmap* _gdibitmap);
		ID3D11Texture2D* CreateTexture2DFromFile(const WCHAR* file);
		ID3D11Texture2D* CreateTexture2DFromResource(int resource);
		CTexture* LoadCachedTextureFromFile(const WCHAR* file, ID3D11ShaderResourceView*& shader);
		CTexture* LoadCachedTextureFromResource(int resource, ID3D11ShaderResourceView*& shader);
		void Checkhr(const char* file, int line);
		//return double between 0 and 1
		double RndDouble();
		//return integer
		//between inclusive min and exclusive max
		int RndInt(int min, int max);
	}

	class CWindow
	{
	private:
		HWND handle;
		MSG Msg;
		std::function<void()> Worker;
		std::function<void(CCore* d3d)> Render;
		friend class CCore;
	public:
		CWindow(int sizex, int sizey, int style);
		int Run();
	};

	class CCore
	{
	private:
		CWindow* window;
		ID3D11BlendState* blendState;
		IDXGISwapChain* swapChain;
		ID3D11RenderTargetView* backBuffer;
		ID3D11Device* device;
		ID3D11DeviceContext* context;
		ID3D11VertexShader* defaultVS;
		ID3D11PixelShader* defaultPS;
		ID3D11InputLayout* layout; //vertex input layout pos:float[3] col:float[3] uv:float[2]
		ID3D11DepthStencilView* depthStencilView;
		ID3D11Texture2D* depthStencilBuffer;
		float backBufferColor[4];
		double frequency;
		long long startTime;
		long long prevFrameTime;
		double gameTime;
		double frameTime;
		void UpdateGameTime();
		bool fullscreen;
		POINT clientSize;

		friend void Render(CCore* d3d);
		friend ID3D11DeviceContext* GetContext();
		friend ID3D11Device* GetDevice();
		friend class CDrawManager;
		friend void AddFloat3(XMFLOAT3* src, XMFLOAT3* dst);
	public:		
		CCore(int sizex, int sizey, std::function<void()> worker, int style);
		HWND GetWindowHandle();
		void SetWindowTitle(const wchar_t* title);
		void SetBackgroundColor(Color color);
		int Run();
		void OpenConsole();
		void CloseConsole();
		double GetGameTime();
		//time per frame, to get FPS -> 1 / GetFrameTime()
		double GetFrameTime();
		double GetFps();
		void SetFullscreen(bool state);
		bool GetFullscreen();
		POINT GetCursorPos();
		void Destroy();
	};

	class CResourceManager
	{
	private:
		std::map<wstring, CTexture*> textures;
		std::map<wstring, CBitmapFont*> fonts;
	public:
		CResourceManager::CResourceManager();
		void AddTexture(CTexture* tex);
		CTexture* GetTexture(wstring name);
		void AddBitmapFont(CBitmapFont* font);
		CBitmapFont* GetBitmapFont(wstring name);
		void Clear();
		void Destroy();
	};

	class CDynamic
	{
	protected:
		ID3D11Buffer* cbBufferVS;
		//matrix algebra for
		//produces worldViewProj used by VS
		void Transform();
		//updates PVAJ etc.
		void Update();
		virtual XMMATRIX GetScaleMatrix() = 0;
		friend class CDrawManager;
	public:
		CDynamic* Parent;
		XMFLOAT3 Position;
		XMFLOAT3 Rotation;
		XMFLOAT3 Velocity;
		XMFLOAT3 Acceleration;
		XMFLOAT3 AngularVel;
		XMFLOAT3 AngularAcc;
		CDynamic();
		~CDynamic();
	};

	class CDrawable
	{
	protected:
		int vertexCount;
		int index;
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* cbBufferPS;
		ID3D11Buffer* cbBufferUV;
		virtual void Draw() = 0;

		friend class CDrawManager;
	public:
		CDrawable();
		bool Visible;
		Color Color;
		UV uv;		
		~CDrawable();
	};

	class CPolygon : public CDrawable, public CDynamic
	{
	protected:
		XMMATRIX GetScaleMatrix() override;
		void Draw() override;

		friend class CDrawManager;
	public:
		CPolygon();
		CPolygon(XMFLOAT2 points[], int n);		
		void Destroy();
	};

	class CRectangle : public CPolygon
	{
	protected:
		XMMATRIX GetScaleMatrix() override;
	public:
		XMFLOAT2 Scale;
		CRectangle(float scalex, float scaley);
	};

	class CCircle : public CPolygon
	{
	protected:
		XMMATRIX GetScaleMatrix() override;
	public:
		float Radius;
		CCircle(float radius, unsigned char resolution);
	};

	class CDrawManager
	{
	private:
		CBitmapFont* defaultFont;
		ID3D11ShaderResourceView* whiteRes;
		ID3D11SamplerState* pointSampler;
		ID3D11SamplerState* lineSampler;
		ID3D11Buffer* indexBufferSprite;
		ID3D11Buffer* vertexBufferSprite;
		vector<CPolygon*> Polygons;
		vector<CSprite*> Sprites;
		vector<CBitmapText*> Texts;
		ID3D11RasterizerState* wireframe;
		ID3D11RasterizerState* solid;
		void DrawAll();

		friend void Render(CCore* d3d);
		friend class CSprite;
		friend class CAnimation;
		friend class CCore;
	public:
		CDrawManager();
		CPolygon* AddPoly(XMFLOAT2 points[], int n);
		CRectangle* AddRect(float sizex, float sizey);
		CCircle* AddCircle(float radius, unsigned char resolution);
		void AddPoly(CPolygon* p);		
		void Destroy();
		void RemovePoly(CPolygon* p);
		//add sprite from file
		//supported file formats: BMP, GIF, JPEG, PNG, TIFF, Exif, WMF, EMF
		CSprite* AddSprite(const WCHAR* texture);
		void AddSprite(CSprite* s);
		void RemoveSprite(CSprite* s);
		CAnimation* AddAnimation(const WCHAR* texture, int x, int y);
		void AddAnimation(CAnimation* a);
		void RemoveAnimation(CAnimation* a);
		TEX_FILTER TexFilterCreationMode;
		CBitmapFont* AddBitmapFont(const WCHAR* file, vector<UV> chars);
		void AddBitmapFont(CBitmapFont* font);
		CBitmapText* AddBitmapText(CBitmapFont* font);
		void AddBitmapText(CBitmapText* text);
		void RemoveBitmapFont(CBitmapFont* font);
		void RemoveBitmapText(CBitmapText* text);
		CBitmapFont* DefaultFont();
	};

	class CCamera : public CDynamic
	{
	private:
		XMMATRIX view;
		XMMATRIX proj;
		XMVECTOR up;
		XMMATRIX GetScaleMatrix() override;
		void CamTransform();
		float nearPlane;
		float farPlane;
		float fovAngle;
		float aspectRatio;

		friend void Render(CCore* d3d);
		friend class CDynamic;
		friend class CBitmapText;
		friend class CCore;
	public:
		CCamera();
		POINTF GetCursorWorldPos(float z);
		POINTF GetFrustumSize(float z);
		XMMATRIX GetViewMatrix();
		XMMATRIX GetProjMatrix();
		void Destroy();
	};

	class CInput
	{
	private:
		int keyCount;
		bool* curState;
		bool* prevState;
		POINT* curMouse;
		POINT* prevMouse;
		int mouseWheel;
		void Activity();

		friend void Render(CCore* d3d);
		friend LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	public:
		CInput();
		int GetMouseWheel();
		bool IsKeyDown(int vkey);
		bool IsKeyPressed(int vkey);
		bool IsKeyReleased(int vkey);
		bool IsAnyKeyDown();
		POINT GetCursorDelta();
		char GetChar(bool enableShift = true, bool enableCapslock = true);
		//offset, where to start looking
		bool IsCapslockActive();
		char GetKey(int offset);
		void ResetKey(int vkey);		
		void Destroy();
	};

	class CSprite : public CDrawable, public CDynamic
	{
	protected:
		wstring resource;
		ID3D11ShaderResourceView* shaderResource;
		bool uncachedTex; //if true sprite will destroy it's own shaderResource
		XMMATRIX GetScaleMatrix() override;
		virtual void Play(){}
		void Draw() override;

		friend class CDrawManager;
	public:
		CTexture* GetTexture();
		TEX_FILTER TexFilter; //point or linear
		XMFLOAT2 Scale;
		bool FlipHorizontally;
		bool FlipVertically;
		CSprite();
		CSprite(CTexture* texture);
		CSprite(const WCHAR* file);
		//sets up scale to match texture size
		void SetNaturalScale();
		//set up scale to match given size
		void SetPixelScale(int width, int height);
		void Destroy();
	};

	class CEventManager
	{
	private:
		vector < Event* > events;
		void Activity();

		friend void Render(CCore* d3d);
	public:
		void AddEvent(std::function<int()> func, wstring name, double delay);
		void RemoveEvent(wstring name);		
		void Destroy();
	};

	class CAnimation : public CSprite
	{
	private:
		double lastFrameChangeTime;
		int frameCount;
		vector<UV> uvTable;
		double indicator;
		bool frameChanged;
		//plays animation
		void Play() override;
	public:
		CAnimation(const WCHAR* file, int x, int y);
		int Start; //first frame of currently played animation
		int Finish; //last frame of currently played animation
		int Frame; //current frame displayed
		double Speed; //in frames per second
		bool FrameChanged(); //true if frame changed in prev game frame
		//set frames order, size of int should be the same as frame count
		void SetOrder(int* order);
		int GetFrameCount();
		//next frame and loop
		void NextFrame();
		//prev frame and loop
		void PreviousFrame();
	};

	class CBitmapFont
	{
	private:
		vector<UV> chars;
		wstring resName;
		ID3D11ShaderResourceView* shaderResource;
		friend class CBitmapText;
	public:
		CBitmapFont(const WCHAR* file, vector<UV> _chars);
		CBitmapFont(CTexture* texture, vector<UV> _chars);
		wstring GetName();
		void Destroy();
	};

	enum class TextHorAlign { Left, Center, Right };
	enum class TextVerAlign { Top, Center, Bottom };

	class CBitmapText : public CDrawable, public CDynamic
	{
	private:
		CBitmapFont* font;
		void Draw() override;
		XMMATRIX GetScaleMatrix() override;
		void TextTransform(int col, int row, int len);

		friend class CDrawManager;
	public:
		TextHorAlign HorizontalAlign;
		TextVerAlign VerticalAlign;
		CBitmapText(CBitmapFont* _font);
		TEX_FILTER TexFilter; //point or linear
		wstring Text;
		float Size;
		float Height;
		float Width;
		float HorizontalSpacing;
		float VerticalSpacing;
		void Destroy();		
	};

	class CDebugManager
	{
	private:
		CBitmapText* debugText;
		std::wstringstream wss;		

		friend void Render(CCore* d3d);
	public:
		CDebugManager();
		void Init(CBitmapFont* font);
		void Debug(int debug, wstring name);
		void Debug(float debug, wstring name);
		void Debug(double debug, wstring name);
		void Debug(wstring debug, wstring name);
		//send wss to debugText and clear wss
		void Flush();
		void Destroy();
	};
}