#include "Boss.h"
#include <iostream>
#include <cmath>

Boss::Boss(SDL_Renderer* renderer, glm::vec2 startPos)
    : Enemy(),
    currentPhase(BossPhase::PHASE_1),
    hasEnteredPhase2(false),
    hasEnteredPhase3(false),
    isUsingUltimate(false),
    ultimateCooldown(15.0f),
    ultimateTimer(0.0f),
    ultimateDamage(80),
    ultimateRadius(150.0f),
    canSummon(true),
    summonCount(0),
    maxSummons(6),
    summonCooldown(20.0f),
    summonTimer(0.0f),
    isCharging(false),
    chargeSpeed(300.0f),
    chargeDirection(0.0f, 0.0f),
    chargeDuration(1.5f),
    chargeTimer(0.0f),
    isSlamming(false),
    slamRadius(120.0f),
    slamDamage(50),
    hasIntroPlayed(false),
    isInvulnerable(true),
    introTimer(3.0f)
{
    this->renderer = renderer;
    this->position = startPos;
    // Lưu vị trí ban đầu để ResetToStartPosition có thể trả về đúng vị trí
    this->initialPosition = startPos;
    this->initialPatrolPointA = startPos - glm::vec2(100.0f, 0.0f);
    this->initialPatrolPointB = startPos + glm::vec2(100.0f, 0.0f);
    this->enemyType = EnemyType::BOSS;

    maxHealth = 800;
    health = 800;
    attackDamage = 40;
    attackCooldown = 2.0f;
    attackRange = 80.0f;
    runSpeed = 80.0f;
    aggroRange = 500.0f;

    // Load textures
    std::string folderPath = "assets/images/Bosses/";
    LoadTexture(renderer, &idleTex, (folderPath + "Idle.png").c_str());
    LoadTexture(renderer, &walkTex, (folderPath + "Walk.png").c_str());
    LoadTexture(renderer, &runTex, (folderPath + "Run.png").c_str());
    LoadTexture(renderer, &jumpTex, (folderPath + "Jump.png").c_str());
    LoadTexture(renderer, &attackTex, (folderPath + "Attack.png").c_str());
    LoadTexture(renderer, &hurtTex, (folderPath + "Hurt.png").c_str());
    LoadTexture(renderer, &deadTex, (folderPath + "Dead.png").c_str());

    // Size: 96x96 (lớn nhất)
    renderWidth = 96.0f;
    renderHeight = 96.0f;
}

Boss::~Boss() {
}

// === TRẢ FRAMES TỪ GAMECONSTANTS ===
FrameConfig Boss::GetFrameConfig(EnemyState state) const {
    FrameConfig cfg = { 0, 0.1f, 700, 600 };

    switch (state) {
    case EnemyState::STATE_IDLE:
        cfg = { GameConstants::BOSS_IDLE_FRAMES, GameConstants::BOSS_IDLE_FRAME_DURATION,
               GameConstants::BOSS_IDLE_FRAME_WIDTH, GameConstants::BOSS_IDLE_FRAME_HEIGHT };
        break;
    case EnemyState::STATE_WALK:
        cfg = { GameConstants::BOSS_WALK_FRAMES, GameConstants::BOSS_WALK_FRAME_DURATION,
               GameConstants::BOSS_WALK_FRAME_WIDTH, GameConstants::BOSS_WALK_FRAME_HEIGHT };
        break;
    case EnemyState::STATE_RUN:
        cfg = { GameConstants::BOSS_RUN_FRAMES, GameConstants::BOSS_RUN_FRAME_DURATION,
               GameConstants::BOSS_RUN_FRAME_WIDTH, GameConstants::BOSS_RUN_FRAME_HEIGHT };
        break;
    case EnemyState::STATE_JUMP:
        cfg = { GameConstants::BOSS_JUMP_FRAMES, GameConstants::BOSS_JUMP_FRAME_DURATION,
               GameConstants::BOSS_JUMP_FRAME_WIDTH, GameConstants::BOSS_JUMP_FRAME_HEIGHT };
        break;
    case EnemyState::STATE_ATTACK:
        cfg = { GameConstants::BOSS_ATTACK_FRAMES, GameConstants::BOSS_ATTACK_FRAME_DURATION,
               GameConstants::BOSS_ATTACK_FRAME_WIDTH, GameConstants::BOSS_ATTACK_FRAME_HEIGHT };
        break;
    case EnemyState::STATE_HURT:
        cfg = { GameConstants::BOSS_HURT_FRAMES, GameConstants::BOSS_HURT_FRAME_DURATION,
               GameConstants::BOSS_HURT_FRAME_WIDTH, GameConstants::BOSS_HURT_FRAME_HEIGHT };
        break;
    case EnemyState::STATE_DEAD:
        cfg = { GameConstants::BOSS_DEAD_FRAMES, GameConstants::BOSS_DEAD_FRAME_DURATION,
               GameConstants::BOSS_DEAD_FRAME_WIDTH, GameConstants::BOSS_DEAD_FRAME_HEIGHT };
        break;
    }

    return cfg;
}

