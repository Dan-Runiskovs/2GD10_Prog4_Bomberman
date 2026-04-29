#pragma once
#include <cstdint>
#include "ComponentBase.h"
#include "Subject.h"

namespace dae
{
	class GameObject;
	class HealthComponent final : public ComponentBase
	{
	public:
		explicit HealthComponent(GameObject& owner, uint8_t health) noexcept;

		void SetHealth(uint8_t health) { m_Health = health; }
		bool TryTakeDamage(uint8_t damege);

		void Update() {} // NO-OP

		uint8_t GetHealth() const { return m_Health; }

		Subject& GetSubject() { return m_Subject; }

		~HealthComponent() noexcept = default;
		HealthComponent(const HealthComponent& other) noexcept = delete;
		HealthComponent(HealthComponent&& other) noexcept = delete;
		HealthComponent& operator=(const HealthComponent& other) noexcept = delete;
		HealthComponent& operator=(HealthComponent&& other) noexcept = delete;
	private:
		uint8_t m_Health;

		Subject m_Subject{};
	};
}