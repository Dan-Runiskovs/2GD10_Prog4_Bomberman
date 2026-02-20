#pragma once
#include <string>
#include <memory>
#include <optional>
#include <vector>
#include <algorithm>
#include <assert.h>
#include "ComponentBase.h"

namespace dae
{
	class Texture2D;
	class GameObject final
	{
	public:
		GameObject() = default;
		void Update() const;
		void MarkForDelete() { m_IsMarkedForDelete = true; }			//TODO: Mark children for deletion as well
		bool IsMarkedForDelete() const { return m_IsMarkedForDelete; }
		
		template<dae::DerivedComponent ComponentType>
		void TryRemoveComponent()
		{
			std::erase_if(m_pComponents,IsSameType<ComponentType>);
		}
		
		template<dae::DerivedComponent ComponentType>
		ComponentType* TryGetComponent()
		{
			for (auto& pComponent : m_pComponents)
			{
				ComponentType* pDerived{ dynamic_cast<ComponentType*>(pComponent.get()) };
				if (pDerived)
					return pDerived;
			}
			return nullptr;
		}

		template<dae::DerivedComponent ComponentType>
		const ComponentType* TryGetComponent() const
		{
			for (const auto& pComponent : m_pComponents)
			{
				ComponentType* pDerived{ dynamic_cast<ComponentType*>(pComponent.get()) };
				if (pDerived)
					return pDerived;
			}
			return nullptr;
		}

		template<dae::DerivedComponent ComponentType>
		static bool IsSameType(std::unique_ptr<ComponentBase> const& pComponent) noexcept {
			return dynamic_cast<ComponentType*>(pComponent.get());
		}

		template<dae::DerivedComponent ComponentType>
		bool HasComponent() const noexcept {
			return std::ranges::any_of(m_pComponents, IsSameType<ComponentType>);
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

		~GameObject() = default;
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;
	private:
		std::vector<std::unique_ptr<ComponentBase>> m_pComponents{};
		bool m_IsMarkedForDelete{ false };
	};
}
