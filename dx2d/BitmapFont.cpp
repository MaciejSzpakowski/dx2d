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
		float texWidth = (float)texture->GetSize().width;
		float texHeight = (float)texture->GetSize().height;

		for (int i = 0, k = 32; k < 127; i++)
		{
			for (int j = 0; j < charsPerRow; j++, k++)
			{
				fWidth = (float)letterSize.width;
				fHeight = (float)letterSize.height;
				chars.push_back(Rect(fWidth / texWidth *j, fHeight / texHeight *i, fWidth /
					texWidth *(j + 1), fHeight / texHeight *(i + 1)));
			}
		}
	}

	BitmapFont::BitmapFont(Texture* _texture, const vector<Rect>& _chars)
	{
		chars = _chars;
		texture = _texture;
	}

	BitmapFont::BitmapFont(Texture* _texture, const Size& letterSize, int charsPerRow)
	{
		texture = _texture;

		float fWidth;
		float fHeight;
		float texWidth = (float)texture->GetSize().width;
		float texHeight = (float)texture->GetSize().height;

		for (int i = 0, k = 32; k < 127; i++)
		{
			for (int j = 0; j < charsPerRow; j++, k++)
			{
				fWidth = (float)letterSize.width;
				fHeight = (float)letterSize.height;
				chars.push_back(Rect(fWidth / texWidth *j, fHeight / texHeight *i, fWidth /
					texWidth *(j + 1), fHeight / texHeight *(i + 1)));
			}
		}
	}
}