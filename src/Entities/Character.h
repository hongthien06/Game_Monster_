#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include "..//Environment/Map.h"
#include "..//Config/GameConstants.h"
#include <iostream>

// Trạng thái cơ bản của Character
enum class CharacterState {
    STATE_IDLE,
    STATE_WALKING,
    STATE_RUNNING,
    STATE_JUMPING
};

class MovementSystem;

// BASE CLASS cho Player và Enemy
class Character {
    friend class MovementSystem;
protected:
    // ===== 8 TEXTURES: 4 CƠ BẢN + 4 BỔ SUNG =====
    SDL_Texture* idleTex;      // Đứng yên
    SDL_Texture* walkTex;      // Đi bộ
    SDL_Texture* runTex;       // Chạy
    SDL_Texture* jumpTex;      // Nhảy
    SDL_Texture* shotTex;      // Bắn (Player) / Magic (Enemy)
    SDL_Texture* attackTex;    // Tấn công cận chiến
    SDL_Texture* hurtTex;      // Bị thương
    SDL_Texture* deadTex;      // Chết

    // ===== RENDERER =====
    SDL_Renderer* renderer;

    // ===== TRANSFORM =====
    glm::vec2 position;        // Vị trí
    glm::vec2 velocity;        // Vận tốc

    // ===== PHYSICS =====
    bool isOnGround;           // Đang đứng trên mặt đất
    bool wasOnGround;          // Trạng thái ground trước đó
    bool landedSoundPlayed;    // Đã phát âm thanh chạm đất
    float landedSoundCooldown; // Cooldown âm thanh chạm đất
    bool flipHorizontal;       // Lật sprite theo chiều ngang
    bool isDashing;            // Đang dash
    float dashTimer;
    float dashCooldownTimer;
    int dashDirection;

    // ===== ANIMATION =====
    CharacterState currentState;    // Trạng thái hiện tại
    CharacterState previousState;   // Trạng thái trước đó
    int currentFrame;                // Frame hiện tại
    float animationTimer;            // Bộ đếm animation

    // ===== HEALTH =====
    int health;                // Máu hiện tại
    int maxHealth;             // Máu tối đa

    // ===== PHƯƠNG THỨC HỖ TRỢ =====
    void LoadTexture(SDL_Renderer* renderer, SDL_Texture** texture, const char* path);

public:
    // ===== CONSTRUCTOR =====
    Character();
    Character(SDL_Renderer* renderer, glm::vec2 startPos,
        const char* idlePath,
        const char* walkPath,
        const char* runPath,
        const char* jumpPath,
        const char* shotPath = nullptr,      // Tùy chọn
        const char* attackPath = nullptr,    // Tùy chọn
        const char* hurtPath = nullptr,      // Tùy chọn
        const char* deadPath = nullptr);     // Tùy chọn

    virtual ~Character();

    // ===== PHƯƠNG THỨC CHÍNH (VIRTUAL) =====
    virtual void Update(float deltaTime, Map& map);
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset);

    // ===== COMBAT (VIRTUAL) =====
    virtual void TakeDamage(int damage) {
        health -= damage;
        if (health < 0) health = 0;
    }

    // ===== TRANSFORM =====
    void SetPosition(const glm::vec2& pos);
    glm::vec2 GetPosition() const;

    // ===== HEALTH =====
    int GetHealth() const { return health; }
    int GetMaxHealth() const { return maxHealth; }
    void SetHealth(int hp) { health = hp; }
    void SetMaxHealth(int maxHp) { maxHealth = maxHp; health = maxHp; }

    // ===== PHƯƠNG THỨC TRỪU TƯỢNG =====
    virtual float GetSpriteWidth() const = 0;

    // ===== BOUNDING BOX (VIRTUAL) =====
    virtual SDL_FRect GetBoundingBox() const {
        return SDL_FRect{
            position.x,
            position.y,
            GetSpriteWidth(),
            GetSpriteWidth()
        };
    }
};