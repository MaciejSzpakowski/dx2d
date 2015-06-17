#include "Private.h"

namespace dx2d
{
	CBitmapText::CBitmapText(CBitmapFont* _font)
	{
		Color = SColor(1, 1, 1, 1);
		font = _font;
		Text = L"";
		HorizontalAlign = TextHorAlign::Left;
		VerticalAlign = TextVerAlign::Top;
	}

	void CBitmapText::Draw()
	{
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
		float horAlignOffset = 0;
		float verAlignOffset = 0;
		if (HorizontalAlign == TextHorAlign::Center)
			horAlignOffset = -len / 2.0f;
		else if (HorizontalAlign == TextHorAlign::Right)
			horAlignOffset = -len;
		XMMATRIX scale = GetScaleMatrix();
		XMMATRIX rot = DirectX::XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
		XMMATRIX origin = DirectX::XMMatrixTranslation(col*1.1f + horAlignOffset, -row*1.1f + verAlignOffset, 0);
		XMMATRIX loc = DirectX::XMMatrixTranslation(Position.x, Position.y, Position.z);
		XMMATRIX worldViewProj = origin * rot * loc * Camera->view * Camera->proj;
		if (Parent != nullptr)
		{
			XMMATRIX parentLoc = DirectX::XMMatrixRotationRollPitchYaw(0, 0, Parent->Rotation.z);
			XMMATRIX parentRot = DirectX::XMMatrixTranslation(Parent->Position.x, Parent->Position.y, Parent->Position.z);
			//worldViewProj = world * parentLoc * parentRot * Camera->view * Camera->proj;
		}
		worldViewProj = DirectX::XMMatrixTranspose(worldViewProj);
		GetContext()->UpdateSubresource(cbBufferVS, 0, NULL, &worldViewProj, 0, 0);
		GetContext()->VSSetConstantBuffers(0, 1, &cbBufferVS);
	}

	XMMATRIX CBitmapText::GetScaleMatrix()
	{
		return DirectX::XMMatrixIdentity();
	}

	void CBitmapText::Destroy()
	{
		DrawManager->RemoveBitmapText(this);
		delete this;
	}

	CBitmapFont::CBitmapFont(const WCHAR* file, vector<UV> _chars)
	{
		fileName = file;
		chars = _chars;
		Functions::LoadCachedTexture(file, shaderResource);
	}

	std::wstring CBitmapFont::GetFileName()
	{
		return fileName;
	}

	void CBitmapFont::Destroy()
	{
		delete this;
	}
}