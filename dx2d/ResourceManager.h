#pragma once

namespace Viva
{
	class CResourceManager
	{
	public:
		std::map<wstring, CTexture*> zTextures;
		std::map<wstring, CBitmapFont*> zFonts;

		CResourceManager::CResourceManager();
		void AddTexture(CTexture* tex);
		CTexture* GetTexture(wstring name);
		void AddBitmapFont(CBitmapFont* font);
		CBitmapFont* GetBitmapFont(wstring name);
		void Clear();
		void Destroy();
	};
}
