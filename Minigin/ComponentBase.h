#pragma once

namespace dae
{
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
		ComponentBase(ComponentID id);
		virtual ~ComponentBase() = default;

		ComponentID GetID() const { return m_ID; }
		virtual void Update() = 0;

		ComponentBase(const ComponentBase& other) = delete;
		ComponentBase(ComponentBase&& other) = delete;
		ComponentBase& operator=(const ComponentBase& other) = delete;
		ComponentBase& operator=(ComponentBase&& other) = delete;
		
	protected:
		const ComponentID m_ID;
	};
}