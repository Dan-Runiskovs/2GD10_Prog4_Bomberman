#include "HealthComponent.h"
#include <algorithm>

dae::HealthComponent::HealthComponent(GameObject& owner, uint8_t health) noexcept
	: ComponentBase(owner)
	, m_Health{ health }
{
}

bool dae::HealthComponent::TryTakeDamage(uint8_t damage)
{
	if(m_Health <= 0) return false;
	
	m_Health -= damage;

	m_Subject.Notify(Event::OnHealthChanged);

	if (m_Health <= 0) m_Subject.Notify(Event::OnDeath);

	return true;
}
