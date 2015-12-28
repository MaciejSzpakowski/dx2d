#pragma once

namespace Viva
{
	class CBitmapText : public CDynamic
	{
	public:
		BitmapFont* zFont;
		void zDraw() override;
		XMMATRIX zGetScaleMatrix() override;
		void zTextTransform(int col, int row, int len);

		CBitmapText(BitmapFont* _font);
		//set up scale to match given size
		void SetPixelScale(int width, int height);
		void Destroy() override;

		HorizontalAlignment HorizontalAlign;
		VerticalAlignment VerticalAlign;
		TextureFilter TexFilter; //point or linear
		wstring Text;
		float Height;
		float Width;
		float HorizontalSpacing;
		float VerticalSpacing;
	};
}
