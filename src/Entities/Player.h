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
    STATE_SHOT,
    STATE_ATTACK,
    STATE_HURT,
    STATE_DEAD
};

class Player : public Character {
private:
    // ===== TEXTURES BỔ SUNG =====
    SDL_Texture* shotTex;
    SDL_Texture* attackTex;
    SDL_Texture* hurtTex;
    SDL_Texture* deadTex;

    // ===== TRẠNG THÁI VÀ ANIMATION =====
    PlayerState playerState;
    PlayerState previousPlayerState;
    int playerCurrentFrame;
    float playerAnimationTimer;

    // ===== COMBAT =====
   /* int health;
    int maxHealth;*/
    bool isAlive;
    bool isAttacking;
    bool canMove;
    float attackCooldown;
    float attackTimer;
    float hurtDuration;
    float hurtTimer;

    // ====== AUDIO ======  Them vao de khoi tao doi tuong lop Audio
    Audio audio;

    // ===== SHOOTING SYSTEM =====
    std::vector<std::unique_ptr<Projectile>> projectiles;
    SDL_Renderer* renderer;       // Lưu renderer để tạo projectile
    float shootCooldown;
    float shootTimer;
    int projectileDamage;
    float projectileSpeed;
    glm::vec2 mouseWorldPos;      // Vị trí chuột trong world

    // THÊM MỚI: Track khi nào spawn arrow
    bool shouldSpawnArrow;        // Cờ để spawn arrow
    int arrowSpawnFrame;          // Frame nào thì spawn arrow

    // ===== PRIVATE METHODS =====
    void UpdatePlayerAnimation(float deltaTime);
    void UpdatePlayerState(float deltaTime);
    void HandleInput();
    void LoadAllTextures(SDL_Renderer* renderer);
    void UpdateProjectiles(float deltaTime);
    glm::vec2 GetArrowSpawnPosition() const;
    void SpawnArrow();           

public:
    // ===== CONSTRUCTOR / DESTRUCTOR =====
    Player(SDL_Renderer* renderer, glm::vec2 startPos = glm::vec2(50.0f, 0.0f));
    virtual ~Player();

    // ===== CORE METHODS =====
    virtual void Update(float deltaTime, Map& map) override;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;

    // ===== COMBAT METHODS =====
    void TakeDamage(int damage);
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

    float GetSpriteWidth() const { return 32.0f; }

    // ===== SETTERS =====
    void SetShootCooldown(float cooldown) { shootCooldown = cooldown; }
    void SetProjectileDamage(int damage) { projectileDamage = damage; }
    void SetProjectileSpeed(float speed) { projectileSpeed = speed; }
    void SetArrowSpawnFrame(int frame) { arrowSpawnFrame = frame; }  
};