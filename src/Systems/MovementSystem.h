#pragma once
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include "../Entities/Character.h"
#include "../Environment/Map.h"

class MovementSystem {
public:
    static void HandleMovement(Character& character, float deltaTime, Map& map);
};
