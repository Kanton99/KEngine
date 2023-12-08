#pragma once
#include <cstdint>
#include <bitset>
#include <queue>
#include <array>
#include <glm/glm.hpp>

using Entity = uint32_t;

const Entity MAX_ENTITIES = 5000;

#pragma region Components //TODO move to own file
struct Transform {
	glm::mat4 position;
};

using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENTS = 32;
#pragma endregion


using Signature = std::bitset<MAX_COMPONENTS>;
class EntityManager
{
public:
	EntityManager();

	Entity createEntity();
	void destroyEntity(Entity entity);

	void setSignature(Entity entity, Signature signature);
	Signature GetSignature(Entity entity);

private:
	std::queue<Entity> mAvailableEntities{};

	std::array<Signature, MAX_ENTITIES> mSignatures{};

	uint32_t mLivingEntityCount;
};

