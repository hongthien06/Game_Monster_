#include "Camera.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>

// Giả định: Chiều cao render của Player là 48.0f
static const float PLAYER_RENDER_HEIGHT = 48.0f;
// THÔNG SỐ ĐÃ CẬP NHẬT: Chiều cao của 1 ô gạch
static const float TILE_HEIGHT = 32.0f;

Camera::Camera(float width, float height, float worldWidth, float worldHeight)
    : position(0.0f, 0.0f),
    size(width, height),
    worldSize(worldWidth, worldHeight),
    deadZoneSize(width * 0.3f, height * 0.3f),
    smoothness(6.0f)
{
}

void Camera::update(const glm::vec2& target, float deltaTime)
{
    deltaTime = std::clamp(deltaTime, 0.0f, 0.033f);

    glm::vec2 targetPos = position;

    // --- LOGIC TRỤC X: VẪN DÙNG VÙNG CHẾT (DEAD ZONE) ---
    glm::vec2 cameraCenter = position + size * 0.5f;
    glm::vec2 deadZoneHalf = deadZoneSize * 0.5f;
    glm::vec2 deadZoneMin = cameraCenter - deadZoneHalf;
    glm::vec2 deadZoneMax = cameraCenter + deadZoneHalf;

    if (target.x < deadZoneMin.x)
        targetPos.x -= (deadZoneMin.x - target.x);
    else if (target.x > deadZoneMax.x)
        targetPos.x += (target.x - deadZoneMax.x);
    // ----------------------------------------------------

    // --- LOGIC TRỤC Y: CỐ ĐỊNH MẶT ĐẤT SÁT ĐÁY MÀN HÌNH ---

    // SỬA: Đặt chân Player ở VỊ TRÍ mà tile gạch bắt đầu (size.y - 32.0f)
    // Điều này đảm bảo toàn bộ ô gạch được thấy và nằm sát mép dưới.
    const float TARGET_Y_SCREEN = size.y - TILE_HEIGHT;

    // targetPos.y = (Vị trí chân trong thế giới) - (Vị trí chân mong muốn trên màn hình)
    targetPos.y = (target.y + PLAYER_RENDER_HEIGHT) - TARGET_Y_SCREEN;
    // ----------------------------------------------------------------------

    // Di chuyen muot
    float t = 1.0f - std::exp(-smoothness * deltaTime);
    position += (targetPos - position) * t;

    // Gioi han camera trong ban do
    float maxX = std::max(0.0f, worldSize.x - size.x);
    float maxY = std::max(0.0f, worldSize.y - size.y);
    position.x = std::clamp(position.x, 0.0f, maxX);
    position.y = std::clamp(position.y, 0.0f, maxY);
}

glm::vec2 Camera::getOffset() const {
    return position;
}