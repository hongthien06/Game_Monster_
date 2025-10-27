#pragma once
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include "..//Environment/Map.h"
#include "..//Config/GameConstants.h"

class PhysicsSystem {
public:
    static void ApplyPhysics(glm::vec2& position, glm::vec2& velocity, float deltaTime, Map& map, bool& isOnGround);
};