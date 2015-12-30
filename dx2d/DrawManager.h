#pragma once

namespace Viva
{
	class CDrawManager
	{
	public:
		CRenderTarget* zDefaultRenderTarget;
		vector<CRenderTarget*> zRenderTargets;
		BitmapFont* zDefaultFont;
		ID3D11ShaderResourceView* zWhiteRes;
		ID3D11SamplerState* zPointSampler;
		ID3D11SamplerState* zLineSampler;
		ID3D11Buffer* zIndexBufferSprite;
		ID3D11Buffer* zVertexBufferSprite;
		ID3D11RasterizerState* zWireframe;
		ID3D11RasterizerState* zSolid;
		XMMATRIX zRenderTargetMatrix;
		ID3D11Buffer* zCbBufferVS;
		ID3D11Buffer* zCbBufferPS;
		ID3D11Buffer* zCbBufferUV;
		ID3D11Buffer* zCbBufferPSExtra;
		void zDrawAll();
		bool zHasObject(CDynamic* d);
		void zRenderTargetTransform(int i);
		void zInit();

		CDrawManager();
		void Destroy();
		Polygon* AddPoly(const vector<XMFLOAT2>& points, CRenderTarget* target = nullptr);
		Rectangle* AddRect(const Size& size, CRenderTarget* target = nullptr);
		Circle* AddCircle(float radius, unsigned char resolution, CRenderTarget* target = nullptr);
		void AddPoly(Polygon* p, CRenderTarget* target = nullptr);
		void RemovePoly(Polygon* p);

		//add sprite from file
		//supported file formats: BMP, GIF, JPEG, PNG, TIFF, Exif, WMF, EMF
		CSprite* AddSprite(LPCWSTR file,
			CRenderTarget* target = nullptr);

		void AddSprite(CSprite* s, CRenderTarget* target = nullptr);
		void RemoveSprite(CSprite* s);
		Animation* AddAnimation(LPCWSTR file, int x, int y, CRenderTarget* target = nullptr);
		void AddAnimation(Animation* a, CRenderTarget* target = nullptr);
		void RemoveAnimation(Animation* a);
		BitmapText* AddBitmapText(BitmapFont* font, CRenderTarget* target = nullptr);
		void AddBitmapText(BitmapText* text, CRenderTarget* target = nullptr);
		void RemoveBitmapText(BitmapText* text);
		BitmapFont* GetDefaultFont();
		CRenderTarget* AddRenderTarget();
		void RemoveRenderTarget(CRenderTarget* target);
		void InitDefaultFont();
		CRenderTarget* GetDefaultRenderTarget() { return zDefaultRenderTarget; }
		
		// try to get texture from the resource manager, if it's not there
		// create a new texture from file and add it to the resource manager
		CTexture* GetTexture(const wchar_t* filename) const;

		//create extra buffer for PS
		//size in bytes must be multiple of 16
		void CreateExtraBuffer(UINT size);

		//destroys all but default render target
		void ClearRenderTargets();

		TextureFilter TexFilterCreationMode;
		bool SupressDuplicateWarning;
	};
}
