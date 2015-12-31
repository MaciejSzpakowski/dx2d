#pragma once

namespace Viva
{
	class Texture : public Resource
	{
	private:
		Size size;
		ID3D11ShaderResourceView* shaderResource;
	public:

		Texture(ID3D11ShaderResourceView* srv, wstring name=L"") :Resource(name), size(Size(0, 0)), shaderResource(srv) {}

		ID3D11ShaderResourceView* _GetShaderResource() const { return shaderResource; }
		ID3D11ShaderResourceView*const* _GetShaderResourceAddress() const { return &shaderResource; }

		// Create texture from file.
		Texture(const wchar_t* filepath);

		// Create texture from array of pixels with dimensions specified in pixels.
		// Array has only one dimension so rows are stored one after another.
		// Texture is built bottom up meaning, the first row in the array will be at the bottom of the texture.
		// Name is optional and is used if you want to add the texture to the Resource Manager.
		Texture(const Pixel data[], const Size& _size, const wchar_t* _name = L"");

		Size GetSize() const { return size; }

		// Remove texture from resource manager if cached and destroy.
		void Destroy()
		{
			if (cached)
				ResourceManager->RemoveResource(name);

			shaderResource->Release();
			delete this;
		}
	};
}
