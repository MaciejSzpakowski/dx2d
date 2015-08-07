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
#include <ctime>
#include <d3dcompiler.h>
#include <Xinput.h>
#include "Keys.h"

#pragma comment(lib,"Gdiplus.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3DCompiler.lib")
#ifdef XINPUT_14
#pragma comment(lib, "Xinput.lib")
#else
#pragma comment(lib, "Xinput9_1_0.lib")
#endif
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
	class CRenderTarget;

	//externals
	extern CCore* Core;
	extern CKey Key;
	extern CButton Button;
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
		CCore* InitCore(int sizex, int sizey, std::function<void()> worker, int style = WS_OVERLAPPEDWINDOW);
		
		//creates new ID3D11Texture2D object from bytes
		//you probably dont want to use it unless you want to deal with directx directly
		ID3D11Texture2D* CreateTexture2DFromBytes(BYTE* data, int width, int height);
		
		//creates new ID3D11Texture2D object from Gdibitmap
		//you probably dont want to use it unless you want to deal with directx directly
		ID3D11Texture2D* CreateTexture2DFromGdibitmap(Gdiplus::Bitmap* _gdibitmap);
		
		//creates new ID3D11Texture2D object from file
		//you probably dont want to use it unless you want to deal with directx directly
		ID3D11Texture2D* CreateTexture2DFromFile(LPCWSTR file);
		
		//creates new ID3D11Texture2D object from resource
		//you probably dont want to use it unless you want to deal with directx directly
		ID3D11Texture2D* CreateTexture2DFromResource(int resource);
		
		//creates CTexture obj that can be used to create sprite or font
		//source: raw bytes, format: one pixel is 4 bytes in order RGBA
		//not cached
		CTexture* GetUncachedTextureFromBytes(BYTE* data, int width, int height);
		
		//creates CTexture obj that can be used to create sprite or font
		//source: file
		CTexture* GetCachedTextureFromFile(LPCWSTR file);

		//creates CTexture obj that can be used to create sprite or font
		//source: internal resource
		//name: unique name
		CTexture* GetCachedTextureFromResource(int resource, wstring name);

		//check for HR when directx init functions are called
		void Checkhr(LPCSTR file, int line);

		//return double between 0 and 1
		double RndDouble();

		//return integer
		//between inclusive min and exclusive max
		int RndInt(int min, int max);

		//ps from file
		//entryPoint: main of the ps
		//target: ps version, ps_5_0 by default
		ID3D11PixelShader* CreatePSFromFile(LPCWSTR file, LPCSTR entryPoint, LPCSTR target = "ps_5_0");
		
		//ps from string
		//entryPoint: main of the ps
		//target: ps version, ps_5_0 by default
		ID3D11PixelShader* CreatePSFromString(LPCSTR string, LPCSTR entryPoint, LPCSTR target = "ps_5_0");
	}

	namespace Collision
	{
		bool IsColliding(CCircle* c1, CCircle* c2);

		bool IsColliding(CCircle* c, CRectangle* r);

		bool IsColliding(CCircle* c, CPolygon* p);

		//very efficient collision check for rectangles
		//if rectangle Rotation.z is 0 it's considered axis aligned
		//if r1 and r2 are axis aligned, the test is even faster
		bool IsColliding(CRectangle* r1, CRectangle* r2);

		//the most powerful (and inefficient) collision check
		//pointOfCollision can be used to get the first point of collision
		//this one checks collision line by line, if any of the two polygons
		//lines intersects it counts as collision
		//otherwise it's not a collision even if one poly is inside the another
		//polygons dont have to be closed
		bool IsColliding(CPolygon* p1, CPolygon* p2, XMFLOAT3* pointOfCollision);

		//uses separated axis theorem to check for collision
		//faster than line by line check but polygons must be closed and convex !
		bool IsCollidingSat(CPolygon* p1, CPolygon* p2);
	}
}