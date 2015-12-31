#pragma once

namespace Viva
{
	class Sprite : public Dynamic
	{
	private:
		Texture* texture;
		TextureFilter texFilter;
		XMFLOAT2 scale;
		bool flipHorizontally;
		bool flipVertically;
		ID3D11PixelShader* pixelShader;

	public:
		Sprite(Texture* _texture);
		Sprite(Texture* _texture, TextureFilter _texFilter);
		Sprite(const wchar_t* filename);

		XMMATRIX _GetScaleMatrix() override;
		virtual void _Play() {}
		void _Draw() override;
		void _CheckForCursor(XMMATRIX transform) override;
		void _SpriteUpdate();

		Texture* GetTexture() { return texture; }
		TextureFilter GetTextureFilter() const { return texFilter; }
		void SetTextureFilter(TextureFilter _texFilter) { texFilter = _texFilter; }
		XMFLOAT2 GetScale() const { return scale; }
		void SetScale(const XMFLOAT2& _scale) { scale = _scale; }
		void SetScaleX(float x) { scale.x = x; }
		void SetScaleY(float y) { scale.y = y; }
		bool IsFlippedHorizontally() const { return flipHorizontally; }
		void SetFlipHorizontally(bool _flipHorizontally) { flipHorizontally = _flipHorizontally; }
		bool IsFlippedVertically() const { return flipVertically; }
		void SetFlipVertically(bool _flipVertically) { flipVertically = _flipVertically; }
		ID3D11PixelShader* GetPixelShader() const { return pixelShader; }
		void SetPixelShader(ID3D11PixelShader* _pixelShader) { pixelShader = _pixelShader; }

		// Scale sprite so its size in pixels on screen is exactly the same as texture size.
		void SetPixelPerfectScale();

		// Scale to match given size in pixels.
		void SetPixelScale(const Viva::Size& _size);

		void Destroy() override;

	};
}
