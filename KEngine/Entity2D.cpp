#include "Entity2D.h"


Entity2D::Entity2D()
{
	this->id = "Entity" + eCounter;
	this->addComponent(new Transform2D());
}

Entity2D::Entity2D(const std::string id)
{
	this->id = id;
	this->addComponent(new Transform2D());
}
