#include "Viva.h"

namespace Viva
{
	CBitmapFont::CBitmapFont(LPCWSTR file, vector<Rect> _chars)
	{
		zChars = _chars;
		zTexture = DrawManager->GetTexture(file);
		zShaderResource = zTexture->zShaderResource;
	}

	CBitmapFont::CBitmapFont(LPCWSTR file, int width, int height, int charsPerRow)
	{
		zTexture = DrawManager->GetTexture(file);
		zShaderResource = zTexture->zShaderResource;

		float fWidth;
		float fHeight;
		vector<Rect> chars1;
		for (int i = 0, k = 32; k < 127; i++)
		{
			for (int j = 0; j < charsPerRow; j++, k++)
			{
				fWidth = (float)width;
				fHeight = (float)height;
				chars1.push_back(Rect(fWidth / zTexture->size.width *j, fHeight / zTexture->size.height *i, fWidth /
					zTexture->size.width *(j + 1), fHeight / zTexture->size.height *(i + 1)));
			}
		}

		zChars = chars1;
	}

	CBitmapFont::CBitmapFont(CTexture* texture, vector<Rect> _chars)
	{
		zChars = _chars;
		zTexture = texture;
		zShaderResource = texture->zShaderResource;
	}

	CBitmapFont::CBitmapFont(CTexture* texture, int width, int height, int charsPerRow)
	{
		zTexture = texture;
		zShaderResource = zTexture->zShaderResource;

		float fWidth;
		float fHeight;
		vector<Rect> chars1;
		for (int i = 0, k = 32; k < 127; i++)
		{
			for (int j = 0; j < charsPerRow; j++, k++)
			{
				fWidth = (float)width;
				fHeight = (float)height;
				chars1.push_back(Rect(fWidth / zTexture->size.width *j, fHeight / zTexture->size.height *i, fWidth /
					zTexture->size.width *(j + 1), fHeight / zTexture->size.height *(i + 1)));
			}
		}

		zChars = chars1;
	}

	void CBitmapFont::Destroy()
	{
		delete this;
	}
}