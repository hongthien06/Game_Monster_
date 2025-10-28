#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include <vector>
#include "../Config/GameConstants.h"


 //* Class Projectile - Quản lý mũi tên và các đạn bay
 //*
 //* Chức năng:
 //* - Di chuyển theo hướng với tốc độ cố định
 //* - Xoay theo hướng bay
 //* - Collision detection
 //* - Trail effect (đuôi)
 //* - Tự hủy sau thời gian

class Projectile {
protected:
    // ===== VỊ TRÍ VÀ CHUYỂN ĐỘNG =====
    glm::vec2 position;      
    glm::vec2 velocity;      
    float speed;            
    float rotation;          
    // ===== COMBAT =====
    int damage;             
    bool isActive;          

    // ===== LIFETIME =====
    float lifetime;         
    float lifeTimer;        

    // ===== RENDERING =====
    SDL_Texture* texture;   
    SDL_Renderer* renderer; 
    // ===== TRAIL EFFECT =====
    struct TrailPoint {
        glm::vec2 position;
        float alpha;        
        float size;        
    };

    std::vector<TrailPoint> trailPoints;
    bool hasTrail;           
    SDL_Color trailColor;    
    float trailSpawnRate;   
    float trailSpawnTimer;   
    int maxTrailPoints;      

public:
    
    Projectile(SDL_Renderer* renderer, glm::vec2 startPos,
        glm::vec2 direction, float speed, int damage);

   
    virtual ~Projectile();
    virtual void Update(float deltaTime);

    
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset);

    // ===== COLLISION =====

    /**
     * GetBoundingBox - Lấy hộp va chạm
     * @return SDL_FRect chứa thông tin va chạm
     */
    SDL_FRect GetBoundingBox() const;

    /**
     * CheckCollision - Kiểm tra va chạm với target
     * @param target: Hộp va chạm của target
     * @return true nếu va chạm
     */
    bool CheckCollision(const SDL_FRect& target);

    /**
     * OnHit - Xử lý khi trúng mục tiêu
     * Gọi hàm này khi projectile va chạm với enemy/player
     */
    virtual void OnHit();

    // ===== GETTERS =====
    bool IsActive() const { return isActive; }
    glm::vec2 GetPosition() const { return position; }
    int GetDamage() const { return damage; }
    float GetRotation() const { return rotation; }

    // ===== SETTERS =====
    void SetTrailEnabled(bool enabled) { hasTrail = enabled; }
    void SetTrailColor(SDL_Color color) { trailColor = color; }

protected:
    /**
     * UpdateTrail - Cập nhật trail effect
     * @param deltaTime: Thời gian giữa 2 frame
     */
    void UpdateTrail(float deltaTime);

    /**
     * RenderTrail - Vẽ trail
     * @param renderer: SDL renderer
     * @param cameraOffset: Camera offset
     */
    void RenderTrail(SDL_Renderer* renderer, glm::vec2 cameraOffset);

    /**
     * CalculateRotation - Tính góc xoay dựa trên velocity
     */
    void CalculateRotation();
};
