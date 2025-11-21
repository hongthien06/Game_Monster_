#pragma once
#include <SDL3/SDL.h>
#include <cmath>
#include <algorithm>

enum class TransitionType {
    FADE,
    CIRCLE_EXPAND,
    WIPE_RIGHT,
    SLIDE_UP,
    ZOOM_FLASH
};

enum class TransitionState {
    IDLE,
    TRANSITIONING_OUT,  // ?ang t?i d?n (menu -> ?en)
    TRANSITIONING_IN    // ?ang sáng d?n (?en -> game)
};

class ScreenTransition {
private:
    SDL_Renderer* renderer;
    TransitionType type;
    TransitionState state;
    float progress;          // 0.0 -> 1.0
    float duration;          // Th?i gian transition (giây)
    float elapsedTime;
    bool isActive;
    int screenWidth;
    int screenHeight;

public:
    ScreenTransition(SDL_Renderer* ren, int w, int h,
        TransitionType t = TransitionType::FADE,
        float dur = 0.4f)
        : renderer(ren), screenWidth(w), screenHeight(h),
        type(t), duration(dur),
        state(TransitionState::IDLE),
        progress(0.0f), elapsedTime(0.0f),
        isActive(false) {
    }

    void Start() {
        isActive = true;
        state = TransitionState::TRANSITIONING_OUT;
        progress = 0.0f;
        elapsedTime = 0.0f;
    }

    void Update(float deltaTime) {
        if (!isActive) return;

        elapsedTime += deltaTime;
        progress = std::min(elapsedTime / duration, 1.0f);

        // Khi hoàn thành fade out, chuy?n sang fade in
        if (state == TransitionState::TRANSITIONING_OUT && progress >= 1.0f) {
            state = TransitionState::TRANSITIONING_IN;
            progress = 0.0f;
            elapsedTime = 0.0f;
        }
        // Khi hoàn thành fade in, k?t thúc transition
        else if (state == TransitionState::TRANSITIONING_IN && progress >= 1.0f) {
            isActive = false;
            state = TransitionState::IDLE;
            progress = 0.0f;
        }
    }

    void Render() {
        if (!isActive && state == TransitionState::IDLE) return;

        switch (type) {
        case TransitionType::FADE:
            RenderFade();
            break;
        case TransitionType::CIRCLE_EXPAND:
            RenderCircleExpand();
            break;
        case TransitionType::WIPE_RIGHT:
            RenderWipeRight();
            break;
        case TransitionType::SLIDE_UP:
            RenderSlideUp();
            break;
        case TransitionType::ZOOM_FLASH:
            RenderZoomFlash();
            break;
        }
    }

    bool IsActive() const { return isActive; }
    TransitionState GetState() const { return state; }
    float GetProgress() const { return progress; }

    // switch background
    bool ShouldSwitchScreen() const {
        return state == TransitionState::TRANSITIONING_IN && progress > 0.0f;
    }

    void SetScreenSize(int w, int h) {
        screenWidth = w;
        screenHeight = h;
    }

private:
    // ===== FADE EFFECT =====
    void RenderFade() {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        Uint8 alpha = 0;
        if (state == TransitionState::TRANSITIONING_OUT) {
            alpha = (Uint8)(progress * 255);
        }
        else {
            alpha = (Uint8)((1.0f - progress) * 255);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_FRect fullScreen = { 0, 0, (float)screenWidth, (float)screenHeight };
        SDL_RenderFillRect(renderer, &fullScreen);
    }

    // ===== CIRCLE EXPAND EFFECT =====
    void RenderCircleExpand() {
        int centerX = screenWidth / 2;
        int centerY = screenHeight / 2;
        float maxRadius = sqrtf((float)(screenWidth * screenWidth + screenHeight * screenHeight)) / 2.0f;

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        if (state == TransitionState::TRANSITIONING_OUT) {
  
            float currentRadius = maxRadius * progress;

            for (float r = 0; r < currentRadius; r += 3.0f) {
                float alpha = 255.0f * (r / currentRadius);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, (Uint8)alpha);
                DrawCircle(centerX, centerY, (int)(currentRadius - r));
            }

            if (progress > 0.3f) {
                float innerRadius = maxRadius * (progress - 0.3f) / 0.7f;
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                DrawFilledCircle(centerX, centerY, (int)innerRadius);
            }
        }
        else {
            // fade in
            float currentRadius = maxRadius * (1.0f - progress);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            DrawFilledCircle(centerX, centerY, (int)currentRadius);
        }
    }

    // ===== WIPE RIGHT EFFECT =====
    void RenderWipeRight() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        float wipeWidth = 0;
        if (state == TransitionState::TRANSITIONING_OUT) {
            wipeWidth = screenWidth * progress;
        }
        else {
            wipeWidth = screenWidth * (1.0f - progress);
        }

        SDL_FRect wipeRect = { 0, 0, wipeWidth, (float)screenHeight };
        SDL_RenderFillRect(renderer, &wipeRect);
    }

    // ===== SLIDE UP EFFECT =====
    void RenderSlideUp() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        float slideHeight = 0;
        if (state == TransitionState::TRANSITIONING_OUT) {
            slideHeight = screenHeight * progress;
        }
        else {
            slideHeight = screenHeight * (1.0f - progress);
        }

        SDL_FRect slideRect = { 0, screenHeight - slideHeight, (float)screenWidth, slideHeight };
        SDL_RenderFillRect(renderer, &slideRect);
    }

    // ===== ZOOM FLASH EFFECT =====
    void RenderZoomFlash() {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        if (state == TransitionState::TRANSITIONING_OUT) {
         
            if (progress < 0.3f) {
                Uint8 alpha = (Uint8)((progress / 0.3f) * 255);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);
            }
            else {
          
                Uint8 alpha = (Uint8)(((progress - 0.3f) / 0.7f) * 255);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
            }
        }
        else {

            Uint8 alpha = (Uint8)((1.0f - progress) * 255);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        }

        SDL_FRect fullScreen = { 0, 0, (float)screenWidth, (float)screenHeight };
        SDL_RenderFillRect(renderer, &fullScreen);
    }

    // ===== HELPER FUNCTIONS =====
    void DrawCircle(int cx, int cy, int radius) {
        if (radius <= 0) return;

        int x = radius;
        int y = 0;
        int err = 0;

        while (x >= y) {
            SDL_RenderPoint(renderer, (float)(cx + x), (float)(cy + y));
            SDL_RenderPoint(renderer, (float)(cx + y), (float)(cy + x));
            SDL_RenderPoint(renderer, (float)(cx - y), (float)(cy + x));
            SDL_RenderPoint(renderer, (float)(cx - x), (float)(cy + y));
            SDL_RenderPoint(renderer, (float)(cx - x), (float)(cy - y));
            SDL_RenderPoint(renderer, (float)(cx - y), (float)(cy - x));
            SDL_RenderPoint(renderer, (float)(cx + y), (float)(cy - x));
            SDL_RenderPoint(renderer, (float)(cx + x), (float)(cy - y));

            if (err <= 0) {
                y += 1;
                err += 2 * y + 1;
            }
            if (err > 0) {
                x -= 1;
                err -= 2 * x + 1;
            }
        }
    }

    void DrawFilledCircle(int cx, int cy, int radius) {
        if (radius <= 0) return;

        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                if (x * x + y * y <= radius * radius) {
                    SDL_RenderPoint(renderer, (float)(cx + x), (float)(cy + y));
                }
            }
        }
    }
};