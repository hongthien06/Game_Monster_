#pragma once

#include <vector>
#include <memory>
#include "../Core/Audio.h"
#include "Character.h"
#include "Projectile.h"

// Forward declaration
class ScreenTransition;

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

// ===== THÊM MỚI: ENUM CHO DEATH STATE =====
enum class DeathTransitionState {
    NONE,           // Không có transition
    FADING_OUT,     // Đang fade to black
    RESPAWNING,     // Đang respawn (màn hình đen)
    FADING_IN       // Đang fade từ đen sang sáng
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

    // ===== LIVES SYSTEM - HỆ THỐNG TRÁI TIM =====
    int lives;              // Số mạng còn lại
    int maxLives;           // Số mạng tối đa
    glm::vec2 respawnPoint; // Điểm hồi sinh
    bool isDying;           // Đang trong trạng thái chết
    float deathTimer;       // Timer để delay respawn
    float deathDelay;
    bool isGameOver;        // Đã hết mạng - Game Over

    // ===== THÊM MỚI: DEATH TRANSITION =====
    DeathTransitionState deathTransitionState;
    std::unique_ptr<ScreenTransition> deathTransition;
    float respawnFlashDuration;     // Thời gian nhấp nháy sau respawn
    float respawnFlashTimer;
    bool isRespawnFlashing;

    // ===== INVENTORY SYSTEM - HỆ THỐNG TÚI ĐỒ =====
    int healthPotionCount;  // Số lượng health potion trong túi
    int maxHealthPotions;   // Số lượng tối đa có thể mang

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
    void UpdatePlayerState(float deltaTime, bool wasOnGroundOld);
    void HandleInput();
    void UpdateProjectiles(float deltaTime);
    glm::vec2 GetArrowSpawnPosition() const;
    void SpawnArrow();
    void Respawn();         // Hồi sinh
    void LoseLife();
    void UpdateDeathTransition(float deltaTime);  // ===== THÊM MỚI =====

    // THÊM MỚI: Knockback & I-frames
    bool isInvulnerable;
    float invulnerabilityDuration;
    float invulnerabilityTimer;

    glm::vec2 knockbackVelocity;
    float knockbackDecay;
    bool isKnockedBack;

    SDL_Renderer* renderer;  // ===== THÊM MỚI: Lưu renderer =====
    bool needsCameraSnap;

public:
    // ===== CONSTRUCTOR / DESTRUCTOR =====
    Player(SDL_Renderer* renderer, glm::vec2 startPos = glm::vec2(50.0f, 0.0f));
    virtual ~Player();

    // ===== CORE METHODS =====
    virtual void Update(float deltaTime, Map& map) override;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;

    void RenderDeathTransition(SDL_Renderer* renderer);

    // ===== COMBAT METHODS =====
    // FIX: OVERRIDE TakeDamage VỚI I-FRAMES
    virtual void TakeDamage(int damage) override;

    bool IsInvulnerable() const { return iFramesTimer > 0; }

    void Attack();
    void Shot();
    void Heal(int amount);

    // ===== INVENTORY METHODS =====
    void UseHealthPotion();                      // Sử dụng potion từ túi (phím H)
    void AddHealthPotion();                      // Thêm potion vào túi khi nhặt
    int GetHealthPotionCount() const { return healthPotionCount; }
    int GetMaxHealthPotions() const { return maxHealthPotions; }


    // ===== LIVES METHODS =====
    void CheckFallDeath();      // Kiểm tra rơi khỏi map
    int GetLives() const { return lives; }
    int GetMaxLives() const { return maxLives; }
    void AddLife();             // Thêm 1 mạng
    void SetRespawnPoint(glm::vec2 point) { respawnPoint = point; }
    bool IsGameOver() const { return isGameOver; }  // Kiểm tra game over
    void ResetGame();           // Reset game để chơi lại

    // ===== MOUSE/SHOOTING =====
    void HandleMouseClick(float mouseScreenX, float mouseScreenY, glm::vec2 cameraOffset);
    void UpdateMousePosition(float mouseScreenX, float mouseScreenY, glm::vec2 cameraOffset);

    // ===== GETTERS =====
    bool IsAlive() const { return isAlive; }
    PlayerState GetPlayerState() const { return playerState; }
    bool IsInDeathTransition() const { return deathTransitionState != DeathTransitionState::NONE; }  // ===== THÊM MỚI =====
    SDL_FRect GetBoundingBox() const;
    std::vector<std::unique_ptr<Projectile>>& GetProjectiles() { return projectiles; }
    virtual float GetSpriteWidth() const override { return 32.0f; }
    // Trong phần getters
    bool NeedsCameraSnap() const { return needsCameraSnap; }
    void ClearCameraSnapFlag() { needsCameraSnap = false; }

    // ===== SETTERS =====
    void SetShootCooldown(float cooldown) { shootCooldown = cooldown; }
    void SetProjectileDamage(int damage) { projectileDamage = damage; }
    void SetProjectileSpeed(float speed) { projectileSpeed = speed; }
    void SetArrowSpawnFrame(int frame) { arrowSpawnFrame = frame; }

    //bool IsInvulnerable() const { return isInvulnerable; }
    void Reset(glm::vec2 startPos);
    void SnapToGround(Map& map);

    void SetPosition(glm::vec2 pos) { position = pos; }
};