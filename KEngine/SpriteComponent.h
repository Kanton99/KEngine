#pragma once
#include "IComponent.h"
#include <SDL.h>
#include <string>
#include <SDL_image.h>
#include <memory>


class SpriteComponent :
    public IComponent
{
public:
    std::unique_ptr<SDL_Surface> sprite;

public:
    SpriteComponent();
    SpriteComponent(const char* path);
    ~SpriteComponent();
    

    void assignSprite(const char* path);
    void render(std::shared_ptr<SDL_Renderer> context);
};

