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

	const auto& texture = *m_pTexture.get();
	auto pos = m_TransformComponent.GetWorldPosition();

	if (m_Width && m_Heigth)
	{
		if (m_DrawCentered)
		{
			Renderer::GetInstance().RenderTexture(texture, pos.x - (m_Width / 2.f), pos.y - (m_Width / 2.f), m_Width, m_Heigth);
			return;
		}
		Renderer::GetInstance().RenderTexture(texture, pos.x, pos.y, m_Width, m_Heigth);
		return;
	}
	else
	{
		if (m_DrawCentered)
		{
			const auto size = texture.GetSize();
			Renderer::GetInstance().RenderTexture(texture, pos.x - (size.x / 2.f), pos.y - (size.y / 2.f));
			return;
		}
		Renderer::GetInstance().RenderTexture(texture, pos.x, pos.y);
		return;
	}
}
