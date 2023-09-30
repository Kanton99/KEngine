#pragma once
#include "IComponent.h"
#include <glm.hpp>

class Transform :
    public IComponent
{
protected:
    glm::mat4 transform;
public:
    Transform();

    void translate(glm::vec3 t);
    void translate(float x, float y, float z);
    void rotate(float angle, glm::vec3 axis);
    void scale(glm::vec3 scale);
    void scale(float x, float y, float z);
};

class Transform2D :
    public Transform
{

public:
    Transform2D();

    void rotate(float degrees);
    void translate(float x, float y);
    void translate(glm::vec2 t);
    void scale(glm::vec2 scale);
    void scale(float x, float y);
};

