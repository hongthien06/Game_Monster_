#include "PhysicsSystem.h"
#include <algorithm>
#include <iostream>
#include "..//Environment/Map.h"

void PhysicsSystem::ApplyPhysics(glm::vec2& position, glm::vec2& velocity, float deltaTime, Map& map, bool& isOnGround)
{
    velocity.y += GameConstants::GRAVITY * deltaTime;

    glm::vec2 newPos = position;

    newPos.y += velocity.y * deltaTime;
    SDL_FRect playerRectY = { newPos.x, newPos.y, 48.0f, 48.0f };

    if (map.checkCollision(playerRectY)) {
        newPos.y = position.y;
        velocity.y = 0;
        isOnGround = true;
    }
    else {
        if (velocity.y > 10.0f)
            isOnGround = false;
    }

    newPos.x += velocity.x * deltaTime;
    SDL_FRect playerRectX = { newPos.x, position.y, 48.0f, 48.0f };

    if (map.checkCollision(playerRectX)) {
        newPos.x = position.x;
        velocity.x = 0;
    }

    position = newPos;

    position.x = std::max(0.0f, std::min(position.x, GameConstants::WORLD_WIDTH - 48.0f));
}
