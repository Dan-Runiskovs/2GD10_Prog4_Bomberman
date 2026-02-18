#include "RenderComponent.h"
#include "Texture2D.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Renderer.h"
#include <assert.h>

dae::RenderComponent::RenderComponent(GameObject& owner) noexcept
	:RenderComponent(owner, nullptr)
{
}

dae::RenderComponent::RenderComponent(GameObject& owner, std::shared_ptr<Texture2D> pTexture) noexcept
	:ComponentBase( owner, ID )
	, m_pTexture{std::move(pTexture)}
{
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
	
	Renderer::GetInstance().RenderTexture(
		*m_pTexture, 0, 0);
}
