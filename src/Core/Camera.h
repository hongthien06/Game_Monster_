#pragma once
#include <glm/glm.hpp>
#include <algorithm>

class Camera {
private:
    glm::vec2 position; // Vi tri cua camera
    glm::vec2 size; // Kich thuoc cua camera
    glm::vec2 worldSize; // Kich thuoc cua ban do
    glm::vec2 deadZoneSize; // Vung tre
    float smoothness; // He so muot 

public:
    Camera(float width, float height, float worldWidth, float worldHeight);

    // Cap nhat vi tri camera
    void update(const glm::vec2& target, float deltaTime);

    // Tra ve vi tri camera 
    glm::vec2 getOffset() const;
};
