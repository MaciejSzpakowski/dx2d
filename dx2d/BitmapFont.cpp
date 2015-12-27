#include "Viva.h"

namespace Viva
{
	BitmapFont::BitmapFont(const wchar_t* filename, const vector<Rect>& _chars)
	{
		chars = _chars;
		texture = DrawManager->GetTexture(filename);
	}

	BitmapFont::BitmapFont(const wchar_t* filename, const Size& letterSize, int charsPerRow)
	{
		texture = DrawManager->GetTexture(filename);

		float fWidth;
		float fHeight;

		for (int i = 0, k = 32; k < 127; i++)
		{
			for (int j = 0; j < charsPerRow; j++, k++)
			{
				fWidth = (float)letterSize.width;
				fHeight = (float)letterSize.height;
				chars.push_back(Rect(fWidth / texture->size.width *j, fHeight / texture->size.height *i, fWidth /
					texture->size.width *(j + 1), fHeight / texture->size.height *(i + 1)));
			}
		}
	}

	BitmapFont::BitmapFont(CTexture* texture, const vector<Rect>& _chars)
	{
		chars = _chars;
		texture = texture;
	}

	BitmapFont::BitmapFont(CTexture* texture, const Size& letterSize, int charsPerRow)
	{
		texture = texture;

		float fWidth;
		float fHeight;

		for (int i = 0, k = 32; k < 127; i++)
		{
			for (int j = 0; j < charsPerRow; j++, k++)
			{
				fWidth = (float)letterSize.width;
				fHeight = (float)letterSize.height;
				chars.push_back(Rect(fWidth / texture->size.width *j, fHeight / texture->size.height *i, fWidth /
					texture->size.width *(j + 1), fHeight / texture->size.height *(i + 1)));
			}
		}
	}
}