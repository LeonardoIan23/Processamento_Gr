// src/Player.h
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Texture.h"
#include "Renderer.h"
#include "Tilemap.h"

enum PlayerAnim { ANIM_DOWN, ANIM_UP, ANIM_LEFT, ANIM_RIGHT };

class Player {
public:
    glm::ivec2 gridPosition;
    glm::ivec2 lastValidPosition;

    Player(glm::ivec2 startPos);
    void loadTexture(const char* file, int frames, int animations);
    void processInput(GLFWwindow *window, float dt, const Tilemap& map);
    void update(float dt, Tilemap& map);
    void draw(Renderer &renderer, const Tilemap& map);
    
    // Funções Getter
    bool isMoving() const { return isVisualMoving; }
    glm::ivec2 getTargetPosition() const { return targetPos; }
    glm::vec2 getRenderPosition(const Tilemap& map);

    // --- NOVA FUNÇÃO DE DEPURAÇÃO ---
    void debug_draw(Renderer &renderer, glm::vec2 position);

private:
    Texture2D spritesheet;
    PlayerAnim animDirection = ANIM_DOWN;
    float animCooldown = 0.1f;
    float animTimer = 0.0f;
    int currentFrame = 0;
    int numFrames = 0;
    int numAnimations = 0;
    bool isVisualMoving = false;
    float moveDuration = 0.15f;
    float visualMoveTimer = 0.0f;
    glm::ivec2 startPos;
    glm::ivec2 targetPos;
    float inputCooldown = 0.16f;
    float inputTimer = 0.16f;
};