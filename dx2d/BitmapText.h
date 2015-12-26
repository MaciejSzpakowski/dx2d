#pragma once

namespace Viva
{
	class CBitmapText : public CDynamic
	{
	public:
		CBitmapFont* zFont;
		void zDraw() override;
		XMMATRIX zGetScaleMatrix() override;
		void zTextTransform(int col, int row, int len);

		CBitmapText(CBitmapFont* _font);
		//set up scale to match given size
		void SetPixelScale(int width, int height);
		void Destroy();

		TextHorAlign HorizontalAlign;
		TextVerAlign VerticalAlign;
		TEX_FILTER TexFilter; //point or linear
		wstring Text;
		float Size;
		float Height;
		float Width;
		float HorizontalSpacing;
		float VerticalSpacing;
	};
}
