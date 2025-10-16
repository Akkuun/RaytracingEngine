#pragma once
#include <vector>
#include "../../shapes/Shape.h"
class RenderEngine
{
public:
    RenderEngine() = default;
    ~RenderEngine() = default;
    void render();
    bool intersect_sphere();

private:
    //Scene Manager that contains all shapes


};