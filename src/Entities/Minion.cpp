#include "Minion.h"
#include <iostream>

// ===== CONSTRUCTOR - PHÂN BIỆT 3 LOẠI ORC =====
Minions::Minions(SDL_Renderer* renderer, glm::vec2 startPos, MinionType type)
    : Enemy(renderer, startPos,
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr,
        EnemyType::MINION),
    minionType(type),
    magicTex(nullptr),
    isCastingMagic(false),
    magicCooldown(2.0f),
    magicTimer(0.0f),
    isRaging(false),
    rageThreshold(0.3f)
{
    // ===== THIẾT LẬP THÔNG SỐ CHO TỪNG LOẠI ORC =====
    std::string folderPath;

    switch (type) {
    case MinionType::ORC_BERSERK:
        // Orc Berserk - Tấn công nhanh, dữ dội
        folderPath = "assets/images/Minions/Orc_Berserk/";
        maxHealth = 40;
        health = 40;
        attackDamage = 15;
        attackCooldown = 0.8f;  // Tấn công nhanh
        chaseSpeed = 120.0f;
        detectionRange = 180.0f;
        std::cout << "[Orc Berserk] Khoi tao - HP: 40, Damage: 15, Attack Speed: NHANH\n";
        break;

    case MinionType::ORC_SHAMAN:
        // Orc Shaman - Tấn công từ xa bằng magic
        folderPath = "assets/images/Minions/Orc_Shaman/";
        maxHealth = 35;
        health = 35;
        attackDamage = 12;
        attackCooldown = 1.5f;
        attackRange = 150.0f;   // Tấn công từ xa
        chaseSpeed = 80.0f;     // Di chuyển chậm
        detectionRange = 200.0f;
        std::cout << "[Orc Shaman] Khoi tao - HP: 35, Damage: 12, MAGIC ATTACK\n";
        break;

    case MinionType::ORC_WARRIOR:
        // Orc Warrior - Cân bằng
        folderPath = "assets/images/Minions/Orc_Warrior/";
        maxHealth = 50;
        health = 50;
        attackDamage = 12;
        attackCooldown = 1.2f;
        chaseSpeed = 90.0f;
        detectionRange = 160.0f;
        std::cout << "[Orc Warrior] Khoi tao - HP: 50, Damage: 12, CAN BANG\n";
        break;

    default:
        std::cerr << "[Minion] Loai Orc khong hop le!\n";
        return;
    }

    // ===== LOAD TEXTURE ĐÚNG CHO TỪNG LOẠI ORC =====
    std::cout << "Loading textures tu: " << folderPath << "\n";

    LoadTexture(renderer, &idleTex, (folderPath + "Idle.png").c_str());
    LoadTexture(renderer, &walkTex, (folderPath + "Walk.png").c_str());
    LoadTexture(renderer, &runTex, (folderPath + "Run.png").c_str());
    LoadTexture(renderer, &jumpTex, (folderPath + "Jump.png").c_str());
    LoadTexture(renderer, &attackTex, (folderPath + "Attack.png").c_str());
    LoadTexture(renderer, &hurtTex, (folderPath + "Hurt.png").c_str());
    LoadTexture(renderer, &deadTex, (folderPath + "Dead.png").c_str());

    // ===== SHAMAN CÓ THÊM MAGIC TEXTURE =====
    if (type == MinionType::ORC_SHAMAN) {
        LoadTexture(renderer, &magicTex, (folderPath + "Magic.png").c_str());
        std::cout << "[Orc Shaman] Load them Magic textures\n";
    }
}

// ===== DESTRUCTOR =====
Minions::~Minions() {
    SDL_DestroyTexture(magicTex);
}

