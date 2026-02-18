#include "ComponentBase.h"
#include "GameObject.h"

dae::ComponentBase::ComponentBase(GameObject& owner, ComponentID id)
	: m_Owner{ owner }
	, m_ID{ id }
{
}
