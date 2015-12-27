#pragma once

namespace Viva
{
	class CTexture : public Resource
	{
	public:
		Size size;
		ID3D11ShaderResourceView* zShaderResource;

		// Create texture from file.
		CTexture(const wchar_t* filepath);

		// Create texture from array of pixels with dimensions specified in pixels.
		// Array has only one dimension so rows are stored one after another.
		// Texture is built bottom up meaning, the first row in the array will be at the bottom of the texture.
		// Name is optional and is used if you want to add the texture to the Resource Manager.
		CTexture(const Pixel data[], const Size& _size, const wchar_t* _name = L"");

		Size GetSize() const { return size; }

		// Remove texture from resource manager if cached and destroy.
		void Destroy()
		{
			if (cached)
				ResourceManager->RemoveResource(name);

			zShaderResource->Release();
			delete this;
		}
	};
}
