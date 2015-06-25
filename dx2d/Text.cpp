#include "Private.h"

namespace dx2d
{
	CBitmapText::CBitmapText(CBitmapFont* _font)
	{
		Color = XMFLOAT4(1, 1, 1, 1);
		font = _font;
		Text = L"";
		HorizontalAlign = TextHorAlign::Left;
		VerticalAlign = TextVerAlign::Top;
		Height = 1;
		Width = 1;
		HorizontalSpacing = 0;
		VerticalSpacing = 0;
		Size = 1;
	}

	void CBitmapText::Draw()
	{
		if (Text.length() == 0)
			return;
		//color
		GetContext()->UpdateSubresource(cbBufferPS, 0, NULL, &Color, 0, 0);
		GetContext()->PSSetConstantBuffers(0, 1, &cbBufferPS);
		//tex
		GetContext()->PSSetShaderResources(0, 1, &font->shaderResource);
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
			if (index < 0 || index > font->chars.size())
				continue;
			GetContext()->UpdateSubresource(cbBufferUV, 0, NULL, &(font->chars[index]), 0, 0);
			GetContext()->VSSetConstantBuffers(1, 1, &cbBufferUV);
			//transform letter and draw
			TextTransform(col,row, len);
			GetContext()->DrawIndexed(6, 0, 0);
			col++;
		}
	}

	void CBitmapText::TextTransform(int col, int row, int len)
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
		XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(Rotation);
		//origin is translation matrix from the center of the text object
		XMMATRIX origin = XMMatrixTranslation(
			(col*(_Width+_HorizontalSpacing) + horAlignOffset)*2,
			(-row*(_Height+_VerticalSpacing) + verAlignOffset)*2, 0);
		XMMATRIX loc = XMMatrixTranslationFromVector(Position);
		XMMATRIX worldViewProj;
		if (Parent == nullptr)
			worldViewProj  = scale * origin * rot * loc * Camera->view * Camera->proj;
		else
		{
			XMMATRIX parentLoc = XMMatrixRotationRollPitchYawFromVector(Parent->Rotation);
			XMMATRIX parentRot = XMMatrixTranslationFromVector(Parent->Position);
			worldViewProj = scale * origin * rot * loc * parentLoc * parentRot * Camera->view * Camera->proj;
		}
		worldViewProj = XMMatrixTranspose(worldViewProj);
		GetContext()->UpdateSubresource(cbBufferVS, 0, NULL, &worldViewProj, 0, 0);
		GetContext()->VSSetConstantBuffers(0, 1, &cbBufferVS);
	}

	void CBitmapText::SetPixelScale(int width, int height)
	{		
		POINTF frustum = Camera->GetFrustumSize(GetPosition().z);
		RECT client;
		GetClientRect(Core->GetWindowHandle(), &client);
		POINTF clientSize = { (float)client.right - client.left, (float)client.bottom - client.top };
		POINTF unitsPerPixel = { frustum.x / clientSize.x, frustum.y / clientSize.y };
		Width = unitsPerPixel.x * width / 4;
		Height = unitsPerPixel.y * height / 4;
		Size = 1;
	}

	XMMATRIX CBitmapText::GetScaleMatrix()
	{
		return XMMatrixIdentity();
	}

	void CBitmapText::Destroy()
	{
		DrawManager->RemoveBitmapText(this);
		delete this;
	}

	CBitmapFont::CBitmapFont(const WCHAR* file, vector<UV> _chars)
	{
		resName = file;
		chars = _chars;
		Functions::LoadCachedTextureFromFile(file, shaderResource);
	}

	CBitmapFont::CBitmapFont(CTexture* texture, vector<UV> _chars)
	{
		resName = texture->name;
		chars = _chars;
		shaderResource = texture->shaderResource;
	}

	wstring CBitmapFont::GetName()
	{
		return resName;
	}

	void CBitmapFont::Destroy()
	{
		delete this;
	}
}