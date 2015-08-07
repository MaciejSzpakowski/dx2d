#include "Private.h"
#include <ctime>
#include <sstream>

namespace dx2d
{
	//globals
	CCore* Core;
	CKey Key;
	CButton Button;
	CDrawManager* DrawManager;
	CCamera* Camera;
	CInput* Input;
	CEventManager* EventManager;
	CResourceManager* ResourceManager;
	CDebugManager* DebugManager;

	HRESULT hr;

	void IntActivity()
	{
		Input->zActivity();
		EventManager->zActivity();
		Camera->zCamTransform();		
		DrawManager->zDrawAll();		
		Core->zUpdateGameTime();
	}	

	namespace Functions
	{
		CCore* InitCore(int sizex, int sizey, std::function<void()> worker, int style)
		{
			CCore* core = new CCore(sizex, sizey, worker, style);
			DrawManager->zInit();
			return core;
		}

		ID3D11Texture2D* CreateTexture2DFromBytes(BYTE* data, int width, int height)
		{
			ID3D11Texture2D *tex;
			D3D11_TEXTURE2D_DESC tdesc;
			D3D11_SUBRESOURCE_DATA tbsd;

			tbsd.pSysMem = (void *)data;
			tbsd.SysMemPitch = width * 4;
			tbsd.SysMemSlicePitch = height*width * 4;

			tdesc.Width = width;
			tdesc.Height = height;
			tdesc.MipLevels = 1;
			tdesc.ArraySize = 1;

			tdesc.SampleDesc.Count = 1;
			tdesc.SampleDesc.Quality = 0;
			tdesc.Usage = D3D11_USAGE_DEFAULT;
			tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

			tdesc.CPUAccessFlags = 0;
			tdesc.MiscFlags = 0;

			Core->zDevice->CreateTexture2D(&tdesc, &tbsd, &tex);

			return tex;
		}

		ID3D11Texture2D* CreateTexture2DFromGdibitmap(Gdiplus::Bitmap* gdibitmap)
		{
			UINT h = gdibitmap->GetHeight();
			UINT w = gdibitmap->GetWidth();

			HBITMAP hbitmap;
			Gdiplus::Color c(0, 0, 0);
			gdibitmap->GetHBITMAP(c, &hbitmap);

			BITMAP bitmap;
			GetObject(hbitmap, sizeof(bitmap), (LPVOID)&bitmap);
			BYTE* data = (BYTE*)bitmap.bmBits;

			FILE* f = fopen("raw.txt", "wb");

			for (int i = 0; i < 126000; i++)
			{
				if(i%30 == 0)
					fprintf(f, "\n");
				fprintf(f,"0x%x,", data[i]);
			}
			fclose(f);
			//have to swap red and blue because bitmap has pixels as integers
			//and shader will read in byte by byte instead of int by int
			//this for loop is ~10% of the function
			for (int i = 0; i < (int)(h*w * 4); i += 4)
			{
				BYTE temp = data[i];
				data[i] = data[i + 2];
				data[i + 2] = temp;
			}

			ID3D11Texture2D* tex = CreateTexture2DFromBytes(data, w, h);
			DeleteObject(hbitmap);
			return tex;
			
			/*Gdiplus::BitmapData* bitmapData = new Gdiplus::BitmapData;
			Gdiplus::Rect rect(0, 0, w, h);

			// Lock a 5x3 rectangular portion of the bitmap for reading.
			gdibitmap->LockBits(
				&rect,
				Gdiplus::ImageLockModeRead,
				PixelFormat32bppARGB,
				bitmapData);

			// Display the hexadecimal value of each pixel in the 5x3 rectangle.
			UINT* pixels = (UINT*)bitmapData->Scan0;

			gdibitmap->UnlockBits(bitmapData);

			delete bitmapData;
			ID3D11Texture2D* tex = CreateTexture2DFromBytes((BYTE*)pixels, w, h);
			return tex;/**/
		}

