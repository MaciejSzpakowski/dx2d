#pragma once

namespace Viva
{
	class BitmapText : public Dynamic
	{
	private:
		BitmapFont* font;
		TextureFilter texFilter;
		wstring text;
		FontMetrics metrics;

	public:
		BitmapText(BitmapFont* _font);

		// How big in pixels each letter should be.
		void SetPixelScale(const Size& _size);

		void Destroy() override;

		FontMetrics GetMetrics() const { return metrics; }
		void SetMetrics(const FontMetrics& _metrics) { metrics = _metrics; }
		TextureFilter GetTexFilter() const { return texFilter; }
		void SetTexFilter(TextureFilter _texFilter) { texFilter = _texFilter; }
		wstring GetText() const { return text; }
		void SetText(const wstring& _text) { text = _text; }
		void SetText(wstring&& _text) { text = _text; }
		BitmapFont* GetFont() const { return font; }
		void SetFont(BitmapFont* _font) { font = _font; }

		void _Draw() override;
		XMMATRIX _GetScaleMatrix() override { return DirectX::XMMatrixIdentity(); }
		void _TextTransform(int col, int row, int len);
	};
}
