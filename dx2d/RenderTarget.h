#pragma once

namespace Viva
{
	class RenderTarget
	{
	private:
		void* extraBufferPSdata;
		ID3D11Texture2D* texture;
		ID3D11RenderTargetView* rtv;
		Sprite* sprite;
		vector<Polygon*> polygons;
		vector<Sprite*> sprites;
		vector<BitmapText*> texts;
		ID3D11PixelShader* pixelShader;

	public:
		RenderTarget();

		//draw all objects that belong to the render target
		void _DrawObjects(const XMMATRIX* view, const XMMATRIX* proj);

		// Draw target itself.
		void _DrawTarget() { sprite->_Draw(); }

		//reorder render target
		void MoveToTop();

		//reorder render target
		void MoveToBottom();

		//reorder render target
		void MoveUp();

		//reorder render target
		void MoveDown();

		void SetExtraBufferPS(void* data) { extraBufferPSdata = data; }
		void* GetExtraBufferPS() const { return extraBufferPSdata; }

		ID3D11PixelShader* GetPixelShader() const { return pixelShader; }
		void SetPixelShader(ID3D11PixelShader* ps) { pixelShader = ps; }

		void AddPoly(Polygon* p)
		{
			polygons.push_back(p);
			p->_SetIndex((int)polygons.size() - 1);
			p->_SetRenderTarget(this);
		}

		void AddSprite(Sprite* s)
		{ 
			sprites.push_back(s); 
			s->_SetIndex((int)sprites.size() - 1);
			s->_SetRenderTarget(this);
		}

		void AddText(BitmapText* t)
		{ 
			texts.push_back(t); 
			t->_SetIndex((int)texts.size() - 1);
			t->_SetRenderTarget(this);
		}

		void RemovePoly(Polygon* p);
		void RemoveText(BitmapText* t);
		void RemoveSprite(Sprite* s);

		void Destroy();

	};
}
