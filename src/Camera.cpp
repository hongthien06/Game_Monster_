#include "Camera.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>

Camera::Camera(float width, float height, float worldWidth, float worldHeight)
    : position(0.0f, 0.0f),
    size(width, height),
    worldSize(worldWidth, worldHeight),
    deadZoneSize(width * 0.4f, height * 0.4f),
    smoothness(4.0f)
{
}

void Camera::update(const glm::vec2& playerPos, float deltaTime)
{
    deltaTime = std::clamp(deltaTime, 0.0f, 0.033f); 

    glm::vec2 deadZoneMin = position + (size * 0.5f) - (deadZoneSize * 0.5f);
    glm::vec2 deadZoneMax = position + (size * 0.5f) + (deadZoneSize * 0.5f);

    glm::vec2 targetPos = playerPos - (size * 0.5f) + glm::vec2(24.0f, 0.0f);
    glm::vec2 desiredPos = position;

    if (targetPos.x < deadZoneMin.x)
        desiredPos.x -= (deadZoneMin.x - targetPos.x);
    else if (targetPos.x > deadZoneMax.x)
        desiredPos.x += (targetPos.x - deadZoneMax.x);

    if (targetPos.y < deadZoneMin.y)
        desiredPos.y -= (deadZoneMin.y - targetPos.y);
    else if (targetPos.y > deadZoneMax.y)
        desiredPos.y += (targetPos.y - deadZoneMax.y);


    float t = 1.0f - std::exp(-smoothness * deltaTime);
    position += (desiredPos - position) * t;

    float maxX = std::max(0.0f, worldSize.x - size.x);
    float maxY = std::max(0.0f, worldSize.y - size.y);

    position.x = std::clamp(position.x, 0.0f, maxX);
    position.y = std::clamp(position.y, 0.0f, maxY);
}

glm::vec2 Camera::getOffset() const {
    return position;
}
