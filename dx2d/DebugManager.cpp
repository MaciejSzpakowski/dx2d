#include "Viva.h"

namespace Viva
{
	CDebugManager::CDebugManager(BitmapFont* font, Camera* camera)
	{
		debugText = new BitmapText(font);
		debugText->SetParent(camera);
		debugText->SetPixelPerfectScale();
		debugText->SetPosition(-18, 13, 19);
	}

	void CDebugManager::_Flush()
	{
		if (debugText == nullptr)
			return;
		debugText->SetText(wss.str());
		wss.str(L"");
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