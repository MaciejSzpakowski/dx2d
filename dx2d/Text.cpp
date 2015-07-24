#include "Private.h"

namespace dx2d
{
	CBitmapText::CBitmapText(CBitmapFont* _font)
	{
		Color = XMFLOAT4(1, 1, 1, 1);
		zFont = _font;
		Text = L"";
		HorizontalAlign = TextHorAlign::Left;
		VerticalAlign = TextVerAlign::Top;
		Height = 1;
		Width = 1;
		HorizontalSpacing = 0;
		VerticalSpacing = 0;
		Size = 1;
	}

	void CBitmapText::zDraw()
	{
		if (Text.length() == 0)
			return;
		//color
		Core->zContext->UpdateSubresource(zCbBufferPS, 0, NULL, &Color, 0, 0);
		Core->zContext->PSSetConstantBuffers(0, 1, &zCbBufferPS);
		//tex
		Core->zContext->PSSetShaderResources(0, 1, &zFont->zShaderResource);
		int len = (int)Text.length();
		int col = 0;
		int row = 0;
		for (int i = 0; i < len; i++)
		{
			//check for new line
			if (Text[i] == '\n')
			{
				row++;
				col = 0;
				continue;
			}			
			//uv
			int index = Text[i] - ' ';
			if (index < 0 || index > zFont->zChars.size())
				continue;
			Core->zContext->UpdateSubresource(zCbBufferUV, 0, NULL, &(zFont->zChars[index]), 0, 0);
			Core->zContext->VSSetConstantBuffers(1, 1, &zCbBufferUV);
			//transform letter and draw
			zTextTransform(col,row, len);
			Core->zContext->DrawIndexed(6, 0, 0);
			col++;
		}
	}

	void CBitmapText::zTextTransform(int col, int row, int len)
	{
		float _Height = Height * Size;
		float _Width = Width * Size;
		float _HorizontalSpacing = HorizontalSpacing * Size;
		float _VerticalSpacing = VerticalSpacing * Size;
		float horAlignOffset = 0;
		float verAlignOffset = 0;
		if (HorizontalAlign == TextHorAlign::Center)
			horAlignOffset = -len / 2.0f;
		else if (HorizontalAlign == TextHorAlign::Right)
			horAlignOffset = (float)-len;
		XMMATRIX scale = XMMatrixScaling(_Width, _Height, 1);
		XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(zRotation);
		//origin is translation matrix from the center of the text object
		XMMATRIX origin = XMMatrixTranslation(
			(col*(_Width+_HorizontalSpacing) + horAlignOffset)*2,
			(-row*(_Height+_VerticalSpacing) + verAlignOffset)*2, 0);
		XMMATRIX loc = XMMatrixTranslationFromVector(zPosition);
		XMMATRIX worldViewProj;
		if (zParent == nullptr)
			worldViewProj  = scale * origin * rot * loc * Camera->zView * Camera->zProj;
		else
		{
			XMMATRIX parentLoc = XMMatrixRotationRollPitchYawFromVector(zParent->zRotation);
			XMMATRIX parentRot = XMMatrixTranslationFromVector(zParent->zPosition);
			worldViewProj = scale * origin * rot * loc * parentLoc * parentRot *
				Camera->zView * Camera->zProj;
		}
		worldViewProj = XMMatrixTranspose(worldViewProj);
		Core->zContext->UpdateSubresource(zCbBufferVS, 0, NULL, &worldViewProj, 0, 0);
		Core->zContext->VSSetConstantBuffers(0, 1, &zCbBufferVS);
	}

	void CBitmapText::SetPixelScale(int width, int height)
	{		
		XMFLOAT2 frustum = Camera->GetFrustumSize(GetPosition().z);
		RECT client;
		GetClientRect(Core->GetWindowHandle(), &client);
		XMFLOAT2 clientSize = { (float)client.right - client.left,
			(float)client.bottom - client.top };
		XMFLOAT2 unitsPerPixel = { frustum.x / clientSize.x, frustum.y / clientSize.y };
		Width = unitsPerPixel.x * width / 4;
		Height = unitsPerPixel.y * height / 4;
		Size = 1;
	}

	XMMATRIX CBitmapText::zGetScaleMatrix()
	{
		return XMMatrixIdentity();
	}

	void CBitmapText::Destroy()
	{
		DrawManager->RemoveBitmapText(this);
		delete this;
	}

	CBitmapFont::CBitmapFont(const WCHAR* file, vector<Rect> _chars)
	{
		zChars = _chars;
		zTexture = Functions::GetCachedTextureFromFile(file);
		zShaderResource = zTexture->zShaderResource;
	}

	CBitmapFont::CBitmapFont(const WCHAR* file, int width, int height, int charsPerRow)
	{
		zTexture = Functions::GetCachedTextureFromFile(file);

		float fWidth;
		float fHeight;
		vector<Rect> chars1;
		for (int i = 0,k=32;k<127 ; i++)
			for (int j = 0; j < charsPerRow; j++, k++)
			{
				fWidth = (float)width;
				fHeight = (float)height;
				chars1.push_back(Rect(fWidth / zTexture->zWidth *j, fHeight / zTexture->zHeight *i, fWidth /
					zTexture->zWidth *(j + 1), fHeight / zTexture->zHeight*(i + 1)));
			}		
		
		zChars = chars1;
		zShaderResource = zTexture->zShaderResource;
	}

	CBitmapFont::CBitmapFont(CTexture* texture, vector<Rect> _chars)
	{
		zChars = _chars;
		zTexture = texture;
		zShaderResource = texture->zShaderResource;
	}

	void CBitmapFont::Destroy()
	{
		delete this;
	}
}