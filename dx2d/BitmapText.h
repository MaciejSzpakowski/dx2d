#pragma once

namespace Viva
{
	class BitmapText : public Dynamic
	{
	private:
		BitmapFont* font;
		vector<XMMATRIX> charScaleMat;
		vector<XMFLOAT2> charScale;
		TextureFilter texFilter;
		wstring text;
		FontMetrics metrics;

		void _TextTransform(float x, int row, float lineLen, int index);
		XMMATRIX _GetScaleMatrix() override { return DirectX::XMMatrixIdentity(); }
		void _UpdateScales();
		float _CurrentLineLength(int off);

	public:
		BitmapText(BitmapFont* _font);

		void Destroy() override;

		FontMetrics GetMetrics() const { return metrics; }
		void SetMetrics(const FontMetrics& _metrics) { metrics = _metrics; _UpdateScales(); }
		TextureFilter GetTexFilter() const { return texFilter; }
		void SetTexFilter(TextureFilter _texFilter) { texFilter = _texFilter; }
		wstring GetText() const { return text; }
		void SetText(const wstring& _text) { text = _text; }
		void SetText(wstring&& _text) { text = _text; }
		BitmapFont* GetFont() const { return font; }
		void SetFont(BitmapFont* _font) { font = _font; }
		void SetSize(float s) override { size = s; _UpdateScales(); }

		void _Draw() override;
	};
}
