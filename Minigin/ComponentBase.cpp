#include "ComponentBase.h"
#include "GameObject.h"

dae::ComponentBase::ComponentBase(GameObject& owner)
	: m_Owner{ owner }
{
}
