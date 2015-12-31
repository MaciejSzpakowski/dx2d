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

#define CHECKHR() Functions::Checkhr(hr,__LINE__)
#define VIVA_ERROR(msg) VivaError(__FUNCSIG__,msg)

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
	class CDrawManager;
	class Camera;
	class RenderTarget;
	class CResourceManager;
	class CInputManager;
	class CEventManager;
	class CDebugManager;

	//externals
	extern CCore* Core;
	extern CDrawManager* DrawManager;	
	extern CResourceManager* ResourceManager;
	extern CInputManager* InputManager;
	extern CEventManager* EventManager;
	extern CDebugManager* DebugManager;
}