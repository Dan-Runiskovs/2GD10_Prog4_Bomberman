#pragma once
#include <functional>
#include <vector>
#include <cstdint>

namespace dae
{
	enum class Event : uint8_t
	{
		OnHealthChanged,
		OnDeath,
		OnScoreChanged
	};

	class Subject
	{
	public:
		using Callback = std::function<void(Event)>;

		size_t AddObserver(Callback callback);
		void RemoveObserver(size_t id);

		void Notify(Event event);

	private:
		std::vector<Callback> m_Observers{};
	};
}