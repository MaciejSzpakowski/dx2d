#include "Private.h"

namespace dx2d
{
	void CEventManager::AddEvent(std::function<int()> func, std::string name)
	{
		CEvent* newEvent = new CEvent;
		newEvent->Activity = func;
		newEvent->Name = name;
		events.push_back(newEvent);
	}

	void CEventManager::RemoveEvent(std::string name)
	{
		for(int i = 0;i<events.size();i++)
			if (events[i] && events[i]->Name == name)
			{
				delete events[i];
				events[i] = nullptr;
			}
	}

	void CEventManager::Activity()
	{
		//might be removed so backwards iteration
		for (int i = (int)events.size() - 1; i >= 0; i--)
		{
			//pop if last event is empty
			if (events[i] == nullptr && i == events.size() - 1)
				events.pop_back();
			//if its empty but not last
			else if (events[i] == nullptr)
			{
				events[i] = events.back();
				events.pop_back();
			}
			//if returned 0 then remove
			else if (!events[i]->Activity())
			{
				delete events[i];
				events[i] = events.back();
				events.pop_back();
			}
		}
	}

	void CEventManager::Destroy()
	{
		for (int i = 0; i < events.size(); i++)
			delete events[i];
		delete this;
	}
}