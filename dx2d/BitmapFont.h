#pragma once

namespace Viva
{
	class CBitmapFont
	{
	public:
		vector<Rect> zChars;
		CTexture* zTexture;
		ID3D11ShaderResourceView* zShaderResource;

		CBitmapFont(LPCWSTR file, vector<Rect> _chars);
		CBitmapFont(LPCWSTR file, int width, int height, int charsPerRow);
		CBitmapFont(CTexture* texture, vector<Rect> _chars);
		CBitmapFont(CTexture* texture, int width, int height, int charsPerRow);
		CTexture* GetTexture() { return zTexture; }
		void Destroy();
	};
}
