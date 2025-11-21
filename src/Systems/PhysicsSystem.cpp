#include "PhysicsSystem.h"
#include <algorithm>
#include <iostream>
#include "..//Environment/Map.h"
#include "../Config/GameConstants.h" 

// HÀM XỬ LÝ VẬT LÝ CHÍNH
void PhysicsSystem::ApplyPhysics(glm::vec2& position, glm::vec2& velocity, float deltaTime, Map& map, bool& isOnGround)
{
    // ÁP DỤNG TRỌNG LỰC
    velocity.y += GameConstants::GRAVITY * deltaTime;

    glm::vec2 newPos = position;
    isOnGround = false;
    // XỬ LÝ DI CHUYỂN VÀ VA CHẠM THEO TRỤC Y
    newPos.y += velocity.y * deltaTime;
    SDL_FRect playerRectY = {
        newPos.x + GameConstants::PLAYER_PHYSICS_BOX_OFFSET_X,    
        newPos.y + GameConstants::PLAYER_PHYSICS_BOX_OFFSET_Y,   
        GameConstants::PLAYER_PHYSICS_BOX_WIDTH,             
        GameConstants::PLAYER_PHYSICS_BOX_HEIGHT              
    };

    // KIỂM TRA VA CHẠM VỚI BẢN ĐỒ
    if (map.checkCollision(playerRectY)) {
        if (velocity.y >= 0) {
            isOnGround = true;
        }

        newPos.y = position.y;
        velocity.y = 0;
    }


    // XỬ LÝ DI CHUYỂN VÀ VA CHẠM THEO TRỤC X
    newPos.x += velocity.x * deltaTime;

    SDL_FRect playerRectX = {
        newPos.x + GameConstants::PLAYER_PHYSICS_BOX_OFFSET_X,    
        position.y + GameConstants::PLAYER_PHYSICS_BOX_OFFSET_Y,  
        GameConstants::PLAYER_PHYSICS_BOX_WIDTH,              
        GameConstants::PLAYER_PHYSICS_BOX_HEIGHT              
    };

    // KIỂM TRA VA CHẠM VỚI BẢN ĐỒ
    if (map.checkCollision(playerRectX)) {
        newPos.x = position.x;
        velocity.x = 0;
    }

    position = newPos;

    // GIỚI HẠN TRONG BẢN ĐỒ
    position.x = std::max(0.0f - GameConstants::PLAYER_PHYSICS_BOX_OFFSET_X, std::min(position.x,GameConstants::WORLD_WIDTH - GameConstants::PLAYER_PHYSICS_BOX_WIDTH - GameConstants::PLAYER_PHYSICS_BOX_OFFSET_X 
        )
    );
}