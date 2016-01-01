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

		_UpdateScales();
	}

	void BitmapText::_UpdateScales()
	{
		Size letterSize;
		float h;
		float w;

		charScale.clear();
		charScaleMat.clear();

		for (int i = 0; i < font->_GetChars().size(); i++)
		{
			const Rect& uv = font->_GetChars()[i];
			letterSize.width = (size_t)((uv.right - uv.left)*font->GetTexture()->GetSize().width);
			letterSize.height = (size_t)((uv.bottom - uv.top)*font->GetTexture()->GetSize().height);
			letterSize.width = 10;
			letterSize.height = 19;

			XMFLOAT2 frustum = Core->GetCamera()->GetFrustumSize(GetPosition().z);
			RECT client;
			GetClientRect(Core->GetWindowHandle(), &client);
			XMFLOAT2 clientSize = { (float)client.right - client.left,
				(float)client.bottom - client.top };
			XMFLOAT2 unitsPerPixel = { frustum.x / clientSize.x, frustum.y / clientSize.y };
			w = metrics.width * size * unitsPerPixel.x * letterSize.width / 2;
			h = metrics.height * size * unitsPerPixel.y * letterSize.height / 2;

			charScaleMat.push_back(DirectX::XMMatrixScaling(w, h, 1));
			charScale.push_back(XMFLOAT2(w, h));
		}
	}

	void BitmapText::_FindLongestLine()
	{
		int index = 0;
		int index2 = text.find(L'\n');
		longestLine = index2 - index;

		while (index2 != wstring::npos)
		{

		}
	}

	void BitmapText::_TextTransform(float col, int row, int len, int _char)
	{
		float _VerticalSpacing = metrics.verticalSpacing * size;
		float horAlignOffset = 0;
		float verAlignOffset = 0;

		if (metrics.horizontalAlign == HorizontalAlignment::Center)
			horAlignOffset = -len / 2.0f;
		else if (metrics.horizontalAlign == HorizontalAlignment::Right)
			horAlignOffset = (float)-len;

		XMMATRIX& scale = charScaleMat[_char - 32];
		XMMATRIX rot = DirectX::XMMatrixRotationRollPitchYawFromVector(rotation);
		//origin is translation matrix from the center of the text object
		XMMATRIX origin = DirectX::XMMatrixTranslation((col + horAlignOffset)*2,
			(-row*(charScale[_char - 32].y + _VerticalSpacing) + verAlignOffset)*2, 0);
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

	void BitmapText::_Draw()
	{
		if (text.length() == 0)
			return;
		//color
		Core->_GetContext()->UpdateSubresource(DrawManager->_GetConstantBufferPS(), 0, NULL, &color, 0, 0);
		//tex
		Core->_GetContext()->PSSetShaderResources(0, 1, font->GetTexture()->_GetShaderResourceAddress());
		int len = (int)text.length();
		int lineLen = (int)text.find(L'\n');
		float horiOff = 0; // horizontal offset of the letter
		int row = 0;
		for (int i = 0; i < len; i++)
		{
			//check for new line
			if (text[i] == '\n')
			{
				lineLen = (int)text.find(L'\n',i+1);
				row++;
				horiOff = 0;
				continue;
			}			
			//uv
			int index = text[i] - ' ';
			if (index < 0 || index > font->_GetChars().size())
				continue;
			Core->_GetContext()->UpdateSubresource(DrawManager->_GetConstantBufferUV(), 0, NULL,
				&(font->_GetChars()[index]), 0, 0);
			//transform letter and draw
			_TextTransform(horiOff, row, lineLen, text[i]);
			Core->_GetContext()->DrawIndexed(6, 0, 0);
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