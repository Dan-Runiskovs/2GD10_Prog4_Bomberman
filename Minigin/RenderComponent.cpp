#include "RenderComponent.h"
#include "Texture2D.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Renderer.h"
#include "TransformComponent.h"
#include <assert.h>
#include <SDL3/SDL.h>

dae::RenderComponent::RenderComponent(GameObject& owner) noexcept
	:RenderComponent(owner, nullptr)
{
}

dae::RenderComponent::RenderComponent(GameObject& owner, std::shared_ptr<Texture2D> pTexture) noexcept
	:RenderComponent(owner, std::move(pTexture), 0.f, 0.f)
{
}

dae::RenderComponent::RenderComponent(GameObject& owner, std::shared_ptr<Texture2D> pTexture, float width, float height) noexcept
	:ComponentBase(owner)
	, m_pTexture{ std::move(pTexture) }
	, m_TransformComponent{ owner.GetComponent<TransformComponent>() }
	, m_Width{width}
	, m_Heigth{height}
{
}

void dae::RenderComponent::SetDimensions(float width, float height)
{
	m_Width = width;
	m_Heigth = height;
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

	auto const& pos = m_TransformComponent.GetWorldPosition();

	if (m_Width && m_Heigth)
	{
		Renderer::GetInstance().RenderTexture(*m_pTexture, pos.x, pos.y, m_Width, m_Heigth);
	}
	else
	{
		Renderer::GetInstance().RenderTexture(*m_pTexture, pos.x, pos.y);
	}
}
