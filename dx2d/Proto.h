#pragma once

#include <algorithm>
#include <functional>
#include <map>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <Windows.h>
#include <gdiplus.h>
#include <d3d11.h>
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <Xinput.h>

#pragma comment(lib,"Gdiplus.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3DCompiler.lib")
#ifdef XINPUT_14
#pragma comment(lib, "Xinput.lib")
#else
#pragma comment(lib, "Xinput9_1_0.lib")
#endif
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")

namespace Viva
{
	//using
	using std::vector;
	using std::wstring;
	using std::wstringstream;

	using DirectX::XMMATRIX;
	using DirectX::XMVECTOR;
	using DirectX::XMFLOAT2;
	using DirectX::XMFLOAT3;
	using DirectX::XMFLOAT4;

	//prototypes
	class CCore;
	class Polygon;
	class Rectangle;
	class Circle;
	class CDrawManager;
	class Camera;
	class CInputManager;
	class CSprite;
	class CEventManager;
	class CResourceManager;
	class Animation;
	class BitmapFont;
	class BitmapText;
	class CDebugManager;
	class CTexture;
	class CRenderTarget;

	//externals
	extern CCore* Core;
	extern CDrawManager* DrawManager;
	extern CInputManager* InputManager;
	extern CEventManager* EventManager;
	extern CResourceManager* ResourceManager;
	extern CDebugManager* DebugManager;
}