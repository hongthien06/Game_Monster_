#include "Boss.h"
#include "../Config/GameConstants.h"
#include <iostream>
#include <cmath>

// LẤY HITBOX VŨ KHÍ BOSS 
SDL_FRect Boss::GetWeaponHitbox() {
    SDL_FRect weaponBox = { 0.0f, 0.0f, 0.0f, 0.0f };
    int currentFrame = this->enemyCurrentFrame;
    bool facingLeft = this->flipHorizontal;

    // CHỈ KÍCH HOẠT HITBOX KHI ĐANG ATTACK VÀ ĐÚNG FRAME GÂY SÁT THƯƠNG
    if (enemyState == EnemyState::STATE_ATTACK) {
        // Frame 5-7: Giai đoạn tay vung xuống (gây sát thương)
        if (currentFrame >= 5 && currentFrame <= 7) {

            // Tăng kích thước hitbox để dễ trúng Player
            weaponBox.w = 60.0f;
            weaponBox.h = 90.0f;

            // Hạ vị trí Y xuống sát đất (ngang tầm Player)
            weaponBox.y = position.y + hitboxOffsetY + 50.0f;

            // Đặt hitbox trước mặt Boss
            if (!facingLeft) {
                // Boss nhìn phải → Hitbox ở bên phải
                weaponBox.x = position.x + hitboxOffsetX + hitboxWidth + 5.0f;
            }
            else {
                // Boss nhìn trái → Hitbox ở bên trái
                weaponBox.x = position.x + hitboxOffsetX - weaponBox.w - 5.0f;
            }
        }
    }

    return weaponBox;
}

// ==============================================================
// CONSTRUCTOR & SETUP
// ==============================================================

Boss::Boss(SDL_Renderer* renderer, glm::vec2 startPos)
    : Enemy(),
    currentPhase(BossPhase::PHASE_1),
    hasEnteredPhase2(false),
    hasEnteredPhase3(false),
    isUsingUltimate(false),
    ultimateCooldown(30.0f),
    ultimateTimer(0.0f),
    ultimateDamage(27),
    ultimateRadius(50.0f),
    canSummon(true),
    summonCount(0),
    maxSummons(6),
    summonCooldown(20.0f),
    summonTimer(0.0f),
    isCharging(false),
    chargeSpeed(90.0f),
    chargeDirection(0.0f, 0.0f),
    chargeDuration(1.5f),
    chargeTimer(0.0f),
    isSlamming(false),
    slamRadius(120.0f),
    slamDamage(50),
    hasIntroPlayed(false),
    isInvulnerable(true),
    introTimer(3.0f),
    effectManager(nullptr)
{
    this->renderer = renderer;
    this->position = startPos;
    this->initialPosition = startPos;
    this->initialPatrolPointA = startPos - glm::vec2(100.0f, 0.0f);
    this->initialPatrolPointB = startPos + glm::vec2(100.0f, 0.0f);
    this->enemyType = EnemyType::BOSS;

    // Thiết lập stats
    maxHealth = 700;
    health = 700;
    attackDamage = 30;
    attackCooldown = 2.0f;
    attackRange = 75.0f;
    runSpeed = 100.0f;
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

    // Thiết lập kích thước render và hitbox
    renderWidth = GameConstants::BOSS_RENDER_WIDTH;
    renderHeight = GameConstants::BOSS_RENDER_HEIGHT;
    hitboxWidth = GameConstants::BOSS_HITBOX_WIDTH;
    hitboxHeight = GameConstants::BOSS_HITBOX_HEIGHT;
    hitboxOffsetX = GameConstants::BOSS_HITBOX_OFFSET_X;
    hitboxOffsetY = GameConstants::BOSS_HITBOX_OFFSET_Y;
}
// HUỶ BOSS
Boss::~Boss() {
}

