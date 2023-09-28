#include "IComponent.h"


void IComponent::update()
{
}

void IComponent::setOwner(Entity* owner)
{
	this->owner = owner;
}
