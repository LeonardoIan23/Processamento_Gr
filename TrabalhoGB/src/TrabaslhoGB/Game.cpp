#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Game::Game(unsigned int width, unsigned int height, GLFWwindow* window)
    : State(GAME_ACTIVE), Width(width), Height(height), Window(window) {}

Game::~Game() {}

void Game::init() {
    MainShader = std::make_unique<Shader>();
    MainShader->load("sprite.vert", "sprite.frag");
    MainRenderer = std::make_unique<Renderer>(*MainShader);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -100.0f, 100.0f);
    MainShader->use();
    MainShader->setInt("image", 0);
    MainShader->setMat4("projection", projection);
    Map = std::make_unique<Tilemap>();
    Map->load("map.txt");
    totalCoins = Map->objects.size();
    ThePlayer = std::make_unique<Player>(Map->playerStartPos);
    ThePlayer->loadTexture("sully.png", 4, 4);
}
void Game::processInput(float dt) {
    if (this->State == GAME_ACTIVE) {
        ThePlayer->processInput(this->Window, dt, *Map);
    }
}

void Game::update(float dt) {
    if (this->State != GAME_ACTIVE) return;
    ThePlayer->update(dt, *Map);
    int tileToCheck = -1;
    if (ThePlayer->isMoving()) {
        tileToCheck = Map->getTileAt(ThePlayer->getTargetPosition());
    } else {
        tileToCheck = Map->getTileAt(ThePlayer->gridPosition);
    }
    if (tileToCheck == Map->tileLavaID || (Map->tileAguaID != -1 && tileToCheck == Map->tileAguaID)) {
        this->State = GAME_OVER;
        ThePlayer->gridPosition = ThePlayer->lastValidPosition;
        std::cout << "GAME OVER! Voce caiu na lava ou na agua." << std::endl;
        return;
    }
    int objectIndex = Map->getObjectAt(ThePlayer->gridPosition);
    if (objectIndex != -1) {
        Map->removeObject(objectIndex);
        Score++;
        std::cout << "Moeda coletada! Pontos: " << Score << "/" << totalCoins << std::endl;
        if (Score >= totalCoins) {
            this->State = GAME_WIN;
            std::cout << "VITORIA! Todas as moedas foram coletadas." << std::endl;
        }
    }
}

void Game::render() {
    glm::vec2 playerRenderPos = ThePlayer->getRenderPosition(*Map);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-playerRenderPos.x + Width / 2, -playerRenderPos.y + Height / 2, 0.0f));
    
    MainShader->use();
    MainShader->setMat4("view", view);

    Map->draw(*MainRenderer);
    ThePlayer->draw(*MainRenderer, *Map);
    
    if (this->State != GAME_ACTIVE) {
        view = glm::mat4(1.0f);
        MainShader->setMat4("view", view);
        glm::vec3 overlayColor = (this->State == GAME_WIN) ? glm::vec3(0.0f, 0.6f, 0.2f) : glm::vec3(0.8f, 0.1f, 0.0f);
        MainRenderer->drawOverlay(overlayColor, 0.5f, this->Width, this->Height);
    }
}