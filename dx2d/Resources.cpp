#include "Private.h"

namespace dx2d
{
	CResourceManager::CResourceManager()
	{
	}

	void CResourceManager::AddTexture(CTexture* tex)
	{
		textures[tex->name] = tex;
	}

	CTexture* CResourceManager::GetTexture(wstring file)
	{
		auto it = textures.find(file);
		if (it != textures.end())
			return it->second;
		else
			return nullptr;
	}

	void CResourceManager::AddBitmapFont(CBitmapFont* font)
	{
		fonts[font->GetName()] = font;
	}

	CBitmapFont* CResourceManager::GetBitmapFont(wstring file)
	{
		auto it = fonts.find(file);
		if (it != fonts.end())
			return it->second;
		else
			return nullptr;
	}

	void CResourceManager::Clear()
	{
		for (auto const &it : textures)
			delete it.second;
		textures.clear();
		for (auto const &it : fonts)
			delete it.second;
		fonts.clear();
	}

	void CResourceManager::Destroy()
	{
		Clear();
		delete this;
	}
}