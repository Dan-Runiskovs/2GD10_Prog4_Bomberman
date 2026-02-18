#include "RenderComponent.h"
#include "Texture2D.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Renderer.h"
#include "TransformComponent.h"
#include <assert.h>

dae::RenderComponent::RenderComponent(GameObject& owner) noexcept
	:RenderComponent(owner, nullptr)
{
}

dae::RenderComponent::RenderComponent(GameObject& owner, std::shared_ptr<Texture2D> pTexture) noexcept
	:ComponentBase( owner, ID )
	, m_pTexture{std::move(pTexture)}
{
	m_pTransformComponent = owner.TryGetComponent<TransformComponent>();
	if(!m_pTransformComponent)
	{
		m_pTransformComponent = owner.AddComponent<TransformComponent>();
	}
}

void dae::RenderComponent::SetTexture(const std::string& filename)
{
	m_pTexture = ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::RenderComponent::SetTexture(std::shared_ptr<Texture2D> pTexture)
{
	m_pTexture = pTexture;
}

void dae::RenderComponent::Render() const
{
	assert(m_pTexture && "Texture is not set");

	auto const& pos = m_pTransformComponent->GetPosition();

	Renderer::GetInstance().RenderTexture(
		*m_pTexture, pos.x, pos.y);
}
