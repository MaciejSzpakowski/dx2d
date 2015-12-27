#pragma once

namespace Viva
{
	class CTexture : public Resource
	{
	public:
		Size size;
		ID3D11ShaderResourceView* zShaderResource;

		// create texture from file
		CTexture(const wchar_t* filepath);

		// create texture from byte array
		CTexture(const Color data[], const Size& _size, const wchar_t* _name);

		Size GetSize() const { return size; }

		void Destroy()
		{
			if (cached)
				ResourceManager->RemoveResource(name);

			zShaderResource->Release();
			delete this;
		}
	};
}