// ===== KÍCH HOẠT RAGE (CHỈ ORC BERSERK) =====
void Minions::ActivateRage() {
    if (isRaging || minionType != MinionType::ORC_BERSERK) return;

    isRaging = true;
    attackCooldown *= 0.5f;     // Tấn công nhanh gấp đôi
    chaseSpeed *= 1.3f;         // Chạy nhanh hơn
    attackDamage = (int)(attackDamage * 1.2f); // Tăng sát thương

    std::cout << "[Orc Berserk] CUONG NO! Toc do va sat thuong tang manh!\n";
    std::cout << "  -> Attack Speed: " << attackCooldown << "s\n";
    std::cout << "  -> Chase Speed: " << chaseSpeed << "\n";
    std::cout << "  -> Damage: " << attackDamage << "\n";
}

// ===== THI PHÁP THUẬT (CHỈ ORC SHAMAN) =====
void Minions::CastMagic() {
    if (magicTimer > 0 || minionType != MinionType::ORC_SHAMAN) return;

    isCastingMagic = true;
    magicTimer = magicCooldown;

    std::cout << "[Orc Shaman] THI PHAP THUAT! Tao projectile magic...\n";

    // TODO: Tạo magic projectile bay về phía target
}

// ===== THỰC HIỆN TẤN CÔNG =====
int Minions::PerformAttack() {
    switch (minionType) {
    case MinionType::ORC_BERSERK:
        // Berserk - Tấn công cận chiến mạnh
        if (IsTargetInAttackRange()) {
            if (isRaging) {
                std::cout << "[Orc Berserk] TAN CONG CUONG NO! Damage: " << attackDamage << "\n";
            }
            else {
                std::cout << "[Orc Berserk] Tan cong can chien! Damage: " << attackDamage << "\n";
            }
            return attackDamage;
        }
        break;

    case MinionType::ORC_SHAMAN:
        // Shaman - Tấn công từ xa bằng magic
        CastMagic();
        std::cout << "[Orc Shaman] Ban magic! Damage: " << attackDamage << "\n";
        return attackDamage;

    case MinionType::ORC_WARRIOR:
        // Warrior - Tấn công cận chiến cân bằng
        if (IsTargetInAttackRange()) {
            std::cout << "[Orc Warrior] Tan cong kiem! Damage: " << attackDamage << "\n";
            return attackDamage;
        }
        break;

    default:
        break;
    }

    return 0;
}

// ===== UPDATE =====
void Minions::Update(float deltaTime, Map& map) {
    Enemy::Update(deltaTime, map);

    // Xử lý magic timer (Shaman)
    if (magicTimer > 0) {
        magicTimer -= deltaTime;
        if (magicTimer <= 0) {
            isCastingMagic = false;
        }
    }

    // Kích hoạt Rage khi máu thấp (Berserk)
    if (minionType == MinionType::ORC_BERSERK && !isRaging) {
        float healthPercent = (float)health / (float)maxHealth;
        if (healthPercent <= rageThreshold) {
            ActivateRage();
        }
    }
}

// ===== RENDER =====
void Minions::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    // Render sprite chính
    Enemy::Render(renderer, cameraOffset);

    // ===== HIỆU ỨNG RAGE (BERSERK) =====
    if (minionType == MinionType::ORC_BERSERK && isRaging) {
        // TODO: Vẽ aura đỏ quanh Berserk
        // Tạm thời: vẽ viền đỏ
        SDL_FRect outline = GetBoundingBox();
        outline.x -= cameraOffset.x - 2;
        outline.y -= cameraOffset.y - 2;
        outline.w += 4;
        outline.h += 4;

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150);
        SDL_RenderRect(renderer, &outline);
    }

    // ===== HIỆU ỨNG MAGIC (SHAMAN) =====
    if (minionType == MinionType::ORC_SHAMAN && isCastingMagic && magicTex) {
        // TODO: Render magic effect xung quanh Shaman
    }
}

// ===== TAKE DAMAGE =====
void Minions::TakeDamage(int damage) {
    // Berserk trong trạng thái Rage nhận ít damage hơn
    if (minionType == MinionType::ORC_BERSERK && isRaging) {
        damage = (int)(damage * 0.9f);
        std::cout << "[Orc Berserk] Giam sat thuong nho Rage! Damage: " << damage << "\n";
    }

    Enemy::TakeDamage(damage);
}