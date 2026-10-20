#pragma once

#include <array>

struct ProjectLightingSettings
{
    float shadow_bias = 0.015f;
    float ambient_multiplier = 1.0f;
    std::array<float, 3> ambient_color{0.03f, 0.03f, 0.03f};
};

struct ProjectDebugSettings
{
    bool show_physics_collider_wireframes = false;
};

struct ProjectSettings
{
    ProjectLightingSettings lighting;
    ProjectDebugSettings debug;
};