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

	void BitmapText::_Draw()
	{
		if (text.length() == 0)
			return;
		//color
		Core->_GetContext()->UpdateSubresource(DrawManager->_GetConstantBufferPS(), 0, NULL, &color, 0, 0);
		//tex
		Core->_GetContext()->PSSetShaderResources(0, 1, font->GetTexture()->_GetShaderResourceAddress());
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
			Core->_GetContext()->UpdateSubresource(DrawManager->_GetConstantBufferUV(), 0, NULL,
				&(font->_GetChars()[index]), 0, 0);
			//transform letter and draw
			_TextTransform(col,row, len);
			Core->_GetContext()->DrawIndexed(6, 0, 0);
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
		XMMATRIX scale = DirectX::XMMatrixScaling(_Width, _Height, 1);
		XMMATRIX rot = DirectX::XMMatrixRotationRollPitchYawFromVector(rotation);
		//origin is translation matrix from the center of the text object
		XMMATRIX origin = DirectX::XMMatrixTranslation(
			(col*(_Width+_HorizontalSpacing) + horAlignOffset)*2,
			(-row*(_Height+_VerticalSpacing) + verAlignOffset)*2, 0);
		XMMATRIX loc = DirectX::XMMatrixTranslationFromVector(position);
		XMMATRIX worldViewProj;
		if (parent == nullptr)
			worldViewProj  = scale * origin * rot * loc * Core->GetCamera()->GetViewMatrix() * Core->GetCamera()->GetProjMatrix();
		else
		{
			XMMATRIX parentLoc = DirectX::XMMatrixRotationRollPitchYawFromVector(parent->_GetRotationVector());
			XMMATRIX parentRot = DirectX::XMMatrixTranslationFromVector(parent->_GetPositionVector());
			worldViewProj = scale * origin * rot * loc * parentLoc * parentRot *
				Core->GetCamera()->GetViewMatrix() * Core->GetCamera()->GetProjMatrix();
		}
		worldViewProj = XMMatrixTranspose(worldViewProj);
		Core->_GetContext()->UpdateSubresource(DrawManager->_GetConstantBufferVS(), 0, NULL, &worldViewProj, 0, 0);
	}

	void BitmapText::SetPixelScale(const Size& _size)
	{		
		XMFLOAT2 frustum = Core->GetCamera()->GetFrustumSize(GetPosition().z);
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
		Dynamic::Destroy();
		if (index != -1)
			DrawManager->RemoveBitmapText(this);
		delete this;
	}
}