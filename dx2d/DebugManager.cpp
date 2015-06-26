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
		zDebugText = DrawManager->AddBitmapText(font);
		zDebugText->SetPosition(-9, 7, 10.3f);
		zDebugText->Parent = Camera;
		zDebugText->SetPixelScale(15, 21);
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