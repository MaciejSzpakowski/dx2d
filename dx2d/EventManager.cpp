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
		zEvents.push_back(newEvent);
		return newEvent;
	}

	void CEventManager::RemoveEvent(wstring name)
	{
		for(int i = 0;i<zEvents.size();i++)
			if (zEvents[i] && zEvents[i]->Name == name)
			{
				delete zEvents[i];
				zEvents[i] = nullptr;
				return;
			}

		throw std::runtime_error("CEventManager::RemoveEvent() event not found");
	}

	void CEventManager::RemoveEvent(Event* e)
	{
		for (int i = 0; i<zEvents.size(); i++)
			if (zEvents[i] && zEvents[i] == e)
			{
				delete zEvents[i];
				zEvents[i] = nullptr;
				return;
			}

		throw std::runtime_error("CEventManager::RemoveEvent() event not found");
	}

	void CEventManager::zActivity()
	{
		//might be removed so backwards iteration
		for (int i = (int)zEvents.size() - 1; i >= 0; i--)
		{
			//pop if last event is empty
			if (zEvents[i] == nullptr && i == zEvents.size() - 1)
			{
				zEvents.pop_back();
				continue;
			}
			//if its empty but not last
			else if (zEvents[i] == nullptr)
			{
				zEvents[i] = zEvents.back();
				zEvents.pop_back();
				continue;
			}			
			int ret = 1;
			
			//if delay time has passed...
			if (Core->GetGameTime() - zEvents[i]->startTime > zEvents[i]->delay)
			//...and it's time for next pulse then run
			if (zEvents[i]->tick == 0 ||
				(Core->GetGameTime() - zEvents[i]->lastPulse > zEvents[i]->tick))
			{
				ret = zEvents[i]->Activity();
				zEvents[i]->lastPulse = Core->GetGameTime();
			}
			//if returned 0 or expired then remove
			if (ret == 0 || (zEvents[i]->lifeTime > 0 &&
				Core->GetGameTime() - zEvents[i]->startTime > zEvents[i]->lifeTime))
			{
				delete zEvents[i];
				zEvents[i] = zEvents.back();
				zEvents.pop_back();
			}
		}
	}

	void CEventManager::Destroy()
	{
		for (int i = 0; i < zEvents.size(); i++)
			delete zEvents[i];
		delete this;
	}
}