		ID3D11Texture2D* CreateTexture2DFromFile(LPCWSTR file)
		{
			ULONG_PTR m_gdiplusToken;
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

			Gdiplus::Bitmap* gdibitmap = new Gdiplus::Bitmap(file);
			if (gdibitmap->GetLastStatus() != 0)
			{
				std::wstringstream msg;
				msg << L"Could not open " << file;
				MessageBoxW(0, msg.str().c_str(), L"File error", MB_ICONEXCLAMATION);
				delete gdibitmap;
				Gdiplus::GdiplusShutdown(m_gdiplusToken);
				return nullptr;
			}
			ID3D11Texture2D* tex = CreateTexture2DFromGdibitmap(gdibitmap);
			delete gdibitmap;
			Gdiplus::GdiplusShutdown(m_gdiplusToken);
			return tex;
		}

		ID3D11Texture2D* CreateTexture2DFromResource(int resource)
		{
			ULONG_PTR m_gdiplusToken;
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

			Gdiplus::Bitmap* gdibitmap = Gdiplus::Bitmap::FromResource(GetModuleHandle(0),
				MAKEINTRESOURCE(resource));
			Gdiplus::Status status = gdibitmap->GetLastStatus();
			if (status != 0)
			{
				std::wstringstream msg;
				msg << L"Could not load " << resource;
				MessageBoxW(0, msg.str().c_str(), L"Resource error", MB_ICONEXCLAMATION);
				delete gdibitmap;
				Gdiplus::GdiplusShutdown(m_gdiplusToken);
				return nullptr;
			}
			ID3D11Texture2D* tex = CreateTexture2DFromGdibitmap(gdibitmap);
			delete gdibitmap;
			Gdiplus::GdiplusShutdown(m_gdiplusToken);
			return tex;
		}

		CTexture* GetCachedTextureFromFile(LPCWSTR file)
		{
			CTexture* res = ResourceManager->GetTexture(file);
			if (res != nullptr)
				return res;
			else
			{				
				D3D11_TEXTURE2D_DESC desc;
				auto tex = Functions::CreateTexture2DFromFile(file);
				if (tex == nullptr)
					throw 0;
				tex->GetDesc(&desc);
				ID3D11ShaderResourceView* srv;
				Core->zDevice->CreateShaderResourceView(tex, 0, &srv);
				tex->Release();
				CTexture* newRes = new CTexture(true, desc.Height, desc.Width, file, srv);
				//add to resources
				ResourceManager->AddTexture(newRes);
				return newRes;
			}
		}

		CTexture* GetCachedTextureFromResource(int resource, wstring name)
		{
			CTexture* res = ResourceManager->GetTexture(name);
			if (res != nullptr)
				return res;
			else
			{				
				D3D11_TEXTURE2D_DESC desc;
				auto tex = Functions::CreateTexture2DFromResource(resource);
				if (tex == nullptr)
					throw 0;
				tex->GetDesc(&desc);
				ID3D11ShaderResourceView* srv;
				Core->zDevice->CreateShaderResourceView(tex, 0, &srv);
				tex->Release();
				CTexture* newRes = new CTexture(true, desc.Height, desc.Width, name, srv);
				//add to resources
				ResourceManager->AddTexture(newRes);
				return newRes;
			}
		}

		CTexture* GetUncachedTextureFromBytes(BYTE* data, int width, int height)
		{
			auto tex = Functions::CreateTexture2DFromBytes(data, width, height);
			if (tex == nullptr)
			{
				MessageBox(0, L"GetUncachedTextureFromBytes(BYTE* data, int width, int height) failed",
					L"Error", MB_ICONEXCLAMATION);
				return nullptr;
			}
			ID3D11ShaderResourceView* srv;
			Core->zDevice->CreateShaderResourceView(tex, 0, &srv);
			tex->Release();
			CTexture* newRes = new CTexture(false, height, width, L"custom", srv);
			return newRes;
		}

		void Checkhr(const char* file, int line)
		{
			if (hr == 0)
				return;
			char str[128];
			FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0,
				hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
				str, 128, 0);
			std::wstringstream message;
			message << file << L" line: " << line << L"\n" << str;
			
