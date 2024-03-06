#include "RenderSystem.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "Coordinator.h"
#include <SDL_vulkan.h>


extern Coordinator gCoordinator;

/// <summary>
/// Draw one frame
/// </summary>
void RenderSystem::draw()
{
	//TODO Clear the render
	/*
	auto imageIndex = engine->preDraw();
	for (auto entity : mEntities) {
		RenderComponent renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
		Transform transformComponent = gCoordinator.getComponent<Transform>(entity);

		engine->drawModel(renderComponent.vertices, renderComponent.indices, &transformComponent.transform, imageIndex);
	}
	engine->postDraw(imageIndex);*/
}

RenderSystem::RenderSystem() :
	//engine(std::unique_ptr<VulkanEngine>(VulkanEngine::get()))
	engine(std::unique_ptr<Engine>(new Engine()))
{
	engine->init();
	window = engine->_window;
}

/// <summary>
/// Load 3d model for an enity
/// </summary>
/// <param name="entity"></param>
/// <param name="modelPath"></param>
void RenderSystem::loadModel(Entity entity, std::string modelPath)
{
	//auto buffers = engine->loadModel(modelPath);
	//RenderComponent *renderComponent = &(gCoordinator.getComponent<RenderComponent>(entity));
	//renderComponent->vertices = buffers.first;
	//renderComponent->indices = buffers.second;
}

/// <summary>
/// Load texture for an entity's model
/// </summary>
/// <param name="entity"></param>
/// <param name="texturePath"></param>
void RenderSystem::loadTexture(Entity entity, std::string texturePath) {
	//engine->createTextureImage(texturePath);

}

/// <summary>
/// Clean up the engine's GPU memory
/// </summary>
void RenderSystem::cleanUp() {
	this->engine->cleanup();
}

