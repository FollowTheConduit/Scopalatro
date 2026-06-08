#if 0

#pragma once

#include <functional>
#include <list>
#include <memory>
#include <vector>

#include <Combat/CombatModelListener.hpp>
#include <Combat/EventContext.hpp>

class CharacterState;
class CombatModel;

struct StatusBase
{
	bool alive = true;
};

template<class Context>
struct Status: virtual public StatusBase
{
	virtual void Solve (Context & ctx, CombatModel & model) = 0;
};

class StatusStack
{
public:
	template<class StatusType, class ... Args>
	void AddStatus (Args ... args)
	{
		m_statuses.emplace (std::move (std::make_unique<StatusType> (std::forward (args)...)));
	}

	template<class EventContext>
	void Solve (EventContext & ctx, CombatModel & model)
	{
		for (auto & status : m_statuses)
		{
			auto* target = dynamic_cast<Status<EventContext> *>(status.get ());

			if (!target) continue;

			target->Solve (ctx, model);
			if (!status->alive)
			{
				m_statuses.remove (status);
			}
		}
	}

private:
	std::map<std::string, std::unique_ptr<StatusBase>> m_statuses;
};

#endif