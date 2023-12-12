#include "RenderSystem.h"

void RenderSystem::draw()
{
	for (auto entity : mEntities) {
		auto renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
		auto transformComponent = gCoordinator.getComponent<Transform>(entity);
		engine->drawModel(renderComponent.vertices, renderComponent.indices, transformComponent.transform);
	}
}

RenderSystem::RenderSystem() :
	engine(std::make_unique<VulkanEngine>(VulkanEngine::get()))
{

}

RenderSystem::~RenderSystem()
{
	engine->cleanup();
}
