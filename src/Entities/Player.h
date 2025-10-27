#pragma once
#include "Character.h"

// Enum định nghĩa các trạng thái của Player
enum class PlayerState {
    STATE_IDLE,
    STATE_WALK,
    STATE_RUN,
    STATE_JUMP,
    STATE_SHOT,
    STATE_ATTACK,
    STATE_HURT,
    STATE_DEAD
};

class Player : public Character {
private:
    //  CÁC TEXTURE BỔ SUNG 
    SDL_Texture* shotTex;
    SDL_Texture* attackTex;
    SDL_Texture* hurtTex;
    SDL_Texture* deadTex;

    //  TRẠNG THÁI VÀ HOẠT ẢNH 
    PlayerState playerState;
    PlayerState previousPlayerState;

    int playerCurrentFrame;
    float playerAnimationTimer;

    //  THUỘC TÍNH COMBAT 
    int health;
    int maxHealth;
    bool isAlive;
    bool isAttacking;
    bool canMove;

    float attackCooldown;
    float attackTimer;

    float hurtDuration;
    float hurtTimer;

    //  PHƯƠNG THỨC PRIVATE 
    void UpdatePlayerAnimation(float deltaTime);
    void UpdatePlayerState(float deltaTime);
    void HandleInput();
    void LoadAllTextures(SDL_Renderer* renderer);

public:
    Player(SDL_Renderer* renderer, glm::vec2 startPos = glm::vec2(50.0f, 0.0f));

    virtual ~Player();

    virtual void Update(float deltaTime) override;

    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;
    void TakeDamage(int damage);
    void Attack();
    void Shot();
    void Heal(int amount);

    bool IsAlive() const { return isAlive; }
    int GetHealth() const { return health; }
    int GetMaxHealth() const { return maxHealth; }
    PlayerState GetPlayerState() const { return playerState; }
};