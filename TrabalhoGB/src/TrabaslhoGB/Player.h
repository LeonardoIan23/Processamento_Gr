// src/Player.h
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Texture.h"
#include "Renderer.h"
#include "Tilemap.h"

// ##### ENUM CORRIGIDA E MAIS DESCRITIVA #####
// A ordem agora corresponde exatamente às linhas do seu spritesheet (sully.png)
// Linha 0: Cima, Linha 1: Direita, Linha 2: Esquerda, Linha 3: Baixo
enum PlayerAnim {
    ANIM_UP = 3,
    ANIM_RIGHT_WALK = 2,
    ANIM_LEFT_WALK = 1,
    ANIM_DOWN = 0
};
// #############################################

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
    // A direção inicial continua a ser para baixo, mas agora usando a enum correta.
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