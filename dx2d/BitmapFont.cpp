#include "Viva.h"

namespace Viva
{
	BitmapFont::BitmapFont(const wchar_t* filename, const vector<RECT>& _chars)
	{
		chars = _chars;
		texture = DrawManager->GetTexture(filename);

		for (int i = 0; i < chars.size();i++)
			charsUv.push_back(Rect(_chars[i].left/ (float)texture->GetSize().width,
								   _chars[i].top / (float)texture->GetSize().height,
								   _chars[i].right / (float)texture->GetSize().width,
								   _chars[i].bottom / (float)texture->GetSize().height));
	}

	BitmapFont::BitmapFont(const wchar_t* filename, const Size& letterSize, int charsPerRow)
	{
		texture = DrawManager->GetTexture(filename);

		for (int i = 0, k = 32; k < 127; i++)
		{
			for (int j = 0; j < charsPerRow && k < 127; j++, k++)
			{
				chars.push_back({ (long)(j*letterSize.width),
					              (long)(i*letterSize.height) ,
					              (long)((j + 1)*letterSize.width),
					              (long)((i + 1)*letterSize.height)});
				charsUv.push_back(Rect(j*letterSize.width / (float)texture->GetSize().width, 
					                   i*letterSize.height / (float)texture->GetSize().height,
					                   (j + 1)*letterSize.width / (float)texture->GetSize().width,
					                   (i + 1)*letterSize.height / (float)texture->GetSize().height));
			}
		}
	}

	BitmapFont::BitmapFont(Texture* _texture, const vector<RECT>& _chars)
	{
		chars = _chars;
		texture = _texture;

		for (int i = 0; i < chars.size(); i++)
			charsUv.push_back(Rect(_chars[i].left / (float)texture->GetSize().width,
				_chars[i].top / (float)texture->GetSize().height,
				_chars[i].right / (float)texture->GetSize().width,
				_chars[i].bottom / (float)texture->GetSize().height));
	}

	BitmapFont::BitmapFont(Texture* _texture, const Size& letterSize, int charsPerRow)
	{
		texture = _texture;

		for (int i = 0, k = 32; k < 127; i++)
		{
			for (int j = 0; j < charsPerRow && k < 127; j++, k++)
			{
				chars.push_back({ (long)(j*letterSize.width),
					(long)(i*letterSize.height) ,
					(long)((j + 1)*letterSize.width),
					(long)((i + 1)*letterSize.height) });
				charsUv.push_back(Rect(j*letterSize.width / (float)texture->GetSize().width,
					i*letterSize.height / (float)texture->GetSize().height,
					(j + 1)*letterSize.width / (float)texture->GetSize().width,
					(i + 1)*letterSize.height / (float)texture->GetSize().height));
			}
		}
	}
}