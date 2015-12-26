#pragma once

namespace Viva
{
	class CRenderTarget
	{
	public:
		ID3D11Texture2D* zTexture;
		ID3D11RenderTargetView* zTargetView;
		CSprite* zSprite;
		vector<CPolygon*> zPolygons;
		vector<CSprite*> zSprites;
		vector<CBitmapText*> zTexts;

		CRenderTarget(ID3D11PixelShader* _post) :
			zTexture(nullptr),
			zTargetView(nullptr),
			zSprite(nullptr),
			PixelShader(_post) {}

		//draw all objects that belong to the render target
		void zDraw();

		//reorder render target
		void MoveToTop();

		//reorder render target
		void MoveToBottom();

		//reorder render target
		void MoveUp();

		//reorder render target
		void MoveDown();

		void Destroy();

		ID3D11PixelShader* PixelShader;
	};
}