			MessageBox(0, message.str().c_str(), L"HRESULT error", MB_ICONERROR);
			exit(1);
		}

		double RndDouble()
		{
			unsigned int p1 = rand();
			unsigned int p2 = rand() << 15;
			unsigned int p3 = rand() << 30;
			unsigned int t = p1 | p2 | p3;
			return (double)t / 0xffffffff;
		}

		int RndInt(int min, int max)
		{
			if (max <= min)
				return 0;
			unsigned int p1 = rand();
			unsigned int p2 = rand() << 15;
			unsigned int p3 = rand() << 30;
			unsigned int t = p1 | p2 | p3;
			return (int)t % (max - min) + min;
		}

		ID3D11PixelShader* CreatePSFromFile(LPCWSTR file, LPCSTR entryPoint, LPCSTR target)
		{
			ID3D11PixelShader* result;
			ID3D10Blob *ps;
			hr = D3DCompileFromFile(file, 0, 0, entryPoint, target, 0, 0, &ps, 0); CHECKHR();
			//D3DCompile
			hr = Core->zDevice->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), 0,
				&result); CHECKHR();
			ps->Release();
			return result;
		}

		ID3D11PixelShader* CreatePSFromString(LPCSTR string, LPCSTR entryPoint, LPCSTR target)
		{
			ID3D11PixelShader* result;
			
			ID3D10Blob *ps;
			hr = D3DCompile(string, strlen(string), 0, 0, 0, entryPoint, target, 0, 0,&ps,0); CHECKHR();
			//D3DCompile
			hr = Core->zDevice->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), 0,
				&result); CHECKHR();
			ps->Release();
			return result;
		}
	}

	namespace Collision
	{
		//the fastest possible collision check,
		//make aligned square region from zRadius
		//check if difference between x or y (position)
		//is more than the sum od radii
		bool CheckRegion(CPolygon* p1, CPolygon* p2)
		{
			float radi = p1->zRadius + p2->zRadius;
			XMFLOAT3 pos1 = p1->GetPosition();
			XMFLOAT3 pos2 = p2->GetPosition();
			return !(abs(pos1.x - pos2.x) > radi ||
				abs(pos1.y - pos2.y) > radi);
		}

		//another fast collision check
		//circle collision
		bool CheckCircle(CPolygon* p1, CPolygon* p2)
		{
			float radi = p1->zRadius + p2->zRadius;
			XMVECTOR dist = p1->zPosition - p2->zPosition;
			XMVECTOR len = XMVector2Length(dist);
			return !(XMVectorGetX(len) > radi);
		}

		// checks if line segments |AB| and |CD| intersect
		bool DoLinesIntersect(XMVECTOR A, XMVECTOR B, XMVECTOR C, XMVECTOR D, XMFLOAT3* intersection)
		{
			//P is intersection point of lines (not line segments)
			XMVECTOR P = XMVector2IntersectLine(A, B, C, D);

			//store points
			XMFLOAT2 a, b, c, d, p;
			XMStoreFloat2(&p, P);
			//special cases (parallel and coincide)
			if (p.x == INFINITY)
				return true;
			else if (isnan(p.x))
				return false;
			XMStoreFloat2(&a, A);
			XMStoreFloat2(&b, B);
			XMStoreFloat2(&c, C);
			XMStoreFloat2(&d, D);			

			//is p on segments
			bool isOnAB, isOnCD;
			if (a.x == b.x)
				isOnAB = p.y >= min(a.y, b.y) && p.y <= max(a.y, b.y);
			else
				isOnAB = p.x >= min(a.x, b.x) && p.x <= max(a.x, b.x);
			if (c.x == d.x)
				isOnCD = p.y >= min(c.y, d.y) && p.y <= max(c.y, d.y);
			else
				isOnCD = p.x >= min(c.x, d.x) && p.x <= max(c.x, d.x);

			//check if P is on both line segments
			bool result = isOnAB && isOnCD;
			if (result && intersection != nullptr)
				XMStoreFloat3(intersection, P);
			return result;
		}

		bool IsColliding(CCircle* c1, CCircle* c2)
		{
			XMVECTOR v = XMVectorSubtract(c1->zPosition, c2->zPosition);
			XMVECTOR len = XMVector2Length(v);
			return XMVectorGetX(len) < (c1->Radius + c2->Radius);
		}

		bool IsColliding(CCircle* c, CRectangle* r)
		{
			return false;
		}

		bool IsColliding(CRectangle* c, CRectangle* r)
		{
			return false;
		}

		bool IsColliding(CCircle* c, CPolygon* p)
		{
			return false;
		}

		XMFLOAT2 GetProj(XMVECTOR edgeNormal, CPolygon* p)
		{
			float min = XMVectorGetX(XMVector2Dot(edgeNormal, p->zTransformedVertices[0]));
			float max = min;
			for (int i = 1; i < p->zVertexCount; i++) 
			{
				float dot = XMVectorGetX(XMVector2Dot(edgeNormal, p->zTransformedVertices[i]));
				if (dot < min)
					min = dot;
				else if (dot > max)
					max = dot;
			}
			return XMFLOAT2(min, max);
		}

		bool DoProjsOverlap(XMFLOAT2 proj1, XMFLOAT2 proj2)
		{
			if (proj1.x <= proj2.y && proj1.y >= proj2.x)
				return true;
			if (proj2.x <= proj1.y && proj2.y >= proj1.x)
				return true;
			return false;
		}

		bool IsCollidingSat(CPolygon* p1, CPolygon* p2)
		{
			if (!CheckRegion(p1, p2))
				return false;

			if (!CheckCircle(p1, p2))
				return false;

			//proj onto all edges normals of p1
			XMVECTOR edgeNormal;
			for (int i = 0; i < p1->zVertexCount; i++)
			{		
				if (i == p1->zVertexCount - 1)
					edgeNormal = p1->zTransformedVertices[0] - p1->zTransformedVertices[i];
				else
					edgeNormal = p1->zTransformedVertices[i + 1] - p1->zTransformedVertices[i];
				edgeNormal = XMVector2Orthogonal(edgeNormal);
				XMFLOAT2 proj1 = GetProj(edgeNormal, p1);
				XMFLOAT2 proj2 = GetProj(edgeNormal, p2);
				if (!DoProjsOverlap(proj1,proj2))
					return false;
			}
			//proj onto all edges normals of p2
			for (int i = 0; i < p2->zVertexCount; i++)
			{
				if (i == p2->zVertexCount - 1)
					edgeNormal = p2->zTransformedVertices[0] - p2->zTransformedVertices[i];
				else
					edgeNormal = p2->zTransformedVertices[i + 1] - p2->zTransformedVertices[i];
				edgeNormal = XMVector2Orthogonal(edgeNormal);
				XMFLOAT2 proj1 = GetProj(edgeNormal, p1);
				XMFLOAT2 proj2 = GetProj(edgeNormal, p2);
				if (!DoProjsOverlap(proj1, proj2))
					return false;
			}
			
			return true;
		}

		bool IsColliding(CPolygon* p1, CPolygon* p2, XMFLOAT3* pointOfCollision)
		{
			if (!CheckRegion(p1, p2))
				return false;

			if (!CheckCircle(p1, p2))
				return false;

			for(int i=0;i<p1->zVertexCount-1;i++)
				for (int j = 0; j < p2->zVertexCount - 1; j++)
				{
					XMVECTOR A = XMVector2Transform(p1->zVertices[i], p1->zWorld);
					XMVECTOR B = XMVector2Transform(p1->zVertices[i+1], p1->zWorld);
					XMVECTOR C = XMVector2Transform(p2->zVertices[j], p2->zWorld);
					XMVECTOR D = XMVector2Transform(p2->zVertices[j+1], p2->zWorld);
					if (DoLinesIntersect(A, B, C, D, pointOfCollision))
						return true;
				}
			return false;
		}
	}
}
