#pragma once

namespace Viva
{
	class CDrawManager
	{
	public:
		CRenderTarget* zDefaultRenderTarget;
		vector<CRenderTarget*> zRenderTargets;
		CBitmapFont* zDefaultFont;
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
		CPolygon* AddPoly(XMFLOAT2 points[], int n,
			CRenderTarget* target = nullptr);
		CRectangle* AddRect(float sizex, float sizey,
			CRenderTarget* target = nullptr);
		CCircle* AddCircle(float radius, unsigned char resolution,
			CRenderTarget* target = nullptr);
		void AddPoly(CPolygon* p,
			CRenderTarget* target = nullptr);
		void RemovePoly(CPolygon* p);

		//add sprite from file
		//supported file formats: BMP, GIF, JPEG, PNG, TIFF, Exif, WMF, EMF
		CSprite* AddSprite(LPCWSTR file,
			CRenderTarget* target = nullptr);

		void AddSprite(CSprite* s,
			CRenderTarget* target = nullptr);
		void RemoveSprite(CSprite* s);
		CAnimation* AddAnimation(LPCWSTR file, int x, int y,
			CRenderTarget* target = nullptr);
		void AddAnimation(CAnimation* a,
			CRenderTarget* target = nullptr);
		void RemoveAnimation(CAnimation* a);
		CBitmapFont* AddBitmapFont(LPCWSTR file, vector<Rect> chars);
		void AddBitmapFont(CBitmapFont* font);
		CBitmapFont* AddBitmapFont(LPCWSTR file, int width, int height, int charsPerRow);
		CBitmapText* AddBitmapText(CBitmapFont* font,
			CRenderTarget* target = nullptr);
		void AddBitmapText(CBitmapText* text,
			CRenderTarget* target = nullptr);
		void RemoveBitmapFont(CBitmapFont* font);
		void RemoveBitmapText(CBitmapText* text);
		CBitmapFont* GetDefaultFont();
		CRenderTarget* AddRenderTarget();
		void DestroyRenderTarget(CRenderTarget* target);
		void InitDefaultFont();
		CRenderTarget* GetDefaultRenderTarget();

		//create extra buffer for PS
		//size in bytes must be multiple of 16
		void CreateExtraBuffer(UINT size);

		//destroys all but default render target
		void ClearRenderTargets();

		TEX_FILTER TexFilterCreationMode;
		bool SupressDuplicateWarning;
	};
}
