#pragma once

namespace Viva
{
	class BitmapFont
	{
	private:
		vector<Rect> chars; // uv cooridantes for letters 
		CTexture* texture;
	public:
		// Create bitmap font from file.
		// _chars: uv coordinates of each letter in ascii order
		BitmapFont(const wchar_t* filename, const vector<Rect>& _chars);

		// Create bitmap font from file.
		// UV cooridnates will be generated starting from 0,0 (upper left corner).
		// letterSize: width and height of each letter in pixels
		// charsPerRow: how many characters there are per row
		BitmapFont(const wchar_t* filename, const Size& letterSize, int charsPerRow);

		// Create bitmap font from texture.
		// _chars: uv coordinates of each letter in ascii order
		BitmapFont(CTexture* _texture, const vector<Rect>& _chars);

		// Create bitmap font from texture.
		// UV cooridnates will be generated starting from 0,0 (upper left corner).
		// letterSize: width and height of each letter in pixels
		// charsPerRow: how many characters there are per row
		BitmapFont(CTexture* _texture, const Size& letterSize, int charsPerRow);

		CTexture* GetTexture() const { return texture; }

		const vector<Rect>& _GetChars() const { return chars; }

		void Destroy()
		{
			delete this;
		}
	};
}
