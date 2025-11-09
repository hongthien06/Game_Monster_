#pragma once
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include "..//Environment/Map.h"
#include "..//Config/GameConstants.h"

class Character;

class MovementSystem {
public:
    static void HandleMovement(Character& character, float deltaTime, Map& map);
    static void RenderDashTrails(SDL_Renderer* renderer);
};
