#include "Private.h"

namespace dx2d
{
	CDebugManager::CDebugManager()
	{
		zDebugText = nullptr;		
	}

	void CDebugManager::Flush()
	{
		if (zDebugText == nullptr)
			return;
		zDebugText->Text = zWss.str();
		zWss.str(L"");
	}

	void CDebugManager::Init(CBitmapFont* font)
	{
		zDebugText = new CBitmapText(DrawManager->zDefaultFont);
		zDebugText->SetPosition(-9, 7, 10.3f);
		zDebugText->zParent = Camera;
		zDebugText->SetPixelScale(10, 14);
	}

	void CDebugManager::Debug(int debug, wstring name)
	{
		zWss << name << ": " << debug << "\n";
	}

	void CDebugManager::Debug(float debug, wstring name)
	{
		zWss << name << ": " << debug << "\n";
	}

	void CDebugManager::Debug(double debug, wstring name)
	{
		zWss << name << ": " << debug << "\n";
	}

	void CDebugManager::Debug(wstring debug, wstring name)
	{
		zWss << name << ": " << debug << "\n";
	}
	
	void CDebugManager::Destroy()
	{
		if (zDebugText != nullptr)
			zDebugText->Destroy();
		delete this;
	}
}