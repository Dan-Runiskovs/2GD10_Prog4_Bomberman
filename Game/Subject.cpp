#include "Subject.h"

size_t dae::Subject::AddObserver(Callback callback)
{
	m_Observers.emplace_back(std::move(callback));
	return m_Observers.size() - 1;
}

void dae::Subject::RemoveObserver(size_t id)
{
	if (id < m_Observers.size())
	{
		m_Observers.at(id) = nullptr;
	}
}


void dae::Subject::Notify(Event event)
{
	for (auto& callback : m_Observers)
	{
		if(callback) callback(event);
	}
}
