#include <EventManager.hpp>

#include <core/Logger.hpp>

using namespace TLOT;

EventID EventManager::FireEvent (Event && callback)
{
	return FireEventAfter (std::forward<Event> (callback), -1.0);
}

EventID EventManager::FireEventAfter (Event && callback, double ms)
{
	EventID ID = GenerateID ();

	EventCapsule capsule {std::move (callback), ms};

	if (locked)
	{
		m_eventsSafe.emplace (ID, std::move (capsule));
	}
	else
	{
		m_events.emplace (ID, std::move (capsule));
	}
	return ID;
}

EventID EventManager::FireEventAfterAnotherAfter (Event && callback, EventID other, double ms)
{
	EventID ID = GenerateID ();

	EventCapsule capsule {std::move (callback), ms};

	if (locked)
	{
		m_eventsAfterSafe[other].emplace (ID, std::move (capsule));
	}
	else
	{
		m_eventsAfter[other].emplace (ID, std::move (capsule));
	}

	return ID;
}

EventID EventManager::FireEventAfterAnother (Event && callback, EventID other)
{
	return FireEventAfterAnotherAfter (std::forward<Event> (callback), other, -1.0);
}

void EventManager::CancelEvent (EventID ID)
{
	m_marked.emplace (ID);
	//Logger::log (LogLevel::Info, "Tasked with destroying Event with ID={}", ID);
}

EventID EventManager::GenerateID ()
{
	return m_nextID++;
}

void EventManager::Poll (double deltaTime)
{
	for (auto & ID : m_marked)
	{
		m_events.erase (ID);
		m_eventsAfter.erase (ID);
	}
	m_marked.clear ();

	locked = true;
	for (auto & [ID, event] : m_events)
	{
		Evaluate (ID, event, deltaTime);
	}
	locked = false;

	for (auto & ID : m_marked)
	{
		m_events.erase (ID);
		m_eventsAfter.erase (ID);
	}
	m_marked.clear ();

	m_events.insert (m_eventsSafe.begin (), m_eventsSafe.end ());
	m_eventsAfter.insert (m_eventsAfterSafe.begin (), m_eventsAfterSafe.end ());
}

void EventManager::Evaluate (EventID ID, EventCapsule & event, double deltaTime)
{
	if (event.countdown > 0.0)
	{
		event.countdown -= deltaTime;
	}

	if (event.countdown > 0.0) return;
	
	EventResult result = event.event (ID);
	if (result == EventResult::Return)
	{
		CancelEvent (ID);

		for (auto & [afterID, afterEvent] : m_eventsAfter[ID])
		{
			Evaluate (afterID, afterEvent, deltaTime);
		}
	}

}
