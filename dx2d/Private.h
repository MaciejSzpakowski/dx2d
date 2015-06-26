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
	using namespace DirectX;
	using std::vector;
	using std::wstring;
	using std::wstringstream;

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
		double lifeTime;
		double tick;
		double lastPulse;
		wstring Name;
		std::function<int()> Activity;
	};

	//used to cache texture loaded from file
	class CTexture
	{
	public:
		int zHeight;
		int zWidth;
		wstring zName;
		ID3D11ShaderResourceView* zShaderResource;

		int GetWidth(){ return zWidth; }
		int GetHeight(){ return zHeight; }
		wstring GetName(){ return zName;}
		//destructor exclusive for resource manager
		~CTexture()
		{
			zShaderResource->Release();
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

	namespace Collision
	{
		bool IsColliding(CCircle* c1, CCircle* c2);
	}

	class CWindow
	{
	public:
		HWND zHandle;
		MSG zMsg;
		std::function<void()> zWorker;
		std::function<void(CCore* d3d)> zRender;
		CWindow(int sizex, int sizey, int style);
		int zRun();
	};

	class CCore
	{
	public:
		CWindow* zWindow;
		ID3D11BlendState* zBlendState;
		IDXGISwapChain* zSwapChain;
		ID3D11RenderTargetView* zBackBuffer;
		ID3D11Device* zDevice;
		ID3D11DeviceContext* zContext;
		ID3D11VertexShader* zDefaultVS;
		ID3D11PixelShader* zDefaultPS;
		ID3D11InputLayout* zLayout; //vertex input layout pos:float[3] col:float[3] uv:float[2]
		ID3D11DepthStencilView* zDepthStencilView;
		ID3D11Texture2D* zDepthStencilBuffer;
		float zBackBufferColor[4];
		double zFrequency;
		long long zStartTime;
		long long zPrevFrameTime;
		double zGameTime;
		double zFrameTime;
		void zUpdateGameTime();
		bool zFullscreen;
		POINT zClientSize;

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
	public:
		std::map<wstring, CTexture*> zTextures;
		std::map<wstring, CBitmapFont*> zFonts;

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
	public:
		XMVECTOR zPosition;
		XMVECTOR zRotation;
		XMVECTOR zVelocity;
		XMVECTOR zAcceleration;
		XMVECTOR zAngularVel;
		XMVECTOR zAngularAcc;
		ID3D11Buffer* zCbBufferVS;
		//matrix algebra for
		//produces worldViewProj used by VS
		void zTransform();
		//updates PVAJ etc.
		void zUpdate();
		virtual XMMATRIX zGetScaleMatrix() = 0;
		virtual void zCheckForCursor(XMMATRIX transform){}
		bool zUnderCursor;
		
		bool IsUnderCursor();		
		CDynamic();
		~CDynamic();

		bool Pickable;
		CDynamic* Parent;

		//repetitive code
		void SetPosition(XMFLOAT3 v){zPosition = XMLoadFloat3(&v);}
		void SetPosition(float x, float y, float z){zPosition = XMVectorSet(x, y, z, 1);}
		void SetPositionX(float x){zPosition = XMVectorSetX(zPosition, x);}
		void SetPositionY(float y){ zPosition = XMVectorSetY(zPosition, y); }
		void SetPositionZ(float z){ zPosition = XMVectorSetZ(zPosition, z); }
		XMFLOAT3 GetPosition(){ XMFLOAT3 v; XMStoreFloat3(&v, zPosition); return v; }
		XMVECTOR GetPositionVector(){return zPosition;}
		void SetVelocity(XMFLOAT3 v){ zVelocity = XMLoadFloat3(&v); }
		void SetVelocity(float x, float y, float z){ zVelocity = XMVectorSet(x, y, z, 1); }
		void SetVelocityX(float x){ zVelocity = XMVectorSetX(zVelocity, x); }
		void SetVelocityY(float y){ zVelocity = XMVectorSetY(zVelocity, y); }
		void SetVelocityZ(float z){ zVelocity = XMVectorSetZ(zVelocity, z); }
		XMFLOAT3 GetVelocity(){ XMFLOAT3 v; XMStoreFloat3(&v, zVelocity); return v; }
		void SetAcceleration(XMFLOAT3 v){ zAcceleration = XMLoadFloat3(&v); }
		void SetAcceleration(float x, float y, float z){ zAcceleration = XMVectorSet(x, y, z, 1); }
		void SetAccelerationX(float x){ zAcceleration = XMVectorSetX(zAcceleration, x); }
		void SetAccelerationY(float y){ zAcceleration = XMVectorSetY(zAcceleration, y); }
		void SetAccelerationZ(float z){ zAcceleration = XMVectorSetZ(zAcceleration, z); }
		XMFLOAT3 GetAcceleration(){ XMFLOAT3 v; XMStoreFloat3(&v, zAcceleration); return v; }
		void SetRotation(XMFLOAT3 v){ zRotation = XMLoadFloat3(&v); }
		void SetRotation(float x, float y, float z){ zRotation = XMVectorSet(x, y, z, 1); }
		void SetRotationX(float x){ zRotation = XMVectorSetX(zRotation, x); }
		void SetRotationY(float y){ zRotation = XMVectorSetY(zRotation, y); }
		void SetRotationZ(float z){ zRotation = XMVectorSetZ(zRotation, z); }
		XMFLOAT3 GetRotation(){ XMFLOAT3 v; XMStoreFloat3(&v, zRotation); return v; }
		void SetAngularVel(XMFLOAT3 v){ zAngularVel = XMLoadFloat3(&v); }
		void SetAngularVel(float x, float y, float z){ zAngularVel = XMVectorSet(x, y, z, 1); }
		void SetAngularVelX(float x){ zAngularVel = XMVectorSetX(zAngularVel, x); }
		void SetAngularVelY(float y){ zAngularVel = XMVectorSetY(zAngularVel, y); }
		void SetAngularVelZ(float z){ zAngularVel = XMVectorSetZ(zAngularVel, z); }
		XMFLOAT3 GetAngularVel(){ XMFLOAT3 v; XMStoreFloat3(&v, zAngularVel); return v; }
		void SetAngularAcc(XMFLOAT3 v){ zAngularAcc = XMLoadFloat3(&v); }
		void SetAngularAcc(float x, float y, float z){ zAngularAcc = XMVectorSet(x, y, z, 1); }
		void SetAngularAccX(float x){ zAngularAcc = XMVectorSetX(zAngularAcc, x); }
		void SetAngularAccY(float y){ zAngularAcc = XMVectorSetY(zAngularAcc, y); }
		void SetAngularAccZ(float z){ zAngularAcc = XMVectorSetZ(zAngularAcc, z); }
		XMFLOAT3 GetAngularAcc(){ XMFLOAT3 v; XMStoreFloat3(&v, zAngularAcc); return v; }		
	};

	class CDrawable
	{
	public:
		int zVertexCount;
		int zIndex;
		ID3D11Buffer* zVertexBuffer;
		ID3D11Buffer* zCbBufferPS;
		ID3D11Buffer* zCbBufferUV;
		virtual void zDraw() = 0;

		CDrawable();			
		~CDrawable();

		bool Visible;
		Color Color;
		UV uv;
	};

	class CPolygon : public CDrawable, public CDynamic
	{
	public:
		XMMATRIX zGetScaleMatrix() override;
		void zDraw() override;

		CPolygon();
		CPolygon(XMFLOAT2 points[], int n);		
		void Destroy();
	};

	class CRectangle : public CPolygon
	{
	public:
		XMMATRIX zGetScaleMatrix() override;

		XMFLOAT2 Scale;
		CRectangle(float scalex, float scaley);
	};

	class CCircle : public CPolygon
	{
	public:
		XMMATRIX zGetScaleMatrix() override;

		float Radius;
		CCircle(float radius, unsigned char resolution);
	};

	class CDrawManager
	{
	public:
		CBitmapFont* zDefaultFont;
		ID3D11ShaderResourceView* zWhiteRes;
		ID3D11SamplerState* zPointSampler;
		ID3D11SamplerState* zLineSampler;
		ID3D11Buffer* zIndexBufferSprite;
		ID3D11Buffer* zVertexBufferSprite;
		vector<CPolygon*> zPolygons;
		vector<CSprite*> zSprites;
		vector<CBitmapText*> zTexts;
		ID3D11RasterizerState* zWireframe;
		ID3D11RasterizerState* zSolid;
		void zDrawAll();

		CDrawManager();
		void Destroy();
		CPolygon* AddPoly(XMFLOAT2 points[], int n);
		CRectangle* AddRect(float sizex, float sizey);
		CCircle* AddCircle(float radius, unsigned char resolution);
		void AddPoly(CPolygon* p);		
		void RemovePoly(CPolygon* p);
		//add sprite from file
		//supported file formats: BMP, GIF, JPEG, PNG, TIFF, Exif, WMF, EMF
		CSprite* AddSprite(const WCHAR* texture);
		void AddSprite(CSprite* s);
		void RemoveSprite(CSprite* s);
		CAnimation* AddAnimation(const WCHAR* texture, int x, int y);
		void AddAnimation(CAnimation* a);
		void RemoveAnimation(CAnimation* a);		
		CBitmapFont* AddBitmapFont(const WCHAR* file, vector<UV> chars);
		void AddBitmapFont(CBitmapFont* font);
		CBitmapText* AddBitmapText(CBitmapFont* font);
		void AddBitmapText(CBitmapText* text);
		void RemoveBitmapFont(CBitmapFont* font);
		void RemoveBitmapText(CBitmapText* text);
		CBitmapFont* DefaultFont();

		TEX_FILTER TexFilterCreationMode;
	};

	class CCamera : public CDynamic
	{
	public:
		XMMATRIX zView;
		XMMATRIX zProj;
		XMVECTOR zUp;
		XMMATRIX zGetScaleMatrix() override;
		void zCamTransform();
		float zNearPlane;
		float zFarPlane;
		float zFovAngle;
		float zAspectRatio;

		CCamera();
		POINTF GetCursorWorldPos(float z);
		POINTF GetFrustumSize(float z);
		XMMATRIX GetViewMatrix();
		XMMATRIX GetProjMatrix();
		void Destroy();
	};

	class CInput
	{
	public:
		int zKeyCount;
		bool* zCurState;
		bool* zPrevState;
		POINT* zCurMouse;
		POINT* zPrevMouse;
		int zMouseWheel;
		void zActivity();

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
	public:
		wstring zResource;
		ID3D11ShaderResourceView* zShaderResource;
		bool zUncachedTex; //if true sprite will destroy it's own shaderResource
		XMMATRIX zGetScaleMatrix() override;
		virtual void zPlay(){}
		void zDraw() override;
		void zCheckForCursor(XMMATRIX transform) override;

		CTexture* GetTexture();		
		CSprite();
		CSprite(CTexture* texture);
		CSprite(const WCHAR* file);
		//sets up scale to match texture size
		void SetNaturalScale();
		//set up scale to match given size
		void SetPixelScale(int width, int height);
		void Destroy();

		TEX_FILTER TexFilter; //point or linear
		XMFLOAT2 Scale;
		bool FlipHorizontally;
		bool FlipVertically;
	};

	class CEventManager
	{
	public:
		vector < Event* > zEvents;
		void zActivity();

		//func: code to execute by event (function pointer or closure)
		//delay: how long to wait before start running
		//lifeTime: destroy event after that time, 0 to never destroy
		//tick: run event once every tick seconds, 0 to run every frame
		void AddEvent(std::function<int()> func, wstring name, double delay, double lifeTime, double tick);
		void RemoveEvent(wstring name);		
		void Destroy();
	};

	class CAnimation : public CSprite
	{
	public:
		double zLastFrameChangeTime;
		int zFrameCount;
		vector<UV> zUvTable;
		double zIndicator;
		bool zFrameChanged;
		//plays animation
		void zPlay() override;
		
		CAnimation(const WCHAR* file, int x, int y);
		bool FrameChanged(); //true if frame changed in prev game frame
		//set frames order, size of int should be the same as frame count
		void SetOrder(int* order);
		int GetFrameCount();
		//next frame and loop
		void NextFrame();
		//prev frame and loop
		void PreviousFrame();
		
		int Start; //first frame of currently played animation
		int Finish; //last frame of currently played animation
		int Frame; //current frame displayed
		double Speed; //in frames per second
	};

	class CBitmapFont
	{
	public:
		vector<UV> zChars;
		wstring zResName;
		ID3D11ShaderResourceView* zShaderResource;

		CBitmapFont(const WCHAR* file, vector<UV> _chars);
		CBitmapFont(CTexture* texture, vector<UV> _chars);
		wstring GetName();
		void Destroy();
	};

	enum class TextHorAlign { Left, Center, Right };
	enum class TextVerAlign { Top, Center, Bottom };

	class CBitmapText : public CDrawable, public CDynamic
	{
	public:
		CBitmapFont* zFont;
		void zDraw() override;
		XMMATRIX zGetScaleMatrix() override;
		void zTextTransform(int col, int row, int len);

		CBitmapText(CBitmapFont* _font);
		//set up scale to match given size
		void SetPixelScale(int width, int height);
		void Destroy();

		TextHorAlign HorizontalAlign;
		TextVerAlign VerticalAlign;		
		TEX_FILTER TexFilter; //point or linear
		wstring Text;
		float Size;
		float Height;
		float Width;
		float HorizontalSpacing;
		float VerticalSpacing;		
	};

	class CDebugManager
	{
	public:
		CBitmapText* zDebugText;
		std::wstringstream zWss;	

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