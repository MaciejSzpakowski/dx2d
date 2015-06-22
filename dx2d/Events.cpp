#include "Private.h"

namespace dx2d
{
	void CEventManager::AddEvent(std::function<int()> func, wstring name, double delay)
	{
		Event* newEvent = new Event;
		newEvent->delay = delay;
		newEvent->startTime = Core->GetGameTime();
		newEvent->Activity = func;
		newEvent->Name = name;
		events.push_back(newEvent);
	}

	void CEventManager::RemoveEvent(wstring name)
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
			int ret = 1;
			//if delay time has passed then run
			if (Core->GetGameTime() - events[i]->startTime > events[i]->delay)
				ret = events[i]->Activity();
			//if returned 0 then remove
			if (ret == 0)
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