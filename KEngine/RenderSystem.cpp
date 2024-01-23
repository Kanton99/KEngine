#include "RenderSystem.h"

void RenderSystem::draw()
{
	auto imageIndex = engine->preDraw();
	for (auto entity : mEntities) {
		auto renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
		auto transformComponent = gCoordinator.getComponent<Transform>(entity);

		engine->drawModel(renderComponent.vertices, renderComponent.indices, &transformComponent.transform, imageIndex);
	}
	engine->postDraw(imageIndex);
}

RenderSystem::RenderSystem() :
	engine(std::make_unique<VulkanEngine>(VulkanEngine::get()))
{

}

void RenderSystem::loadModel(Entity entity, std::string modelPath)
{
	auto buffers = engine->loadModel(modelPath);
	auto renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
	renderComponent.vertices = buffers.first;
	renderComponent.indices = buffers.second;
}


