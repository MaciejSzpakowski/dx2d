#pragma once
#include <Windows.h>
#include <functional>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include "Keys.h"
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
	class CText;
	class CEventManager;
	class CResourceManager;
	class CAnimation;
	class CBitmapFont;
	class CBitmapText;

	//externals
	extern CCore* Core;
	extern CKey Key;
	extern CDrawManager* DrawManager;
	extern CCamera* Camera;
	extern CInput* Input;
	extern CEventManager* EventManager;
	extern CResourceManager* ResourceManager;
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

	struct SColor
	{
		float r, g, b, a;
		SColor(){}
		SColor(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a){}
		SColor& operator = (const XMFLOAT4& other)
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
		std::string Name;
		std::function<int()> Activity;
	};

	//used to cache texture loaded from file
	class CTexture
	{
	public:
		int Height;
		int Width;
		std::wstring fileName;
		ID3D11ShaderResourceView* shaderResource;
		//destructor exclusive for resource manager
		~CTexture()
		{
			shaderResource->Release();
		}
	};

	namespace Functions
	{
		CCore* NewCore(int sizex, int sizey, std::function<void()> worker);
		ID3D11Texture2D* CreateTexture2D(BYTE* data, int width, int height);
		ID3D11Texture2D* CreateTexture2DFromFile(const WCHAR* file);
		ID3D11Texture2D* CreateTexture2DFromText(std::wstring text);
		void LoadCachedTexture(const WCHAR* textureFile, ID3D11ShaderResourceView*& shader);
		void Checkhr(const char* file, int line);
		//return double between 0 and 1
		double RndDouble();
		//return integer
		//between inclusive min and exclusive max
		int RndInt(int min, int max);
	}

	class Window
	{
	private:
		HWND handle;
		MSG Msg;
		std::function<void()> Worker;
		std::function<void(CCore* d3d)> Render;
		friend class CCore;
	public:
		Window(int sizex, int sizey);
		int Run();
	};	

	class CCore
	{
	private:
		Window* window;
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

		friend void Render(CCore* d3d);
		friend ID3D11DeviceContext* GetContext();
		friend ID3D11Device* GetDevice();
		friend void AddFloat3(XMFLOAT3* src, XMFLOAT3* dst);
		friend class CDrawManager;
	public:
		ID3D11BlendState* blendState;
		CCore(int sizex, int sizey, std::function<void()> worker);
		HWND GetWindowHandle();
		CCamera* GetCamera();
		CInput* GetInputManager();
		void SetWindowTitle(const char* title);
		void SetBackgroundColor(SColor color);
		int Run();
		void OpenConsole();
		void CloseConsole();
		double GetGameTime();
		//time per frame, to get FPS -> 1 / GetFrameTime()
		double GetFrameTime();
		double GetFps();
		void SetFullscreen(bool state);
		bool GetFullscreen();
		void Destroy();
	};	

	class CResourceManager
	{
	private:
		std::map<std::wstring, CTexture*> textures;
		std::map<std::wstring, CBitmapFont*> fonts;
	public:
		void AddTexture(CTexture* tex);
		CTexture* GetTexture(std::wstring file);
		void AddBitmapFont(CBitmapFont* font);
		CBitmapFont* GetBitmapFont(std::wstring file);
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
		XMFLOAT3 Spin;
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

		friend class CDrawManager;
	public:
		CDrawable();
		bool Visible;
		SColor Color;
		UV uv;
		virtual void Draw() = 0;
		~CDrawable();
	};

	class CPolygon : public CDrawable,public CDynamic
	{
	protected:		
		XMMATRIX GetScaleMatrix() override;
	public:
		CPolygon();
		CPolygon(XMFLOAT2 points[], int n);		
		void Draw() override;
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

		friend void Render(CCore* d3d);
		friend class CSprite;
		friend class CAnimation;
	public:
		CDrawManager();
		CPolygon* AddPoly(XMFLOAT2 points[], int n);
		CRectangle* AddRect(float sizex, float sizey);
		CCircle* AddCircle(float radius, unsigned char resolution);
		void AddPoly(CPolygon* p);
		void DrawAll();
		void Destroy();
		void RemovePoly(CPolygon* p);		
		//add sprite from file
		//supported file formats: BMP, GIF, JPEG, PNG, TIFF, Exif, WMF, EMF
		CSprite* AddSprite(const WCHAR* texture);
		void AddSprite(CSprite* s);
		CText* AddText(std::wstring text);
		void AddText(CText* t);
		void RemoveSprite(CSprite* s);
		void RemoveText(CText* t);
		CAnimation* AddAnimation(const WCHAR* texture, int x, int y);
		void AddAnimation(CAnimation* a);
		void RemoveAnimation(CAnimation* a);
		TEX_FILTER TexFilterCreationMode;
		CBitmapFont* AddBitmapFont(const WCHAR* file, vector<UV> chars);
		CBitmapText* AddBitmapText(CBitmapFont* font);
		void AddBitmapText(CBitmapText* text);
		void RemoveBitmapFont(CBitmapFont* font);
		void RemoveBitmapText(CBitmapText* text);
	};	

	class CCamera : public CDynamic
	{
	private:		
		XMMATRIX view;
		XMMATRIX proj;
		XMVECTOR up;		
		XMMATRIX GetScaleMatrix() override;
		void CamTransform();

		friend void Render(CCore* d3d);
		friend class CDynamic;
		friend class CBitmapText;
	public:
		CCamera();
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
	public:
		CInput();
		int MouseWheel;
		bool IsKeyDown(int vkey);
		bool IsKeyPressed(int vkey);
		bool IsKeyReleased(int vkey);
		bool IsAnyKeyDown();
		POINT GetCursorDelta();
		char GetChar(bool enableShift=true, bool enableCapslock=true);
		//offset, where to start looking
		bool IsCapslockActive();
		char GetKey(int offset);
		void ResetKey(int vkey);
		void Activity();
		void Destroy();
	};	

	class CSprite : public CDrawable, public CDynamic
	{
	protected:		
		ID3D11ShaderResourceView* shaderResource;
		bool uncachedTex; //if true sprite will destroy it's own shaderResource
		XMMATRIX GetScaleMatrix() override;	
		virtual void Play(){}

		friend class CDrawManager;
	public:
		TEX_FILTER TexFilter; //point or linear
		XMFLOAT2 Scale;
		bool FlipHorizontally;
		bool FlipVertically;
		CSprite();
		CSprite(const WCHAR* texture);
		void Draw() override;
		void Destroy();		
	};

	class CText : public CSprite
	{
	protected:
	public:
		CText(std::wstring text);
	};	

	class CEventManager
	{
	private:
		vector < Event* > events;
	public:
		void AddEvent(std::function<int()> func,std::string name, double delay);
		void RemoveEvent(std::string name);
		void Activity();
		void Destroy();
	};

	class CAnimation : public CSprite
	{
	private:
		double lastFrameChangeTime;
		int frameCount;
		vector<UV> uvTable;
		double indicator;
		//plays animation
		void Play() override;
	public:
		CAnimation(const WCHAR* file,int x,int y);
		int Start; //first frame of currently played animation
		int Finish; //last frame of currently played animation
		int Frame; //current frame displayed
		double Speed; //in frames per second
		bool FrameChanged; //true if frame changed in prev game frame
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
		std::wstring fileName;
		ID3D11ShaderResourceView* shaderResource;
		friend class CBitmapText;
	public:
		CBitmapFont(const WCHAR* file, vector<UV> _chars);
		std::wstring GetFileName();
		void Destroy();
	};

	enum class TextHorAlign { Left, Center, Right };
	enum class TextVerAlign { Top, Center, Bottom };

	class CBitmapText : public CDrawable, public CDynamic
	{
	private:
		CBitmapFont* font;
	public:
		TextHorAlign HorizontalAlign;
		TextVerAlign VerticalAlign;
		CBitmapText(CBitmapFont* _font);
		TEX_FILTER TexFilter; //point or linear
		std::wstring Text;
		void Draw() override;
		XMMATRIX GetScaleMatrix() override;
		void Destroy();
		void TextTransform(int col, int row, int len);
	};
}