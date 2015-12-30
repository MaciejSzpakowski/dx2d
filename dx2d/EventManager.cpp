#include "Viva.h"

namespace Viva
{
	Event* CEventManager::AddEvent
		(std::function<int()> func, wstring name, double delay, double lifeTime, double tick)
	{
		Event* newEvent = new Event;
		newEvent->tick = tick;
		newEvent->lifeTime = lifeTime;
		newEvent->delay = delay;
		newEvent->startTime = Core->GetGameTime();
		newEvent->Activity = func;
		newEvent->Name = name;
		newEvent->lastPulse = 0;
		events.push_back(newEvent);
		return newEvent;
	}

	void CEventManager::RemoveEvent(wstring name)
	{
		for(int i = 0;i<events.size();i++)
			if (events[i] && events[i]->Name == name)
			{
				delete events[i];
				events[i] = nullptr;
				return;
			}

		throw std::runtime_error("CEventManager::RemoveEvent() event not found");
	}

	void CEventManager::RemoveEvent(Event* e)
	{
		for (int i = 0; i<events.size(); i++)
			if (events[i] && events[i] == e)
			{
				delete events[i];
				events[i] = nullptr;
				return;
			}

		throw std::runtime_error("CEventManager::RemoveEvent() event not found");
	}

	void CEventManager::_Activity()
	{
		//might be removed so backwards iteration
		for (int i = (int)events.size() - 1; i >= 0; i--)
		{
			//pop if last event is empty
			if (events[i] == nullptr && i == events.size() - 1)
			{
				events.pop_back();
				continue;
			}
			//if its empty but not last
			else if (events[i] == nullptr)
			{
				events[i] = events.back();
				events.pop_back();
				continue;
			}			
			int ret = 1;
			
			//if delay time has passed...
			if (Core->GetGameTime() - events[i]->startTime > events[i]->delay)
			//...and it's time for next pulse then run
			if (events[i]->tick == 0 ||
				(Core->GetGameTime() - events[i]->lastPulse > events[i]->tick))
			{
				ret = events[i]->Activity();
				events[i]->lastPulse = Core->GetGameTime();
			}
			//if returned 0 or expired then remove
			if (ret == 0 || (events[i]->lifeTime > 0 &&
				Core->GetGameTime() - events[i]->startTime > events[i]->lifeTime))
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