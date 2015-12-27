#pragma once

namespace Viva
{
	class CTexture
	{
	public:
		bool zCached;
		Size size;
		wstring zName;
		ID3D11ShaderResourceView* zShaderResource;

		// create texture from file
		CTexture(const wchar_t* filepath);

		// create texture from byte array
		CTexture(const Color data[], const Size& _size, const wchar_t* _name);

		Size GetSize() const { return size; }

		wstring GetName() const { return zName; }

		void Destroy()
		{
			zShaderResource->Release();
			delete this;
		}
	};
}
