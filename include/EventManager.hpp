#pragma once

// TODO: add this to engine

#include <stdint.h>

#include <functional>
#include <map>
#include <set>
#include <vector>

using EventID = uint64_t;

enum class EventResult
{
	Yield,
	Return
};

class EventManager;

using Event = std::function<EventResult (EventID ID)>;

class EventManager
{
public:
	EventID FireEvent (Event && callback);
	EventID FireEventAfter (Event && callback, double ms);
	EventID FireEventAfterAnother (Event && callback, EventID other);
	EventID FireEventAfterAnotherAfter (Event && callback, EventID other, double ms);

	void CancelEvent (EventID ID);

	void Poll (double deltaTime);

private:
	struct EventCapsule
	{
		EventCapsule (Event && event):
			event {event}
		{
			
		}

		EventCapsule (Event && event, double countdown):
			event {event},
			countdown {countdown}
		{
			
		}

		EventCapsule (EventCapsule && other) = default;
		EventCapsule (EventCapsule & other) = default;
		EventCapsule (EventCapsule const & other) = default;

		EventCapsule & operator= (EventCapsule & other) = default;
		EventCapsule & operator= (EventCapsule const & other) = default;

		Event event;
		double countdown = -1.0;
	};

	bool locked = false;

	EventID GenerateID ();
	void Evaluate (EventID ID, EventCapsule & event, double deltaTime);

	std::map<EventID, EventCapsule> m_events;
	std::map<EventID, EventCapsule> m_eventsSafe;

	std::map<EventID, std::map<EventID, EventCapsule>> m_eventsAfter;
	std::map<EventID, std::map<EventID, EventCapsule>> m_eventsAfterSafe;

	std::set<EventID> m_marked;
	EventID m_nextID = 0;
};
