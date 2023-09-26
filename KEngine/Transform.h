#pragma once
#include "IComponent.h"
#include "Math.h"
#define _USE_MATH_DEFINES
#include <math.h>

class Transform2D :
    public IComponent
{
private:
    Matrix3 transform;

public:
    Transform2D();

    void rotate(float degrees);
    void translate(float x, float y);
    void translate(Vector2 t);
};

