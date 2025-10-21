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

    // Vi tri player o giua man hinh
    glm::vec2 targetCenter = playerPos - (size * 0.5f) + glm::vec2(24.0f, 0.0f);

    // Di chuyen 
    float t = 1.0f - std::exp(-smoothness * deltaTime);
    position += (targetCenter - position) * t;

    // Gioi han camera trong bien ban do
    float maxX = std::max(0.0f, worldSize.x - size.x);
    float maxY = std::max(0.0f, worldSize.y - size.y);
    position.x = std::clamp(position.x, 0.0f, maxX);
    position.y = std::clamp(position.y, 0.0f, maxY);
}


glm::vec2 Camera::getOffset() const {
    return position;
}
