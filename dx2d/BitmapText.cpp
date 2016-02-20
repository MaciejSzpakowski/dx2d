#include "Viva.h"

namespace Viva
{
	BitmapText::BitmapText(BitmapFont* _font)
	{
		color = Color(1, 1, 1, 1);
		font = _font;
		text = L"";
		size = 1;
		charScale.reserve(_font->_GetChars().size());
		charScaleMat.reserve(_font->_GetChars().size());

		metrics.horizontalAlign = HorizontalAlignment::Left;
		metrics.verticalAlign = VerticalAlignment::Top;
		metrics.height = 1;
		metrics.width = 1;
		metrics.horizontalSpacing = 0;
		metrics.verticalSpacing = 0;

		SetPixelPerfectScale();
	}

	void BitmapText::SetPixelPerfectScale()
	{
		XMFLOAT2 frustum = Core->GetCamera()->GetFrustumSize(GetPosition().z);
		Size letterSize;
        RECT client;
		GetClientRect(Core->GetWindowHandle(), &client);
		XMFLOAT2 clientSize = { (float)client.right - client.left, (float)client.bottom - client.top };
		XMFLOAT2 unitsPerPixel = { frustum.x / clientSize.x, frustum.y / clientSize.y };
		Rect uv;
		float h;
		float w;

		charScale.clear();
		charScaleMat.clear();

		for (int i = 0; i < font->_GetChars().size(); i++)
		{
			uv = font->_GetCharsUv()[i];
			letterSize.width = font->_GetChars()[i].right - font->_GetChars()[i].left;
			letterSize.height = font->_GetChars()[i].bottom - font->_GetChars()[i].top;			
			w = metrics.width * size * unitsPerPixel.x * letterSize.width / 2;
			h = metrics.height * size * unitsPerPixel.y * letterSize.height / 2;

			charScaleMat.push_back(DirectX::XMMatrixScaling(w, h, 1));
			charScale.push_back(XMFLOAT2(w, h));
		}
	}

	void BitmapText::_TextTransform(float x, int row, float lineLen, int index, const XMMATRIX* view, const XMMATRIX* proj)
	{
		float _VerticalSpacing = metrics.verticalSpacing * size;
		float horAlignOffset = 0;
		float verAlignOffset = 0;

		if (metrics.horizontalAlign == HorizontalAlignment::Center)
			horAlignOffset = -lineLen / 2.0f;
		if (metrics.horizontalAlign == HorizontalAlignment::Right)
			horAlignOffset = -lineLen;

		XMMATRIX& scale = charScaleMat[index];
		XMMATRIX rot = DirectX::XMMatrixRotationRollPitchYawFromVector(rotation);
		//origin is translation matrix from the center of the character sprite
		//offset is from the center becuase that how indexed buffer is built, its origin is in the middle
		//thats why i add charScale[index].x so sprite is drawn from left, not from the middle
		XMMATRIX origin = DirectX::XMMatrixTranslation((x + horAlignOffset) * 2 + charScale[index].x,
				(-row*(charScale[index].y + _VerticalSpacing) + verAlignOffset)*2, 0);
		XMMATRIX loc = DirectX::XMMatrixTranslationFromVector(position);
		XMMATRIX worldViewProj;

		if (parent == nullptr)
			worldViewProj  = scale * origin * rot * loc * *view * *proj;
		else
		{
			XMMATRIX parentLoc = DirectX::XMMatrixRotationRollPitchYawFromVector(parent->_GetRotationVector());
			XMMATRIX parentRot = DirectX::XMMatrixTranslationFromVector(parent->_GetPositionVector());
			worldViewProj = scale * origin * rot * loc * parentLoc * parentRot * *view * *proj;
		}

		worldViewProj = XMMatrixTranspose(worldViewProj);
		Core->_GetContext()->UpdateSubresource(DrawManager->_GetConstantBufferVS(), 0, NULL, &worldViewProj, 0, 0);
	}

	float BitmapText::_CurrentLineLength(int off)
	{
		float len = 0;

		for (int i = off; text[i] != L'\n' && text[i] != 0; i++)
			len += charScale[text[i] - 32].x + metrics.horizontalSpacing;

		return len - metrics.horizontalSpacing;
	}

	void BitmapText::_Draw()
	{
        const XMMATRIX* view = Core->GetCamera()->_GetViewPtr();
        const XMMATRIX* proj = Core->GetCamera()->_GetProjPtr();

		if (text.length() == 0)
			return;
		//color
		Core->_GetContext()->UpdateSubresource(DrawManager->_GetConstantBufferPS(), 0, NULL, &color, 0, 0);
		//tex
		Core->_GetContext()->PSSetShaderResources(0, 1, font->GetTexture()->_GetShaderResourceAddress());
		int len = (int)text.length();
		float lineLen = 0;
		if (metrics.horizontalAlign == HorizontalAlignment::Center || 
			metrics.horizontalAlign == HorizontalAlignment::Right)
			lineLen = _CurrentLineLength(0);

		float horiOff = 0; // horizontal offset of the letter
		int row = 0;
		for (int i = 0; i < len;i++)
		{
			//check for new line
			if (text[i] == '\n')
			{
				if (metrics.horizontalAlign == HorizontalAlignment::Center ||
					metrics.horizontalAlign == HorizontalAlignment::Right)
					lineLen = _CurrentLineLength(i+1);
				row++;
				horiOff = 0;
				continue;
			}			
			//uv
			int index = text[i] - ' ';
			if (index < 0 || index > font->_GetChars().size())
				continue;
			// set uv for VS
			Core->_GetContext()->UpdateSubresource(DrawManager->_GetConstantBufferUV(), 0, NULL,
				&(font->_GetCharsUv()[index]), 0, 0);
			//transform letter and draw
			_TextTransform(horiOff, row, lineLen, index, view, proj);
			Core->_GetContext()->DrawIndexed(6, 0, 0);
			// add offset
			horiOff += charScale[text[i] - 32].x + metrics.horizontalSpacing * size;
		}
	}

	void BitmapText::Destroy()
	{
		Dynamic::Destroy();
		if (index != -1)
			DrawManager->RemoveBitmapText(this);
		delete this;
	}
}