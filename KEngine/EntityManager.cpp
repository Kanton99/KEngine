#include "EntityManager.hpp"

#include <assert.h>

EntityManager::EntityManager():
	mLivingEntityCount(0)
{
	for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) mAvailableEntities.push(entity);
}

Entity EntityManager::createEntity()
{
	assert(mLivingEntityCount < MAX_ENTITIES && "Too many entities in existence");

	Entity id = mAvailableEntities.front();
	mAvailableEntities.pop();
	++mLivingEntityCount;
	return id;
}

void EntityManager::destroyEntity(Entity entity)
{
	assert(entity < MAX_ENTITIES && "Entity out of range");

	mSignatures[entity].reset();

	mAvailableEntities.push(entity);
	--mLivingEntityCount;
}

void EntityManager::setSignature(Entity entity, Signature signature)
{
	assert(entity < MAX_ENTITIES && "Entity out of range");

	mSignatures[entity] = signature;
}

Signature EntityManager::GetSignature(Entity entity)
{
	assert(entity < MAX_ENTITIES && "Entity out of range");

	return mSignatures[entity];
}

