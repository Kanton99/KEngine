#pragma once
#include "EntityManager.hpp"
#include "ComponentArray.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include <memory>

class ComponentManager
{
private:
	std::unordered_map<const char*, ComponentType> mComponentTypes{};

	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays{};

	ComponentType mNextComponentType{};

	template<typename T>
	std::shared_ptr<ComponentArray<T>> getComponentArray() {
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use");

		return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
	}

public:
	template<typename T>
	void RegisterComponent()
	{
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) == mComponentTypes.end() && "Registering component type more than once.");

		// Add this component type to the component type map
		mComponentTypes.insert({ typeName, mNextComponentType });

		// Create a ComponentArray pointer and add it to the component arrays map
		mComponentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

		// Increment the value so that the next component registered will be different
		++mNextComponentType;
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

		// Return this component's type - used for creating signatures
		return mComponentTypes[typeName];
	}

	template<typename T>
	void AddComponent(Entity entity, T component)
	{
		// Add a component to the array for an entity
		getComponentArray<T>()->insertData(entity, component);
	}

	template<typename T>
	void RemoveComponent(Entity entity)
	{
		// Remove a component from the array for an entity
		GetComponentArray<T>()->RemoveData(entity);
	}

	template<typename T>
	T& GetComponent(Entity entity)
	{
		// Get a reference to a component from the array for an entity
		return getComponentArray<T>()->getData(entity);
	}

	void EntityDestroyed(Entity entity)
	{
		// Notify each component array that an entity has been destroyed
		// If it has a component for that entity, it will remove it
		for (auto const& pair : mComponentArrays)
		{
			auto const& component = pair.second;

			component->entityDestroyed(entity);
		}
	}
};

