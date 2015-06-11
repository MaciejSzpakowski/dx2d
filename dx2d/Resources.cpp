#include "Private.h"

namespace dx2d
{
	void CResourceManager::AddTexture(CTexture* tex)
	{
		textures[tex->fileName] = tex;
	}

	CTexture* CResourceManager::GetTexture(std::wstring file)
	{
		auto it = textures.find(file);
		if (it != textures.end())
			return it->second;
		else
			return nullptr;
	}

	void CResourceManager::Clear()
	{
		for (auto const &it : textures)
			delete it.second;
		textures.clear();
	}

	void CResourceManager::Destroy()
	{
		Clear();
		delete this;
	}
}