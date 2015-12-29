#include "Viva.h"

namespace Viva
{
	BitmapText::BitmapText(BitmapFont* _font)
	{
		color = Color(1, 1, 1, 1);
		font = _font;
		text = L"";
		size = 1;

		metrics.horizontalAlign = HorizontalAlignment::Left;
		metrics.verticalAlign = VerticalAlignment::Top;
		metrics.height = 1;
		metrics.width = 1;
		metrics.horizontalSpacing = 0;
		metrics.verticalSpacing = 0;
	}

	void BitmapText::zDraw()
	{
		if (text.length() == 0)
			return;
		//color
		Core->zContext->UpdateSubresource(DrawManager->zCbBufferPS, 0, NULL, &color, 0, 0);
		//tex
		Core->zContext->PSSetShaderResources(0, 1, &font->GetTexture()->zShaderResource);
		int len = (int)text.length();
		int col = 0;
		int row = 0;
		for (int i = 0; i < len; i++)
		{
			//check for new line
			if (text[i] == '\n')
			{
				row++;
				col = 0;
				continue;
			}			
			//uv
			int index = text[i] - ' ';
			if (index < 0 || index > font->_GetChars().size())
				continue;
			Core->zContext->UpdateSubresource(DrawManager->zCbBufferUV, 0, NULL, 
				&(font->_GetChars()[index]), 0, 0);
			//transform letter and draw
			_TextTransform(col,row, len);
			Core->zContext->DrawIndexed(6, 0, 0);
			col++;
		}
	}

	void BitmapText::_TextTransform(int col, int row, int len)
	{
		float _Height = metrics.height * size;
		float _Width = metrics.width * size;
		float _HorizontalSpacing = metrics.horizontalSpacing * size;
		float _VerticalSpacing = metrics.verticalSpacing * size;
		float horAlignOffset = 0;
		float verAlignOffset = 0;
		if (metrics.horizontalAlign == HorizontalAlignment::Center)
			horAlignOffset = -len / 2.0f;
		else if (metrics.horizontalAlign == HorizontalAlignment::Right)
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

	void BitmapText::SetPixelScale(const Size& _size)
	{		
		XMFLOAT2 frustum = Camera->GetFrustumSize(GetPosition().z);
		RECT client;
		GetClientRect(Core->GetWindowHandle(), &client);
		XMFLOAT2 clientSize = { (float)client.right - client.left,
			(float)client.bottom - client.top };
		XMFLOAT2 unitsPerPixel = { frustum.x / clientSize.x, frustum.y / clientSize.y };
		metrics.width = unitsPerPixel.x * _size.width / 4;
		metrics.height = unitsPerPixel.y * _size.height / 4;
		size = 1;
	}

	void BitmapText::Destroy()
	{
		CDynamic::Destroy();
		DrawManager->RemoveBitmapText(this);
		delete this;
	}
}