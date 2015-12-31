#pragma once

namespace Viva
{
	class CDrawManager
	{
	private:
		RenderTarget* defaultRenderTarget;
		vector<RenderTarget*> renderTargets;
		BitmapFont* defaultFont;
		ID3D11SamplerState* pointSampler;
		ID3D11SamplerState* lineSampler;
		ID3D11Buffer* indexBufferSprite;
		ID3D11Buffer* vertexBufferSprite;
		ID3D11RasterizerState* wireframe;
		ID3D11RasterizerState* solid;
		XMMATRIX renderTargetMatrix;
		ID3D11Buffer* constantBufferVS;
		ID3D11Buffer* constantBufferPS;
		ID3D11Buffer* constantBufferUV;
		ID3D11Buffer* constantBufferPSExtra;
		TextureFilter defaultTextureFilter;

		void _RenderTargetTransform(int i);
	public:
		void _DrawAll();
		void _InitDefaultFont();
		ID3D11Buffer* _GetConstantBufferVS() { return constantBufferVS; }
		ID3D11Buffer* _GetConstantBufferPS() const { return constantBufferPS; }
		ID3D11Buffer* _GetConstantBufferUV() const { return constantBufferUV; }
		ID3D11Buffer* _GetConstantBufferPSExtra() const { return constantBufferPSExtra; }
		void _SetRenderTargetMatrix(XMMATRIX&& mat) { renderTargetMatrix = mat; }
		ID3D11RasterizerState* _GetWireframe() const { return wireframe; }
		ID3D11RasterizerState* _GetSolid() const { return solid; }
		ID3D11Buffer*const* _GetVertexBufferSpriteAddress() const { return &vertexBufferSprite; }
		ID3D11SamplerState*const* _GetPointSamplerAddress() const { return &pointSampler; }
		ID3D11SamplerState*const* _GetLineSamplerAddress() const { return &lineSampler; }

		CDrawManager();
		void Destroy();

		TextureFilter GetDefaultTextureFilter() const { return defaultTextureFilter; }
		void SetDefaultTextureFilter(TextureFilter filter) { defaultTextureFilter = filter; }

		// Create a new poly from list of points and add it to draw manager.
		Polygon* AddPoly(const vector<XMFLOAT2>& points, RenderTarget* target = nullptr);
		
		// Create a new rectangle and add it to draw manager.
		Rectangle* AddRect(const Size& size, RenderTarget* target = nullptr);
		
		// Create a new circle and add it to draw manager.
		Circle* AddCircle(float radius, unsigned char resolution, RenderTarget* target = nullptr);
		
		// Add existing poly to draw manager.
		void AddPoly(Polygon* p, RenderTarget* target = nullptr);

		// Remove poly from draw manager.
		void RemovePoly(Polygon* p);

		// Create sprite from file and add it to draw manager.
		// Supported file formats: BMP, GIF, JPEG, PNG, TIFF, Exif, WMF, EMF.
		Sprite* AddSprite(const wchar_t* filename, RenderTarget* target = nullptr);

		// Add existing sprite to draw manager.
		void AddSprite(Sprite* s, RenderTarget* target = nullptr);

		// Remove sprite from draw manager.
		void RemoveSprite(Sprite* s);
		
		// Create animation from file and add it to draw manager.
		// Supported file formats: BMP, GIF, JPEG, PNG, TIFF, Exif, WMF, EMF.
		Animation* AddAnimation(LPCWSTR file, int x, int y, RenderTarget* target = nullptr);

		// Add existing animation to draw manager.
		void AddAnimation(Animation* a, RenderTarget* target = nullptr);
		
		// Remove animation from draw manager.
		void RemoveAnimation(Animation* a);

		// Create bitmap text and add it to draw manager.
		BitmapText* AddBitmapText(BitmapFont* font, RenderTarget* target = nullptr);

		// Add existing bitmap text to draw manager.
		void AddBitmapText(BitmapText* text, RenderTarget* target = nullptr);
		
		// Remove bitmap text from draw manager.
		void RemoveBitmapText(BitmapText* text);

		BitmapFont* GetDefaultFont() const { return defaultFont; }

		// Create and add render target to draw manager.
		RenderTarget* AddRenderTarget();

		// Remove render target from draw manager.
		void RemoveRenderTarget(RenderTarget* target);
		
		RenderTarget* GetDefaultRenderTarget() { return defaultRenderTarget; }

		void MoveRenderTargetToBottom(RenderTarget* t);
		void MoveRenderTargetToTop(RenderTarget* t);
		void MoveRenderTargetUp(RenderTarget* t);
		void MoveRenderTargetDown(RenderTarget* t);
		
		// Try to get texture from the resource manager.
		// If it's not there create a new texture from file 
		// and add it to the resource manager.
		Texture* GetTexture(const wchar_t* filename) const;

		//create extra buffer for PS
		//size in bytes must be multiple of 16
		void CreateExtraBuffer(UINT size);

		//destroys all but default render target
		void ClearRenderTargets();

	};
}
