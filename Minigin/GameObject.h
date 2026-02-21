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
		GameObject();
		GameObject(GameObject* pParent);
		void Update() const;
		void MarkForDelete() { m_IsMarkedForDelete = true; }			//TODO: Mark children for deletion as well
		bool IsMarkedForDelete() const { return m_IsMarkedForDelete; }

#pragma region ChildManipulation
		bool IsChild(GameObject* parent) const;
		void SetParent(GameObject* parent, bool keepWorldPosition);
		GameObject* GetParent() { return m_pParent; }
		const GameObject* GetParent() const { return m_pParent; }
		const unsigned int GetChildCount() const;
		GameObject* GetChildAt(unsigned int index) { return m_pChildren.at(index); }
		const GameObject* GetChildAt(unsigned int index) const { return m_pChildren.at(index); }
#pragma endregion ChildManipulation

#pragma region ComponentManipulation
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

		template<DerivedComponent ComponentType>
		ComponentType& GetComponent()
		{
			auto* pComponent = TryGetComponent<ComponentType>();
			assert(pComponent && "Required component missing");
			return *pComponent;
		}

		template<DerivedComponent ComponentType>
		const ComponentType& GetComponent() const
		{
			auto* pComponent = TryGetComponent<ComponentType>();
			assert(pComponent && "Required component missing");
			return *pComponent;
		}

		template<dae::DerivedComponent ComponentType>
		static bool IsSameType(std::unique_ptr<ComponentBase> const& pComponent) noexcept {
			return dynamic_cast<ComponentType*>(pComponent.get());
		}

		template<dae::DerivedComponent ComponentType>
		bool HasComponent() const noexcept {
			return std::ranges::any_of(m_pComponents, IsSameType<ComponentType>);
		}

		template<DerivedComponent ComponentType, typename... Args>
		ComponentType& AddComponent(Args&&... args)
		{
			assert(!HasComponent<ComponentType>() && "Component already exists");

			auto component = std::make_unique<ComponentType>(*this, std::forward<Args>(args)...);
			ComponentType& ref = *component;
			m_pComponents.emplace_back(std::move(component));
			return ref;
		}
#pragma endregion ComponentManipulation

		~GameObject() = default;
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;
	private:
		std::vector<std::unique_ptr<ComponentBase>> m_pComponents{};
		GameObject* m_pParent{ nullptr };
		std::vector<GameObject*> m_pChildren{};

		void AddChild(GameObject* pChild);
		void RemoveChild(GameObject* pChild);

		bool m_IsMarkedForDelete{ false };
	};
}
