#pragma once
#include <Windows.h>
#include <functional>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
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
	class CTexture;

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

	namespace Functions
	{
		//creates new CCore object so you dont have to use new operator
		CCore* NewCore(int sizex, int sizey, std::function<void()> worker, int style = WS_OVERLAPPEDWINDOW);
		
		//creates new ID3D11Texture2D object from bytes
		//you probably dont want to use it unless you want to deal with directx directly
		ID3D11Texture2D* CreateTexture2DFromBytes(BYTE* data, int width, int height);
		
		//creates new ID3D11Texture2D object from Gdibitmap
		//you probably dont want to use it unless you want to deal with directx directly
		ID3D11Texture2D* CreateTexture2DFromGdibitmap(Gdiplus::Bitmap* _gdibitmap);
		
		//creates new ID3D11Texture2D object from file
		//you probably dont want to use it unless you want to deal with directx directly
		ID3D11Texture2D* CreateTexture2DFromFile(const WCHAR* file);
		
		//creates new ID3D11Texture2D object from resource
		//you probably dont want to use it unless you want to deal with directx directly
		ID3D11Texture2D* CreateTexture2DFromResource(int resource);
		
		//creates CTexture obj that can be used to create sprite or font
		//source: raw bytes, format: one pixel is 4 bytes in order RGBA
		//not cached
		CTexture* GetUncachedTextureFromBytes(BYTE* data, int width, int height);
		
		//creates CTexture obj that can be used to create sprite or font
		//source: file
		CTexture* GetCachedTextureFromFile(const WCHAR* file);

		//creates CTexture obj that can be used to create sprite or font
		//source: internal resource
		//name: unique name
		CTexture* GetCachedTextureFromResource(int resource, wstring name);

		//check for HR when directx init functions are called
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
		bool IsColliding(CSprite* s1, CSprite* s2);
	}
}