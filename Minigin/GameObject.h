#pragma once
#include <string>
#include <memory>
#include <optional>
#include <vector>
#include "ComponentBase.h"

namespace dae
{
	class Texture2D;
	class GameObject final
	{
	public:
		GameObject() = default;
		void Update() const;
		
		template<dae::DerivedComponent ComponentType>
		void TryRemoveComponent()
		{
			std::erase_if(m_pComponents,
				[](const std::unique_ptr<ComponentBase>& component)
				{
					return component->GetID() == ComponentType::ID;
				});
		}
		
		template<dae::DerivedComponent ComponentType>
		ComponentType* TryGetComponent()
		{
			for (auto& component : m_pComponents)
			{
				if (component->GetID() == ComponentType::ID)
					return static_cast<ComponentType*>(component.get());
			}
			return nullptr;
		}
		template<dae::DerivedComponent ComponentType>
		const ComponentType* TryGetComponent() const
		{
			for (const auto& component : m_pComponents)
			{
				if (component->GetID() == ComponentType::ID)
					return static_cast<const ComponentType*>(component.get());
			}
			return nullptr;
		}

		template<dae::DerivedComponent ComponentType, typename... Args>
		ComponentType* AddComponent(Args&&...args)
		{
			if (TryGetComponent<ComponentType>())
				return nullptr;

			auto component = std::make_unique<ComponentType>(*this, std::forward<Args>(args)...);
			ComponentType* rawPtr = component.get();

			m_pComponents.emplace_back(std::move(component));
			return rawPtr;
		}

		virtual ~GameObject() = default;
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;
	private:
		std::optional<std::vector<std::unique_ptr<ComponentBase>>::iterator> HasComponent(ComponentBase::ComponentID id);
		
		std::vector<std::unique_ptr<ComponentBase>> m_pComponents{};
	};
}
