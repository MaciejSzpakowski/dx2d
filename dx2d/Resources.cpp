#include "Private.h"

namespace dx2d
{
	CResourceManager::CResourceManager()
	{
	}

	void CResourceManager::AddTexture(CTexture* tex)
	{
		zTextures[tex->zName] = tex;
	}

	CTexture* CResourceManager::GetTexture(wstring file)
	{
		auto it = zTextures.find(file);
		if (it != zTextures.end())
			return it->second;
		else
			return nullptr;
	}

	void CResourceManager::AddBitmapFont(CBitmapFont* font)
	{
		zFonts[font->GetName()] = font;
	}

	CBitmapFont* CResourceManager::GetBitmapFont(wstring file)
	{
		auto it = zFonts.find(file);
		if (it != zFonts.end())
			return it->second;
		else
			return nullptr;
	}

	void CResourceManager::Clear()
	{
		for (auto const &it : zTextures)
			delete it.second;
		zTextures.clear();
		for (auto const &it : zFonts)
			delete it.second;
		zFonts.clear();
	}

	void CResourceManager::Destroy()
	{
		Clear();
		delete this;
	}
}