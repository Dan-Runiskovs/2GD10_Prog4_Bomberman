#pragma once
#include <memory>
#include <concepts>

namespace dae
{
	class GameObject;
	class ComponentBase
	{
	public:
		enum class ComponentID : unsigned char
		{
			Transform = 0,
			Render = 1,
			Text = 2,
			FPS = 255
		};
		ComponentID GetID() const { return m_ID; }
		bool HasSameID(ComponentID id) const { return m_ID == id; }

		virtual void Update() = 0;

		virtual ~ComponentBase() noexcept = default;
		ComponentBase(const ComponentBase& other) noexcept = delete;
		ComponentBase(ComponentBase&& other) noexcept = delete;
		ComponentBase& operator=(const ComponentBase& other) noexcept = delete;
		ComponentBase& operator=(ComponentBase&& other) noexcept = delete;

	protected:
		ComponentBase(GameObject& owner, ComponentID id);
		const ComponentID m_ID;
		GameObject& m_Owner;
	};

	template<typename DerivedComponentType>
	concept DerivedComponent = std::derived_from<DerivedComponentType, ComponentBase>;
}