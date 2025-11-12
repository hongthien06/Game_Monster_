#pragma once

#include <vector>
#include <memory>
#include "../Core/Audio.h"
#include "Character.h"
#include "Projectile.h"

// Enum định nghĩa các trạng thái của Player
enum class PlayerState {
    STATE_IDLE,
    STATE_WALK,
    STATE_RUN,
    STATE_JUMP,
    STATE_DASH,
    STATE_SHOT,
    STATE_ATTACK,
    STATE_HURT,
    STATE_DEAD
};

class Player : public Character {
private:
    // ===== TRẠNG THÁI VÀ ANIMATION =====
    PlayerState playerState;
    PlayerState previousPlayerState;
    int playerCurrentFrame;
    float playerAnimationTimer;

    // ===== COMBAT =====
    bool isAlive;
    bool isAttacking;
    bool canMove;
    float attackCooldown;
    float attackTimer;
    float hurtDuration;
    float hurtTimer;

    // FIX: I-FRAMES - BẤT TỬ TẠM THỜI
    float iFramesDuration;
    float iFramesTimer;
    bool isFlashing;
    float flashTimer;

    // ====== AUDIO ======
    Audio audio;

    // ===== SHOOTING SYSTEM =====
    std::vector<std::unique_ptr<Projectile>> projectiles;
    float shootCooldown;
    float shootTimer;
    int projectileDamage;
    float projectileSpeed;
    glm::vec2 mouseWorldPos;      // Vị trí chuột trong world

    // ===== ARROW SPAWNING =====
    bool shouldSpawnArrow;        // Cờ để spawn arrow
    int arrowSpawnFrame;          // Frame nào thì spawn arrow

    // ===== PRIVATE METHODS =====
    void UpdatePlayerAnimation(float deltaTime);
    void UpdatePlayerState(float deltaTime);
    void HandleInput();
    void UpdateProjectiles(float deltaTime);
    glm::vec2 GetArrowSpawnPosition() const;
    void SpawnArrow();

    // THÊM MỚI: Knockback & I-frames
    bool isInvulnerable;
    float invulnerabilityDuration;
    float invulnerabilityTimer;

    glm::vec2 knockbackVelocity;
    float knockbackDecay;
    bool isKnockedBack;

public:
    // ===== CONSTRUCTOR / DESTRUCTOR =====
    Player(SDL_Renderer* renderer, glm::vec2 startPos = glm::vec2(50.0f, 0.0f));
    virtual ~Player();

    // ===== CORE METHODS =====
    virtual void Update(float deltaTime, Map& map) override;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;

    // ===== COMBAT METHODS =====
    // FIX: OVERRIDE TakeDamage VỚI I-FRAMES
    virtual void TakeDamage(int damage) override;

    bool IsInvulnerable() const { return iFramesTimer > 0; }

    void Attack();
    void Shot();
    void Heal(int amount);

    // ===== MOUSE/SHOOTING =====
    void HandleMouseClick(float mouseScreenX, float mouseScreenY, glm::vec2 cameraOffset);
    void UpdateMousePosition(float mouseScreenX, float mouseScreenY, glm::vec2 cameraOffset);

    // ===== GETTERS =====
    bool IsAlive() const { return isAlive; }
    PlayerState GetPlayerState() const { return playerState; }
    SDL_FRect GetBoundingBox() const;
    std::vector<std::unique_ptr<Projectile>>& GetProjectiles() { return projectiles; }
    virtual float GetSpriteWidth() const override { return 32.0f; }

    // ===== SETTERS =====
    void SetShootCooldown(float cooldown) { shootCooldown = cooldown; }
    void SetProjectileDamage(int damage) { projectileDamage = damage; }
    void SetProjectileSpeed(float speed) { projectileSpeed = speed; }
    void SetArrowSpawnFrame(int frame) { arrowSpawnFrame = frame; }

    //bool IsInvulnerable() const { return isInvulnerable; }
};