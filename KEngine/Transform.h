#pragma once
#include "IComponent.h"
#include "Math.h"
#define _USE_MATH_DEFINES
#include <math.h>

class Transform :
    public IComponent
{
protected:
    Matrix4 transform;

public:
    Transform();

    void translate(Vector3 t);
    void translate(float x, float y, float z);
};

class Transform2D :
    public Transform
{

public:
    Transform2D();

    void rotate(float degrees);
    void translate(float x, float y);
    void translate(Vector2 t);
};

