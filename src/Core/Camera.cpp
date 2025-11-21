#include "Camera.h"
#include "../Config/GameConstants.h" 
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>


Camera::Camera(float width, float height, float worldWidth, float worldHeight)
    : position(0.0f, 0.0f),
    size(width, height),
    worldSize(worldWidth, worldHeight),
    deadZoneSize(width * 0.3f, height * 0.3f),
    smoothness(6.0f)
{
}

// TÍNH TOÁN VỊ TRÍ CỦA CAMERA
void Camera::update(const glm::vec2& target, float deltaTime)
{

    deltaTime = std::clamp(deltaTime, 0.0f, 0.033f);
    glm::vec2 targetPos = position;

    // TÍNH TOÁN VÙNG CHẾT
    glm::vec2 cameraCenter = position + size * 0.5f;
    glm::vec2 deadZoneHalf = deadZoneSize * 0.5f;
    glm::vec2 deadZoneMin = cameraCenter - deadZoneHalf;
    glm::vec2 deadZoneMax = cameraCenter + deadZoneHalf;

    // KIỂM TRA NẾU NHÂN VẬT RA KHỎI VÙNG CHẾT BÊN TRÁI
    if (target.x < deadZoneMin.x)
        targetPos.x -= (deadZoneMin.x - target.x);
    // KIỂM TRA NẾU NHÂN VẬT RA KHỎI VÙNG CHẾT BÊN PHẢI
    else if (target.x > deadZoneMax.x)
        targetPos.x += (target.x - deadZoneMax.x);
    const float TARGET_Y_SCREEN = size.y - GameConstants::TILE_HEIGHT;
    targetPos.y = (target.y + GameConstants::PLAYER_RENDER_HEIGHT) - TARGET_Y_SCREEN;
    
    // LÀM MƯỢT CAMERA
    float t = 1.0f - std::exp(-smoothness * deltaTime);
    position += (targetPos - position) * t;
    
    // GIỚI HẠN TRONG MAP
    float maxX = std::max(0.0f, worldSize.x - size.x);
    float maxY = std::max(0.0f, worldSize.y - size.y);
    position.x = std::clamp(position.x, 0.0f, maxX);
    position.y = std::clamp(position.y, 0.0f, maxY);
}


// TRẢ VỀ VỊ TRÍ HIỆN TẠI CỦA CAMERA
glm::vec2 Camera::getOffset() const {
    return position;
}