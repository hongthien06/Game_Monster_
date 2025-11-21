#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include "..//Environment/Map.h"
#include "..//Config/GameConstants.h"
#include <iostream>

// TRẠNG THÁI NHÂN VẬT
enum class CharacterState {
    STATE_IDLE,
    STATE_WALKING,
    STATE_RUNNING,
    STATE_JUMPING,
    STATE_DASHING
};

// LỚP HỆ THỐNG DI CHUYỂN
class MovementSystem;

class Character {
    friend class MovementSystem;
protected:
    // CÁC TEXTURE HÌNH ẢNH
    SDL_Texture* idleTex;     
    SDL_Texture* walkTex;     
    SDL_Texture* runTex;       
    SDL_Texture* jumpTex;     
    SDL_Texture* shotTex;      
    SDL_Texture* attackTex;    
    SDL_Texture* hurtTex;     
    SDL_Texture* deadTex;     
    SDL_Texture* heartTexture;
    SDL_Texture* dashTex;

    // CÁC BIẾN VẬT LÝ VÀ TRẠNG THÁI
    SDL_Renderer* renderer;
    glm::vec2 position;        
    glm::vec2 velocity;        
    bool isOnGround;          
    bool wasOnGround;          
    bool landedSoundPlayed;   
    float landedSoundCooldown;
    bool flipHorizontal; 
    bool isRunning;

    // BIẾN CHO KỸ NĂNG DASH
    bool isDashing;           
    float dashTimer;
    float dashCooldownTimer;
    int dashDirection;

    // QUẢN LÝ ANIMATION
    CharacterState currentState;    
    CharacterState previousState;   
    int currentFrame;                
    float animationTimer;            

    // CHỈ SỐ NHÂN VẬT
    int health;               
    int maxHealth;          

    // HÀM LOAD ẢNH
    void LoadTexture(SDL_Renderer* renderer, SDL_Texture** texture, const char* path);

public:
    Character();
    Character(SDL_Renderer* renderer, glm::vec2 startPos,
        const char* idlePath,
        const char* walkPath,
        const char* runPath,
        const char* jumpPath,
        const char* dashPath,
        const char* shotPath = nullptr,     
        const char* attackPath = nullptr,   
        const char* hurtPath = nullptr,      
        const char* deadPath = nullptr,
        const char * heartPath = nullptr
        );     

    virtual ~Character();

    // HÀM VÒNG LẶP GAME
    virtual void Update(float deltaTime, Map& map);
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset);

    // NHẬN SÁT THƯƠNG
    virtual void TakeDamage(int damage) {
        health -= damage;
        if (health < 0) health = 0;
    }

    // SETTER GETTER CHO VỊ TRÍ
    void SetPosition(const glm::vec2& pos);
    glm::vec2 GetPosition() const;

    // SETTER GETTER CHO MÁU
    int GetHealth() const { return health; }
    int GetMaxHealth() const { return maxHealth; }
    void SetHealth(int hp) { health = hp; }
    void SetMaxHealth(int maxHp) { maxHealth = maxHp; health = maxHp; }

    // HÀM TÍNH TOÁN VA CHẠM
    virtual float GetSpriteWidth() const = 0;
    virtual SDL_FRect GetBoundingBox() const {
        return SDL_FRect{
            position.x,
            position.y,
            GetSpriteWidth(),
            GetSpriteWidth()
        };
    }
};