#include "ComponentBase.h"
#include "GameObject.h"

dae::ComponentBase::ComponentBase(ComponentID id, std::shared_ptr<GameObject> pOwner)
	: m_ID{ id }
	, m_pOwner{ std::move(pOwner) }
{
}
