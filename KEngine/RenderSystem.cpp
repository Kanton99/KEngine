#include "RenderSystem.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "Coordinator.h"
#include <SDL_vulkan.h>

extern Coordinator gCoordinator;
void RenderSystem::draw()
{
	//TODO Clear the render
	
	auto imageIndex = engine->preDraw();
	for (auto entity : mEntities) {
		RenderComponent renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
		Transform transformComponent = gCoordinator.getComponent<Transform>(entity);

		engine->drawModel(renderComponent.vertices, renderComponent.indices, &transformComponent.transform, imageIndex);
	}
	engine->postDraw(imageIndex);
}

RenderSystem::RenderSystem() :
	engine(std::unique_ptr<VulkanEngine>(VulkanEngine::get()))
{
	window = engine->_window;
}

void RenderSystem::loadModel(Entity entity, std::string modelPath)
{
	auto buffers = engine->loadModel(modelPath);
	RenderComponent *renderComponent = &(gCoordinator.getComponent<RenderComponent>(entity));
	renderComponent->vertices = buffers.first;
	renderComponent->indices = buffers.second;
}

void RenderSystem::loadTexture(Entity entity, std::string texturePath) {
	engine->createTextureImage(texturePath);

}

void RenderSystem::cleanUp() {
	this->engine->cleanup();
}

