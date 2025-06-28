// src/Game.h
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "Shader.h"
#include "Renderer.h"
#include "Tilemap.h"
#include "Player.h"

enum GameState { GAME_ACTIVE, GAME_WIN, GAME_OVER };

class Game {
public:
    GameState State;
    GLFWwindow* Window;
    unsigned int Width, Height;
    int Score = 0;

    Game(unsigned int width, unsigned int height, GLFWwindow* window);
    ~Game();

    void init();
    // A função de input agora não precisa mais receber o mapa
    void processInput(float dt);
    void update(float dt);
    void render();

private:
    std::unique_ptr<Shader> MainShader;
    std::unique_ptr<Renderer> MainRenderer;
    std::unique_ptr<Tilemap> Map;
    std::unique_ptr<Player> ThePlayer;
    int totalCoins = 0;
};