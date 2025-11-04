#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include "..//Environment/Map.h"
#include "..//Config/GameConstants.h"
#include <iostream>

enum class CharacterState {
    STATE_IDLE,
    STATE_WALKING,
    STATE_RUNNING,
    STATE_JUMPING
};

class MovementSystem;

class Character {
    friend class MovementSystem;
protected:
    // ===== 8 TEXTURES: 4 CƠ BẢN + 4 BỔ SUNG =====
    SDL_Texture* idleTex;
    SDL_Texture* walkTex;
    SDL_Texture* runTex;
    SDL_Texture* jumpTex;
    SDL_Texture* shotTex;      // Thêm cho Player
    SDL_Texture* attackTex;    // Thêm cho Player
    SDL_Texture* hurtTex;      // Thêm cho Player
    SDL_Texture* deadTex;      // Thêm cho Player

    // ===== RENDERER =====
    SDL_Renderer* renderer;    // Lưu renderer để dùng chung

    // ===== TRANSFORM =====
    glm::vec2 position;
    glm::vec2 velocity;

    // ===== PHYSICS =====
    bool isOnGround;
    bool wasOnGround;                     // Bien duoc them de xu li logic am thanh
    bool landedSoundPlayed;       // Bien duoc them de xu li logic am thanh
    float landedSoundCooldown;     // Bien duoc them de xu li logic am thanh
    bool flipHorizontal;
    bool isDashing;
    float dashTimer;
    float dashCooldownTimer;
    int dashDirection;

    // ===== ANIMATION =====
    CharacterState currentState;
    CharacterState previousState;
    int currentFrame;
    float animationTimer;

    // ===== HEALTH =====
    int health;
    int maxHealth;

    // ===== HELPER METHOD =====
    void LoadTexture(SDL_Renderer* renderer, SDL_Texture** texture, const char* path);

public:
    // ===== CONSTRUCTOR - HỖ TRỢ 8 TEXTURES =====
    Character();
    Character(SDL_Renderer* renderer, glm::vec2 startPos,
        const char* idlePath,
        const char* walkPath,
        const char* runPath,
        const char* jumpPath,
        const char* shotPath = nullptr,     // Tùy chọn
        const char* attackPath = nullptr,   // Tùy chọn
        const char* hurtPath = nullptr,     // Tùy chọn
        const char* deadPath = nullptr);    // Tùy chọn

    virtual ~Character();

    // ===== CORE METHODS =====
    virtual void Update(float deltaTime, Map& map);
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset);

    // ===== TRANSFORM =====
    void SetPosition(const glm::vec2& pos);
    glm::vec2 GetPosition() const;

    // ===== HEALTH =====
    int GetHealth() const { return health; }
    int GetMaxHealth() const { return maxHealth; }

    // ===== ABSTRACT METHOD =====
    // Đây là hàm VIRTUAL cần thiết để lấy chiều rộng sprite cho thanh máu
    virtual float GetSpriteWidth() const = 0;
};