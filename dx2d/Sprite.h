#pragma once

namespace Viva
{
	class CSprite : public CDynamic
	{
	public:
		CTexture* zTexture;
		ID3D11ShaderResourceView* zShaderResource;
		XMMATRIX _GetScaleMatrix() override;
		virtual void _Play() {}
		void _Draw() override;
		void zCheckForCursor(XMMATRIX transform) override;
		void zSpriteUpdate();

		CSprite();
		CSprite(CTexture* texture);
		CSprite(LPCWSTR file);

		CTexture* GetTexture() { return zTexture; }

		//sets up scale to match texture size
		void SetNaturalScale();
		//set up scale to match given size
		void SetPixelScale(const Viva::Size& _size);
		void Destroy() override;

		TextureFilter TexFilter; //point or linear
		XMFLOAT2 Scale;
		bool FlipHorizontally;
		bool FlipVertically;
		ID3D11PixelShader* PixelShader;
	};
}
