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
		bool IsColliding(CSprite* s1, CSprite* s2);
	}
}