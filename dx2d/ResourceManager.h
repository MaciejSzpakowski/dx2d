#pragma once

namespace Viva
{
	class CResourceManager
	{
	private:
		std::map<wstring, Resource*> resources;

	public:
		CResourceManager::CResourceManager() {}

		void AddResource(Resource* res);

		Resource* GetResource(const wstring& name) const;

		// checks if resource is in resource manager, if true, outRes is points to the resource
		bool PeekResource(const wstring& name, Resource** outRes) const;

		void RemoveResource(const wstring& name);

		void RemoveAll();

		void Destroy();
	};
}
