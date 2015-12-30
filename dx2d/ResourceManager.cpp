#include "Viva.h"

namespace Viva
{
	void CResourceManager::AddResource(Resource* res)
	{
		auto it = resources.find(res->GetName());

		if (it != resources.end())
			throw VIVA_ERROR("Resource of that name already exists");

		res->_SetCached(true);
		resources[res->GetName()] = res;
	}

	Resource* CResourceManager::GetResource(const wstring& name) const
	{
		auto it = resources.find(name);

		if (it == resources.end())
			throw VIVA_ERROR("Resource not found");

		return it->second;
	}

	void CResourceManager::RemoveResource(const wstring& name)
	{
		auto it = resources.find(name);

		if (it == resources.end())
			throw VIVA_ERROR("Resource not found");

		it->second->_SetCached(false);
		resources.erase(it);
	}

	bool CResourceManager::PeekResource(const wstring& name, Resource** outRes) const
	{
		auto it = resources.find(name);

		if (it != resources.end() && outRes != nullptr)
			*outRes = it->second;

		return it != resources.end();
	}

	void CResourceManager::RemoveAll()
	{	
		for (auto& r : resources)
			r.second->_SetCached(false);

		resources.clear();
	}

	void CResourceManager::Destroy()
	{
		RemoveAll();
		delete this;
	}
}