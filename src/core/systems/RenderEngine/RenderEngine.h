#pragma once

class RenderEngine
{
public:
    RenderEngine() = default;
    ~RenderEngine() = default;
    void render();
    bool intersect_sphere();
};