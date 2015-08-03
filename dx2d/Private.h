#pragma once
#include "Proto.h"

namespace dx2d
{
#define CHECKHR() Functions::Checkhr(__FILE__,__LINE__)	

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

	struct Rect
	{
		float left;
		float top;
		float right;
		float bottom;
		Rect(){}
		Rect(float _left, float _top, float _right, float _bottom) :
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
		bool zCached;
		int zHeight;
		int zWidth;
		wstring zName;
		ID3D11ShaderResourceView* zShaderResource;

		CTexture(bool cached, int height, int width, wstring name, 
			ID3D11ShaderResourceView* shaderResource) :
			zCached(cached),zHeight(height),zWidth(width),zName(name),
			zShaderResource(shaderResource){}
		int GetWidth(){ return zWidth; }
		int GetHeight(){ return zHeight; }
		wstring GetName(){ return zName;}
		void Destroy()
		{
			zShaderResource->Release();
			delete this;
		}				
	};	

	class CWindow
	{
	public:
		HWND zHandle;
		MSG zMsg;
		std::function<void()> zWorker;
		std::function<void()> zActivity;
		CWindow(int sizex, int sizey, int style);
		int zRun();
		void Destroy();
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
		ID3D11PixelShader* zDefaultPost;
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
		void SetWindowTitle(LPCWSTR title);
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
		void Destroy();
		void SaveScreenshot(LPCWSTR file);
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
		XMMATRIX zWorld;
		XMVECTOR zPosition;
		XMVECTOR zRotation;
		XMVECTOR zVelocity;
		XMVECTOR zAcceleration;
		XMVECTOR zAngularVel;
		XMVECTOR zAngularAcc;		
		CDynamic* zParent;
		vector<CDynamic*> zChildren;
		ID3D11Buffer* zCbBufferVS;
		//matrix algebra for
		//produces worldViewProj used by VS
		void zTransform();
		//updates PVAJ etc.
		void zUpdate();
		virtual XMMATRIX zGetScaleMatrix() = 0;
		virtual void zCheckForCursor(XMMATRIX transform){}
		bool zUnderCursor;
		
		CDynamic* GetParent();
		void SetParent(CDynamic* parent);
		vector<CDynamic*> GetChildren();
		XMMATRIX GetWorldMatrix();
		bool IsUnderCursor();		
		CDynamic();
		~CDynamic();

		bool Pickable;		
		float SizeVel;
		float SizeAcc;
		float Size;
		XMFLOAT2 Origin;

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
		CRenderTarget* zRenderTarget;
		int zVertexCount;
		int zIndex;
		ID3D11Buffer* zVertexBuffer;
		ID3D11Buffer* zCbBufferPS;		
		ID3D11Buffer* zCbBufferUV;
		//extra buffer
		ID3D11Buffer* zCbBufferPSExtra;
		void* zExtraBufferData;
		virtual void zDraw() = 0;

		CDrawable();

		//used to send custom data to PS
		//size must be multiple of 16, always round up to nearest 16
		void SetConstantBufferPS(void* data, UINT size);
		~CDrawable();
		CRenderTarget* GetRenderTarget(){ return zRenderTarget; }
		void SetRenderTarget(CRenderTarget* target);

		bool Visible;
		Color Color;
		Rect UV;
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

	class CRenderTarget
	{
	public:
		ID3D11Texture2D* zTexture;
		ID3D11RenderTargetView* zTargetView;
		CSprite* zSprite;
		vector<CPolygon*> zPolygons;
		vector<CSprite*> zSprites;
		vector<CBitmapText*> zTexts;

		CRenderTarget() :
			zTexture(nullptr),
			zTargetView(nullptr),
			zSprite(nullptr),
			PixelShader(Core->zDefaultPost){}

		//draw all objects that belong to the render target
		void zDraw();

		//reorder render target
		void MoveToTop();

		//reorder render target
		void MoveToBottom();

		//reorder render target
		void MoveUp();

		//reorder render target
		void MoveDown();

		void Destroy();

		ID3D11PixelShader* PixelShader;
	};

	class CDrawManager
	{
	public:
		CRenderTarget* zDefaultRenderTarget;
		vector<CRenderTarget*> zRenderTargets;
		CBitmapFont* zDefaultFont;
		ID3D11ShaderResourceView* zWhiteRes;
		ID3D11SamplerState* zPointSampler;
		ID3D11SamplerState* zLineSampler;
		ID3D11Buffer* zIndexBufferSprite;
		ID3D11Buffer* zVertexBufferSprite;		
		ID3D11RasterizerState* zWireframe;
		ID3D11RasterizerState* zSolid;
		XMMATRIX zRenderTargetMatrix;
		void zDrawAll();
		bool zHasObject(CDrawable* d);
		void zRenderTargetTransform(int i);
		void zInit();

		CDrawManager();
		void Destroy();
		CPolygon* AddPoly(XMFLOAT2 points[], int n,
			CRenderTarget* target = nullptr);
		CRectangle* AddRect(float sizex, float sizey,
			CRenderTarget* target = nullptr);
		CCircle* AddCircle(float radius, unsigned char resolution,
			CRenderTarget* target = nullptr);
		void AddPoly(CPolygon* p,
			CRenderTarget* target = nullptr);
		void RemovePoly(CPolygon* p);

		//add sprite from file
		//supported file formats: BMP, GIF, JPEG, PNG, TIFF, Exif, WMF, EMF
		CSprite* AddSprite(LPCWSTR file, 
			CRenderTarget* target = nullptr);

