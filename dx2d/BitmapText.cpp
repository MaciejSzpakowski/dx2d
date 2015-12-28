#include "Viva.h"

namespace Viva
{
	CBitmapText::CBitmapText(BitmapFont* _font)
	{
		color = XMFLOAT4(1, 1, 1, 1);
		zFont = _font;
		Text = L"";
		HorizontalAlign = HorizontalAlignment::Left;
		VerticalAlign = VerticalAlignment::Top;
		Height = 1;
		Width = 1;
		HorizontalSpacing = 0;
		VerticalSpacing = 0;
		size = 1;
	}

	void CBitmapText::zDraw()
	{
		if (Text.length() == 0)
			return;
		//color
		Core->zContext->UpdateSubresource(DrawManager->zCbBufferPS, 0, NULL, &color, 0, 0);
		//tex
		Core->zContext->PSSetShaderResources(0, 1, &zFont->GetTexture()->zShaderResource);
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
			if (index < 0 || index > zFont->_GetChars().size())
				continue;
			Core->zContext->UpdateSubresource(DrawManager->zCbBufferUV, 0, NULL, &(zFont->_GetChars()[index]), 0, 0);
			//transform letter and draw
			zTextTransform(col,row, len);
			Core->zContext->DrawIndexed(6, 0, 0);
			col++;
		}
	}

	void CBitmapText::zTextTransform(int col, int row, int len)
	{
		float _Height = Height * size;
		float _Width = Width * size;
		float _HorizontalSpacing = HorizontalSpacing * size;
		float _VerticalSpacing = VerticalSpacing * size;
		float horAlignOffset = 0;
		float verAlignOffset = 0;
		if (HorizontalAlign == HorizontalAlignment::Center)
			horAlignOffset = -len / 2.0f;
		else if (HorizontalAlign == HorizontalAlignment::Right)
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
		Core->zContext->UpdateSubresource(DrawManager->zCbBufferVS, 0, NULL, &worldViewProj, 0, 0);
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
		size = 1;
	}

	XMMATRIX CBitmapText::zGetScaleMatrix()
	{
		return XMMatrixIdentity();
	}

	void CBitmapText::Destroy()
	{
		CDynamic::Destroy();
		DrawManager->RemoveBitmapText(this);
		delete this;
	}
}