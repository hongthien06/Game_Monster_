#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include "../Config/GameConstants.h"
#include <iostream>
#include <vector>

 // Enum trạng thái chung cho tất cả enemy
enum class EnemyState {
    STATE_IDLE,       
    STATE_PATROL,
    STATE_CHASE,     
    STATE_ATTACK,   
    STATE_HURT,    
    STATE_DEAD    
};

// Loại enemy (để phân biệt và xử lý khác nhau)
enum class EnemyType {
    TYPE_MINION,      
    TYPE_ELITE,       
    TYPE_BOSS         
};

class Enemy {
protected:
    //  TEXTURE VÀ HOẠT ẢNH 
    SDL_Texture* idleTex;     
    SDL_Texture* walkTex;     
    SDL_Texture* attackTex;     
    SDL_Texture* hurtTex;   
    SDL_Texture* deadTex;     

    int currentFrame;         
    float animationTimer;      

    // Thông số sprite (sẽ được set bởi lớp con)
    int frameWidth;
    int frameHeight;
    int idleFrameCount;
    int walkFrameCount;
    int attackFrameCount;
    int hurtFrameCount;
    int deadFrameCount;

    //  VỊ TRÍ VÀ DI CHUYỂN 
    glm::vec2 position;      
    glm::vec2 velocity;       
    bool facingRight;        
    bool isOnGround;          

    //  THUỘC TÍNH COMBAT 
    int health;              
    int maxHealth;              
    int attackDamage;         
    float attackRange;        
    float attackCooldown;       
    float attackTimer;     

    bool isAlive;             
    float hurtDuration;      
    float hurtTimer;       

    //  AI VÀ HÀNH VI 
    EnemyState currentState;   
    EnemyState previousState;   
    EnemyType enemyType;       

    float detectionRange;      
    float patrolSpeed;        
    float chaseSpeed;       

    // Tuần tra
    glm::vec2 patrolPointA; 
    glm::vec2 patrolPointB;    
    glm::vec2 currentTarget; 
    bool movingToB;            

    // Reference tới player (để tính khoảng cách và đuổi theo)
    glm::vec2* playerPosition;  

    void UpdateAnimation(float deltaTime);

    void UpdateAI(float deltaTime);

    void UpdatePhysics(float deltaTime);

    bool CheckPlayerInRange();

    void MoveTowards(glm::vec2 target, float speed, float deltaTime);

public:
    Enemy();
    Enemy(SDL_Renderer* renderer, glm::vec2 startPos, EnemyType type,
        glm::vec2 patrol_A, glm::vec2 patrol_B);

    virtual ~Enemy();
    virtual void Update(float deltaTime);

    //Vẽ enemy lên màn hình
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset);

    // ===== COMBAT METHODS =====
    void TakeDamage(int damage);
     
    virtual void Attack();

    virtual void Die();

    //  GETTERS/SETTERS 

    void SetPlayerPosition(glm::vec2* playerPos) { playerPosition = playerPos; }
    glm::vec2 GetPosition() const { return position; }
    void SetPosition(const glm::vec2& pos) { position = pos; }
    bool IsAlive() const { return isAlive; }
    int GetHealth() const { return health; }
    int GetMaxHealth() const { return maxHealth; }
    EnemyState GetState() const { return currentState; }
    EnemyType GetType() const { return enemyType; }
    int GetAttackDamage() const { return attackDamage; }
    SDL_FRect GetBoundingBox() const;
};