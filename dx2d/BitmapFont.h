#pragma once

namespace Viva
{
	class BitmapFont
	{
	private:
		vector<RECT> chars; // pixel cooridantes for glyphs
		vector<Rect> charsUv; // uv coordinates for glyphs
		Texture* texture;
	public:
		// Create bitmap font from file.
		// _chars: pixel coordinates of each letter in ascii order
		BitmapFont(const wchar_t* filename, const vector<RECT>& _chars);

		// Create bitmap font from file.
		// UV cooridnates will be generated starting from 0,0 (upper left corner).
		// letterSize: width and height of each letter in pixels
		// charsPerRow: how many characters there are per row
		BitmapFont(const wchar_t* filename, const Size& letterSize, int charsPerRow);

		// Create bitmap font from texture.
		// _chars: uv coordinates of each letter in ascii order
		BitmapFont(Texture* _texture, const vector<RECT>& _chars);

		// Create bitmap font from texture.
		// UV cooridnates will be generated starting from 0,0 (upper left corner).
		// letterSize: width and height of each letter in pixels
		// charsPerRow: how many characters there are per row
		BitmapFont(Texture* _texture, const Size& letterSize, int charsPerRow);

		Texture* GetTexture() const { return texture; }

		const vector<RECT>& _GetChars() const { return chars; }
		const vector<Rect>& _GetCharsUv() const { return charsUv; }

		void Destroy()
		{
			delete this;
		}
	};
}
