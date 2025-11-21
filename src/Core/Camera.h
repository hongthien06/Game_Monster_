#pragma once
#include <glm/glm.hpp>
#include <algorithm>

class Camera {
private:
    glm::vec2 position;
    glm::vec2 size; 
    glm::vec2 worldSize;
    glm::vec2 deadZoneSize;
    float smoothness; 

public:
    Camera(float width, float height, float worldWidth, float worldHeight);
    void update(const glm::vec2& target, float deltaTime);
    glm::vec2 getOffset() const;
};