void Boss::PlayIntro() {
    std::cout << "=== BOSS XUAT HIEN ===\n";
}

void Boss::TriggerIntro() {
    if (!hasIntroPlayed) {
        PlayIntro();
        enemyState = EnemyState::STATE_IDLE;
    }
}

void Boss::CheckPhaseTransition() {
    float healthPercent = (float)health / (float)maxHealth;

    if (!hasEnteredPhase2 && healthPercent <= 0.7f) {
        EnterPhase2();
    }

    if (!hasEnteredPhase3 && healthPercent <= 0.4f) {
        EnterPhase3();
    }
}

void Boss::EnterPhase2() {
    hasEnteredPhase2 = true;
    currentPhase = BossPhase::PHASE_2;

    attackDamage = (int)(attackDamage * 1.3f);
    runSpeed *= 1.2f;
    attackCooldown *= 0.8f;

    std::cout << "=== BOSS ENTERED PHASE 2 (70% HP) ===\n";
    // LOẠI BỎ SummonMinions()
}

void Boss::EnterPhase3() {
    hasEnteredPhase3 = true;
    currentPhase = BossPhase::PHASE_3;

    attackDamage = (int)(attackDamage * 1.5f);
    runSpeed *= 1.4f;
    attackCooldown *= 0.6f;
    ultimateCooldown *= 0.7f;

    std::cout << "=== BOSS ENTERED PHASE 3 (40% HP) - ENRAGED! ===\n";
    // LOẠI BỎ SummonMinions()
}

void Boss::SummonMinions() {
    if (summonCount >= maxSummons || summonTimer > 0) return;

    summonTimer = summonCooldown;
    summonCount += 2;
}

void Boss::ChargeAttack() {
    if (isCharging) return;

    isCharging = true;
    chargeTimer = chargeDuration;
    chargeDirection = glm::normalize(targetPosition - position);
}

void Boss::GroundSlam() {
    if (isSlamming) return;

    isSlamming = true;

    float distToTarget = GetDistanceToTarget();
    if (distToTarget <= slamRadius) {
        std::cout << "Boss dap dat! Damage: " << slamDamage << "\n";
    }

    isSlamming = false;
}

void Boss::UseUltimate() {
    if (ultimateTimer > 0) return;

    isUsingUltimate = true;
    ultimateTimer = ultimateCooldown;

    float distToTarget = GetDistanceToTarget();
    if (distToTarget <= ultimateRadius) {
        std::cout << "Target nhan " << ultimateDamage << " damage!\n";
    }

    isUsingUltimate = false;
}

int Boss::PerformAttack() {
    if (currentPhase == BossPhase::PHASE_3 && ultimateTimer <= 0) {
        if ((rand() % 100) < 40) {
            UseUltimate();
            return ultimateDamage;
        }
    }

    if (currentPhase == BossPhase::PHASE_2 && (rand() % 100) < 30) {
        ChargeAttack();
        return attackDamage;
    }

    if (IsTargetInAttackRange()) {
        if ((rand() % 100) < 50) {
            GroundSlam();
            return slamDamage;
        }
        else {
            return attackDamage;
        }
    }

    return 0;
}

void Boss::Update(float deltaTime, Map& map) {
    if (!hasIntroPlayed) {
        introTimer -= deltaTime;
        if (introTimer <= 0) {
            hasIntroPlayed = true;
            isInvulnerable = false;
            enemyState = EnemyState::STATE_IDLE;
        }
        return;
    }

    CheckPhaseTransition();

    if (isCharging) {
        chargeTimer -= deltaTime;
        velocity = chargeDirection * chargeSpeed;

        if (chargeTimer <= 0) {
            isCharging = false;
            velocity.x = 0.0f;
        }
    }

    if (ultimateTimer > 0) ultimateTimer -= deltaTime;
    // LOẠI BỎ summonTimer

    Enemy::Update(deltaTime, map);
}

void Boss::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    Enemy::Render(renderer, cameraOffset);
}

void Boss::TakeDamage(int damage) {
    if (isInvulnerable) {
        return;
    }

    if (currentPhase == BossPhase::PHASE_3) {
        damage = (int)(damage * 0.8f);
    }

    Enemy::TakeDamage(damage);
}