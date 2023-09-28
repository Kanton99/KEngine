#include "Entity2D.h"
#include "Transform.h"


Entity2D::Entity2D()
{
	this->id = "Entity" + eCounter;
	this->addComponent(std::move(std::make_unique<Transform2D>()));
}

Entity2D::Entity2D(const std::string id)
{
	this->id = id;
	this->addComponent(std::move(std::make_unique<Transform2D>()));
}