		void AddSprite(CSprite* s,
			CRenderTarget* target = nullptr);
		void RemoveSprite(CSprite* s);
		CAnimation* AddAnimation(LPCWSTR file, int x, int y,
			CRenderTarget* target = nullptr);
		void AddAnimation(CAnimation* a,
			CRenderTarget* target = nullptr);
		void RemoveAnimation(CAnimation* a);		
		CBitmapFont* AddBitmapFont(LPCWSTR file, vector<Rect> chars);
		void AddBitmapFont(CBitmapFont* font);
		CBitmapFont* AddBitmapFont(LPCWSTR file, int width, int height, int charsPerRow);
		CBitmapText* AddBitmapText(CBitmapFont* font,
			CRenderTarget* target = nullptr);
		void AddBitmapText(CBitmapText* text,
			CRenderTarget* target = nullptr);
		void RemoveBitmapFont(CBitmapFont* font);
		void RemoveBitmapText(CBitmapText* text);
		CBitmapFont* GetDefaultFont();
		CRenderTarget* AddRenderTarget();
		void DestroyRenderTarget(CRenderTarget* target);
		void InitDefaultFont();

		//destroys all but default render target
		void ClearRenderTargets();

		TEX_FILTER TexFilterCreationMode;
		bool SupressDuplicateWarning;
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
		XMFLOAT3 GetCursorWorldPos(float z);
		XMFLOAT2 GetFrustumSize(float z);
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
		bool zActiveGamepads[XUSER_MAX_COUNT];
		XINPUT_STATE* zGamepadStatePrev;
		XINPUT_STATE* zGamepadStateCur;
		bool zGamepadConnected;
		bool zGamepadDisconnected;
		void zCheckGamepads();
		void zActivity();
		void zTest();
		
		CInput();

		// MOUSE
		int GetMouseWheel();
		POINT GetCursorDelta();
		POINT GetCursorClientPos();

		// KEYS (aslo mouse buttons) and KEYBOARD
		bool IsKeyDown(int vkey);
		bool IsKeyPressed(int vkey);
		bool IsKeyReleased(int vkey);
		bool IsAnyKeyDown();		
		char GetChar(bool enableShift = true, bool enableCapslock = true);
		//offset, where to start looking
		bool IsCapslockActive();
		char GetKey(int offset);
		void ResetKey(int vkey);

		//how many gamepads listen to
		//keeps this at minimum for efficiency
		//max count is XUSER_MAX_COUNT (currently 4)
		UINT AcceptGamepads;
		bool IsAnyButtonDown(UINT gamepad);
		bool IsButtonDown(UINT gamepad, int button);
		bool IsButtonPressed(UINT gamepad, int button);
		bool IsButtonReleased(UINT gamepad, int button);
		BYTE GetLeftTrigger(UINT gamepad);
		BYTE GetRightTrigger(UINT gamepad);
		SHORT GetLeftStickX(UINT gamepad);
		SHORT GetLeftStickY(UINT gamepad);
		SHORT GetRightStickX(UINT gamepad);
		SHORT GetRightStickY(UINT gamepad);
		BYTE GetLeftTriggerDelta(UINT gamepad);
		BYTE GetRightTriggerDelta(UINT gamepad);
		SHORT GetLeftStickXDelta(UINT gamepad);
		SHORT GetLeftStickYDelta(UINT gamepad);
		SHORT GetRightStickXDelta(UINT gamepad);
		SHORT GetRightStickYDelta(UINT gamepad);
		void SetMotorSpeed(UINT gamepad, USHORT speedLeft, USHORT speedRight);
		
		//technically: was gamepad connected during last read
		//generally: is that gamepad connected
		bool IsGamepadActive(UINT gamepad);

		//was a new gamepad detected between last and current read
		bool GamepadConnected();

		//was a gamepad lost between last and current read
		bool GamepadDisconnected();
		void Destroy();
	};

	class CSprite : public CDrawable, public CDynamic
	{
	public:		
		CTexture* zTexture;
		ID3D11ShaderResourceView* zShaderResource;
		XMMATRIX zGetScaleMatrix() override;
		virtual void zPlay(){}
		void zDraw() override;
		void zCheckForCursor(XMMATRIX transform) override;
		void zSpriteUpdate();
		
		CSprite();
		CSprite(CTexture* texture);
		CSprite(LPCWSTR file);

		CTexture* GetTexture(){ return zTexture; }

		//sets up scale to match texture size
		void SetNaturalScale();
		//set up scale to match given size
		void SetPixelScale(int width, int height);
		void Destroy();

		TEX_FILTER TexFilter; //point or linear
		XMFLOAT2 Scale;
		bool FlipHorizontally;
		bool FlipVertically;
		ID3D11PixelShader* PixelShader;
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
		vector<Rect> zUvTable;
		double zIndicator;
		bool zFrameChanged;
		//plays animation
		void zPlay() override;
		
		CAnimation(LPCWSTR file, int x, int y);
		bool FrameChanged(); //true if frame changed in prev game frame
		//set frames order, size of the array should be the same as frame count
		void SetOrder(int order[]);
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
		vector<Rect> zChars;
		CTexture* zTexture;
		ID3D11ShaderResourceView* zShaderResource;

		CBitmapFont(LPCWSTR file, vector<Rect> _chars);
		CBitmapFont(const WCHAR* file, int width, int height, int charsPerRow);
		CBitmapFont(CTexture* texture, vector<Rect> _chars);
		CBitmapFont(CTexture* texture, int width, int height, int charsPerRow);
		CTexture* GetTexture(){ return zTexture; }
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