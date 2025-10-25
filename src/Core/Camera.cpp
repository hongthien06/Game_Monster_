#include "Camera.h"
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

void Camera::update(const glm::vec2& playerPos, float deltaTime)
{
    deltaTime = std::clamp(deltaTime, 0.0f, 0.033f);

    // Trung tam camera hien tai
    glm::vec2 cameraCenter = position + size * 0.5f;

    // Gioi han vung chet
    glm::vec2 deadZoneHalf = deadZoneSize * 0.5f;
    glm::vec2 deadZoneMin = cameraCenter - deadZoneHalf;
    glm::vec2 deadZoneMax = cameraCenter + deadZoneHalf;

    glm::vec2 target = position;

    // Di chuyen neu ra ngoai vung chet
    if (playerPos.x < deadZoneMin.x)
        target.x -= (deadZoneMin.x - playerPos.x);
    else if (playerPos.x > deadZoneMax.x)
        target.x += (playerPos.x - deadZoneMax.x);

    if (playerPos.y < deadZoneMin.y)
        target.y -= (deadZoneMin.y - playerPos.y);
    else if (playerPos.y > deadZoneMax.y)
        target.y += (playerPos.y - deadZoneMax.y);

    // Di chuyen muot
    float t = 1.0f - std::exp(-smoothness * deltaTime);
    position += (target - position) * t;

    // Gioi han camera trong ban do
    float maxX = std::max(0.0f, worldSize.x - size.x);
    float maxY = std::max(0.0f, worldSize.y - size.y);
    position.x = std::clamp(position.x, 0.0f, maxX);
    position.y = std::clamp(position.y, 0.0f, maxY);
}

glm::vec2 Camera::getOffset() const {
    return position;
}
