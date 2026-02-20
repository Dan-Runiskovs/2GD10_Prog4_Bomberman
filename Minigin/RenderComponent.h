#pragma once
#include <string>
#include "ComponentBase.h"

namespace dae
{
	class Texture2D;
	class TransformComponent;
	class RenderComponent final : public ComponentBase
	{
	public:
		RenderComponent(GameObject& owner) noexcept;
		RenderComponent(GameObject& owner, std::shared_ptr<Texture2D> pTexture) noexcept;

		void Update() override {};

		void SetTexture(const std::string& filename);
		void SetTexture(std::shared_ptr<Texture2D> pTexture);

		void Render() const;

		virtual ~RenderComponent() = default;
		RenderComponent(const RenderComponent& other) = delete;
		RenderComponent(RenderComponent&& other) = delete;
		RenderComponent& operator=(const RenderComponent& other) = delete;
		RenderComponent& operator=(RenderComponent&& other) = delete;
	private:
		std::shared_ptr<Texture2D> m_pTexture{ nullptr };

		TransformComponent& m_TransformComponent;	// Requires Transform component
	};
}