#pragma once
#include "IComponent.h"
#include <SDL.h>
#include <string>
#include <SDL_image.h>
class SpriteComponent :
    public IComponent
{
public:
    SDL_Surface *sprite = NULL;

public:
    SpriteComponent();
    SpriteComponent(const char* path);

    void assignSprite(const char* path);
    void render(SDL_Renderer *context);
};

