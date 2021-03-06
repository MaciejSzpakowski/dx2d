#pragma once

namespace Viva
{
	class CEventManager
	{
	private:
		vector<Event*> events;
	public:
		void _Activity();

		//func: code to execute by event (function pointer or closure)
		//delay: how long to wait before start running
		//lifeTime: destroy event after that time, 0 to never destroy
		//tick: run event once every tick seconds, 0 to run every frame
		Event* AddEvent(std::function<int()> func, wstring name, double delay, double lifeTime, double tick);
		
		void RemoveEvent(wstring name);
		void RemoveEvent(Event* e);
		
		void Destroy();
	};
}
