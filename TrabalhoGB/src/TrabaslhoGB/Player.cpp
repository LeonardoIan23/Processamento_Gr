#include "Player.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

Player::Player(glm::ivec2 startPos)
    : gridPosition(startPos), lastValidPosition(startPos),
      startPos(startPos), targetPos(startPos) {}

void Player::loadTexture(const char* file, int frames, int animations) {
    this->spritesheet.load(file, true);
    this->numFrames = frames;
    this->numAnimations = animations;
}

void Player::processInput(GLFWwindow* window, float dt, const Tilemap& map) {
    if (isVisualMoving || inputTimer < inputCooldown) { return; }

    glm::ivec2 moveDir(0, 0);

    // Mapeamento para 8 direções
    bool n = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool s = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool w = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool e = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

    if (n && !s) moveDir.y = -1;
    if (s && !n) moveDir.y = 1;
    if (w && !e) moveDir.x = -1;
    if (e && !w) moveDir.x = 1;

    if (moveDir.x != 0 || moveDir.y != 0) {
        lastValidPosition = gridPosition;
        glm::ivec2 potentialTarget = gridPosition + moveDir;

        if (map.isPositionWalkable(potentialTarget) && abs(map.getHeightAt(potentialTarget) - map.getHeightAt(gridPosition)) <= 1) {
            isVisualMoving = true;
            visualMoveTimer = 0.0f;
            inputTimer = 0.0f;
            startPos = gridPosition;
            targetPos = potentialTarget;
            
            // ########## LÓGICA DE ANIMAÇÃO CORRIGIDA ##########
            // Com base na análise do seu spritesheet, invertemos a lógica para esquerda/direita.
            if (moveDir.x > 0) {          // Se move para a direita (tecla 'D')...
                animDirection = ANIM_LEFT_WALK;  // ...usa a animação que está na linha 2.
            } else if (moveDir.x < 0) {   // Se move para a esquerda (tecla 'A')...
                animDirection = ANIM_RIGHT_WALK; // ...usa a animação que está na linha 1.
            } else if (moveDir.y > 0) {   // Se move para baixo (tecla 'S')...
                animDirection = ANIM_DOWN;       // ...usa a animação da linha 3.
            } else if (moveDir.y < 0) {   // Se move para cima (tecla 'W')...
                animDirection = ANIM_UP;         // ...usa a animação da linha 0.
            }
            // ######################################################
        }
    }
}

void Player::update(float dt, Tilemap& map) {
    inputTimer += dt;
    if (isVisualMoving) {
        animTimer += dt;
        if (animTimer >= animCooldown) {
            animTimer = 0.0f;
            currentFrame = (currentFrame + 1) % numFrames;
        }
        visualMoveTimer += dt;
        if (visualMoveTimer >= moveDuration) {
            isVisualMoving = false;
            gridPosition = targetPos;
            if(map.tilePisadoID != -1) {
                map.setTileAt(gridPosition, map.tilePisadoID);
            }
        }
    } else {
        currentFrame = 0;
    }
}

void Player::draw(Renderer &renderer, const Tilemap& map) {
    float frameWidth = 1.0f / (float)this->numFrames;
    float frameHeight = 1.0f / (float)this->numAnimations;
    // A enum é convertida para float implicitamente aqui, o que funciona perfeitamente.
    glm::vec2 uvOffset = { (float)currentFrame * frameWidth, (float)animDirection * frameHeight };
    glm::vec2 uvScale = { frameWidth, frameHeight };
    glm::vec2 renderPos = this->getRenderPosition(map);
    float spriteWidth = 128.0f;
    float spriteHeight = 128.0f;
    // Ajusta a posição para o "pé" do sprite ficar no centro do tile
    renderPos.x -= spriteWidth / 2.0f;
    renderPos.y -= spriteHeight - (map.tileHeight / 2.0f);
    renderer.draw(this->spritesheet, renderPos, glm::vec2(spriteWidth, spriteHeight), 0.0f, glm::vec3(1.0f), 1.0f, uvOffset, uvScale);
}

glm::vec2 Player::getRenderPosition(const Tilemap& map) {
    glm::vec2 renderPos;
    if (isVisualMoving) {
        float progress = visualMoveTimer / moveDuration;
        glm::vec2 startScreenPos = map.getScreenPos(startPos);
        glm::vec2 targetScreenPos = map.getScreenPos(targetPos);
        renderPos = glm::mix(startScreenPos, targetScreenPos, progress);
    } else {
        renderPos = map.getScreenPos(gridPosition);
    }
    return renderPos;
}