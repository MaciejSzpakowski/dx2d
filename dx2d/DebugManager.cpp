#include "Private.h"

namespace dx2d
{
	CDebugManager::CDebugManager()
	{
		debugText = nullptr;		
	}

	void CDebugManager::Flush()
	{
		if (debugText == nullptr)
			return;
		debugText->Text = wss.str();
		wss.str(L"");
	}

	void CDebugManager::Init(CBitmapFont* font)
	{
		debugText = DrawManager->AddBitmapText(font);
		debugText->SetPosition(-9, 7, 10.3f);
		debugText->Parent = Camera;
		debugText->SetPixelScale(15, 21);
	}

	void CDebugManager::Debug(int debug, wstring name)
	{
		wss << name << ": " << debug << "\n";
	}

	void CDebugManager::Debug(float debug, wstring name)
	{
		wss << name << ": " << debug << "\n";
	}

	void CDebugManager::Debug(double debug, wstring name)
	{
		wss << name << ": " << debug << "\n";
	}

	void CDebugManager::Debug(wstring debug, wstring name)
	{
		wss << name << ": " << debug << "\n";
	}
	
	void CDebugManager::Destroy()
	{
		if (debugText != nullptr)
			debugText->Destroy();
		delete this;
	}
}