// === TRẢ FRAMES TỪ GAMECONSTANTS ===
FrameConfig Boss::GetFrameConfig(EnemyState state) const {
    FrameConfig cfg = { 0, 0.1f, 700, 626 };

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
// PHÁT INTRO BOSS (HIỆU ỨNG XUẤT HIỆN)
void Boss::PlayIntro() {
    std::cout << "=== BOSS XUAT HIEN ===\n";

    // Boss xuất hiện với hiệu ứng fade từ trong ra
    if (effectManager) {

        effectManager->TriggerScreenShake(3.0f, 0.5f);
    }
}

void Boss::TriggerIntro() {
    if (!hasIntroPlayed) {
        PlayIntro();
        enemyState = EnemyState::STATE_IDLE;
    }
}


// PHASE LOGIC

// KIỂM TRA VÀ CHUYỂN PHASE THEO MÁU
void Boss::CheckPhaseTransition() {
    float healthPercent = (float)health / (float)maxHealth;

    if (!hasEnteredPhase2 && healthPercent <= 0.7f) {
        EnterPhase2();
    }

    if (!hasEnteredPhase3 && healthPercent <= 0.4f) {
        EnterPhase3();
    }
}
// VÀO PHASE 2 (70% HP)
void Boss::EnterPhase2() {
    hasEnteredPhase2 = true;
    currentPhase = BossPhase::PHASE_2;
    // Tăng sức mạnh
    attackDamage = (int)(attackDamage * 1.3f);
    runSpeed *= 1.2f;
    attackCooldown *= 0.8f;

    // Hiệu ứng màn hình
    if (effectManager) {
        effectManager->CreateFlash(
            position,
            { 255, 150, 0, 220 },  // Cam đậm
            0.6f,
            0.3f
        );
        effectManager->TriggerScreenShake(10.0f, 0.5f);
    }
}
// VÀO PHASE 3 (40% HP - NỔI ĐIÊN)
void Boss::EnterPhase3() {
    hasEnteredPhase3 = true;
    currentPhase = BossPhase::PHASE_3;

    attackDamage = (int)(attackDamage * 1.5f);
    runSpeed *= 1.4f;
    attackCooldown *= 0.6f;
    ultimateCooldown *= 0.7f;

    if (effectManager) {
        effectManager->CreateFlash(
            position,
            { 255, 0, 0, 240 },  // Đỏ rất đậm
            0.8f,    // Intensity rất cao
            0.5f     // Duration dài
        );
        effectManager->TriggerScreenShake(15.0f, 0.8f);
    }
}

// ==============================================================
// ATTACK SKILLS
// ==============================================================
// TRIỆU HỒI MINIONS
void Boss::SummonMinions() {
    if (summonCount >= maxSummons || summonTimer > 0) return;

    summonTimer = summonCooldown;
    summonCount += 2;
}
// LAO TỚI (CHARGE ATTACK)
void Boss::ChargeAttack() {
    if (isCharging) return;

    isCharging = true;
    chargeTimer = chargeDuration;
    chargeDirection = glm::normalize(targetPosition - position);

    if (effectManager) {
        effectManager->CreateFlash(
            position,
            { 255, 255, 200, 100 },  // Vàng nhạt
            0.3f,
            0.15f
        );
    }
}

void Boss::GroundSlam() {
    // 1. Nếu đang dậm rồi thì thoát, tránh spam chiêu liên tục
    if (isSlamming) return;

    // 2. Bật cờ hiệu dậm đất (để hàm GetWeaponHitbox biết mà vẽ hitbox to)
    isSlamming = true;

    enemyState = EnemyState::STATE_ATTACK;
    enemyCurrentFrame = 0;       // Đưa về frame đầu tiên của animation đánh
    enemyAnimationTimer = 0.0f;  // Reset bộ đếm thời gian frame

    // 3. Debug log (Kiểm tra khoảng cách để xem có trúng không - chỉ để test)
    float distToTarget = GetDistanceToTarget();
    if (distToTarget <= slamRadius) {
        std::cout << "Boss bat dau Ground Slam! (Hitbox se kich hoat o frame 5-7)\n";
    }

    // 4. Kích hoạt Hiệu ứng (Visual & Sound)
    if (effectManager) {
        // Tạo chớp sáng màu vàng đậm (Gold/Orange) báo hiệu nguy hiểm
        effectManager->CreateFlash(
            position,
            { 255, 200, 0, 180 },  // R:255, G:200, B:0 (Vàng cam), Alpha: 180
            0.5f,    // Intensity (Độ sáng)
            0.2f     // Duration (Thời gian tồn tại)
        );

        // Rung màn hình để tạo cảm giác va đập mạnh
        effectManager->TriggerScreenShake(8.0f, 0.5f);
    }


}
// CHIÊU CUỐI (ULTIMATE ATTACK)
void Boss::UseUltimate() {
    if (ultimateTimer > 0) return;

    isUsingUltimate = true;
    ultimateTimer = ultimateCooldown;

    float distToTarget = GetDistanceToTarget();

    if (effectManager) {
        // Flash đỏ toàn màn hình
        effectManager->CreateFlash(
            position,
            { 255, 50, 50, 200 },  // Đỏ đậm
            0.7f,    // Intensity rất cao
            0.4f     // Duration dài
        );

        // Rung màn hình mạnh
        effectManager->TriggerScreenShake(12.0f, 0.6f);
    }
}
// THỰC HIỆN ĐÁNH (RANDOM SKILL)
int Boss::PerformAttack() {
    // Phase 3: Ưu tiên Ultimate
    if (currentPhase == BossPhase::PHASE_3 && ultimateTimer <= 0) {
        if ((rand() % 100) < 40) {
            UseUltimate();
            return ultimateDamage;
        }
    }
    // Phase 2: Random Charge
    if (currentPhase == BossPhase::PHASE_2 && (rand() % 100) < 30) {
        ChargeAttack();
        return attackDamage;
    }
    // Trong tầm đánh: Ground Slam hoặc đánh thường
    if (IsTargetInAttackRange()) {
        if ((rand() % 100) < 50) {
            GroundSlam();
            return slamDamage;
        }
        else {
            if (effectManager) {
                effectManager->CreateFlash(
                    position,
                    { 255, 200, 100, 80 },  // Vàng nhạt
                    0.15f,
                    0.08f
                );
            }
            return attackDamage;
        }
    }

    return 0;
}

// ==============================================================
// UPDATE & RENDER
// ==============================================================
// CẬP NHẬT BOSS MỖI FRAME
void Boss::Update(float deltaTime, Map& map) {
    // Xử lý intro
    if (!hasIntroPlayed) {
        introTimer -= deltaTime;
        if (introTimer <= 0) {
            hasIntroPlayed = true;
            isInvulnerable = false;
            enemyState = EnemyState::STATE_IDLE;
        }
        return;
    }
    // Kiểm tra chuyển phase
    CheckPhaseTransition();

    // Reset trạng thái skill khi không còn đánh
    if (enemyState != EnemyState::STATE_ATTACK) {
        isSlamming = false;
        isUsingUltimate = false;
    }

    // Nếu đang STATE_ATTACK nhưng Player quá xa → Đuổi theo trước
    if (enemyState == EnemyState::STATE_ATTACK) {
        float distToPlayer = GetDistanceToTarget();

        // Nếu Player ngoài tầm đánh → Chuyển sang RUN
        if (distToPlayer > attackRange) {
            enemyState = EnemyState::STATE_RUN;
            attackTimer = attackCooldown * 0.5f; // Reset cooldown ngắn hơn
        }
        else {
            // Đã vào tầm → Đứng yên để đánh
            velocity.x = 0.0f;
        }
    }

    // Xử lý Charge Attack
    if (isCharging) {
        chargeTimer -= deltaTime;
        velocity = chargeDirection * chargeSpeed;

        if (chargeTimer <= 0) {
            isCharging = false;
            velocity.x = 0.0f;
        }
    }

    if (ultimateTimer > 0) ultimateTimer -= deltaTime;

    Enemy::Update(deltaTime, map);
}

void Boss::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    // Vẽ healthbar
    if (isAlive) {
        float bossWidth = renderWidth;
        float barWidth = 150.0f;
        float barX = position.x - cameraOffset.x + (bossWidth / 2.0f) - (barWidth / 2.0f);
        float barY = position.y - cameraOffset.y - 15.0f;
        float barHeight = 8.0f;

        SDL_FRect bgRect = { barX, barY, barWidth, barHeight };
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 220);
        SDL_RenderFillRect(renderer, &bgRect);

        float healthPercent = (float)health / (float)maxHealth;

        SDL_Color barColor;
        if (healthPercent > 0.6f) {
            barColor = { 231, 76, 60, 255 };
        }
        else if (healthPercent > 0.3f) {
            barColor = { 241, 196, 15, 255 };
        }
        else {
            barColor = { 192, 57, 43, 255 };
        }

        SDL_FRect hpRect = {
            barX + 2,
            barY + 2,
            (barWidth - 4) * healthPercent,
            barHeight - 4
        };
        SDL_SetRenderDrawColor(renderer, barColor.r, barColor.g, barColor.b, barColor.a);
        SDL_RenderFillRect(renderer, &hpRect);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
        SDL_RenderRect(renderer, &bgRect);
    }

    // Vẽ sprite
    SDL_Texture* currentTexture = nullptr;
    FrameConfig cfg = GetFrameConfig(enemyState);

    switch (enemyState) {
    case EnemyState::STATE_IDLE: currentTexture = idleTex; break;
    case EnemyState::STATE_WALK: currentTexture = walkTex; break;
    case EnemyState::STATE_RUN: currentTexture = runTex; break;
    case EnemyState::STATE_JUMP: currentTexture = jumpTex; break;
    case EnemyState::STATE_ATTACK: currentTexture = attackTex; break;
    case EnemyState::STATE_HURT: currentTexture = hurtTex; break;
    case EnemyState::STATE_DEAD: currentTexture = deadTex; break;
    }

    if (!currentTexture) return;

    int safeFrame = std::min(enemyCurrentFrame, cfg.frameCount - 1);

    SDL_FRect srcRect = {
        (float)(safeFrame * cfg.frameWidth),
        0.0f,
        (float)cfg.frameWidth,
        (float)cfg.frameHeight
    };

    SDL_FRect dstRect = {
        position.x - cameraOffset.x,
        position.y - cameraOffset.y,
        renderWidth,
        renderHeight
    };

    SDL_RenderTextureRotated(
        renderer,
        currentTexture,
        &srcRect,
        &dstRect,
        0.0,
        nullptr,
        flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE
    );
}
// NHẬN SÁT THƯƠNG (BOSS KHÔNG BỊ CHOÁNG - SUPER ARMOR)
void Boss::TakeDamage(int damage) {
    if (isInvulnerable) {
        return;
    }

    if (currentPhase == BossPhase::PHASE_3) {
        damage = (int)(damage * 0.8f);
    }

    // Hiệu ứng lóe sáng
    if (effectManager) {
        effectManager->CreateFlash(position, { 255, 255, 255, 120 }, 0.2f, 0.08f);
    }

    // 1. Trừ máu
    health -= damage;

    if (health <= 0) {
        health = 0;
        Die();
        return;
    }

    // Nếu Boss đang đi dạo hoặc đứng yên mà bị đánh -> Chuyển sang chạy đuổi ngay
    if (enemyState == EnemyState::STATE_IDLE || enemyState == EnemyState::STATE_WALK) {
        enemyState = EnemyState::STATE_RUN;
    }

    if (GetDistanceToTarget() > aggroRange) {
        aggroRange = 1500.0f; // Tăng tầm aggro lên vô hạn (hoặc rất xa)
    }

    // Quay mặt về hướng người chơi (để không bị đánh lén sau lưng mà vẫn chạy tới trước)
    float diffX = targetPosition.x - position.x;
    if (std::abs(diffX) > 10.0f) {
        flipHorizontal = (diffX < 0);
    }

    // Enrage: Giảm hồi chiêu nếu bị đánh (để đánh trả nhanh hơn)
    if (attackTimer > 0.5f) {
        attackTimer = 0.5f;
    }
}