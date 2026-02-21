#pragma once
#include <memory>
#include <concepts>

namespace dae
{
	class GameObject;
	class ComponentBase
	{
	public:
		virtual void Update() = 0;

		virtual ~ComponentBase() noexcept = default;
		ComponentBase(const ComponentBase& other) noexcept = delete;
		ComponentBase(ComponentBase&& other) noexcept = delete;
		ComponentBase& operator=(const ComponentBase& other) noexcept = delete;
		ComponentBase& operator=(ComponentBase&& other) noexcept = delete;

	protected:
		ComponentBase(GameObject& owner);
		GameObject& GetOwner() { return m_Owner; }
		const GameObject& GetOwner() const { return m_Owner; }
	private:
		GameObject& m_Owner;
	};

	template<typename DerivedComponentType>
	concept DerivedComponent = std::derived_from<DerivedComponentType, ComponentBase>;
}