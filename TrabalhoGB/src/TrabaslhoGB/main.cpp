// src/main.cpp

#include <glad/glad.h>      // <-- ESTA LINHA É A SOLUÇÃO. ELA PRECISA VIR ANTES DA GLFW.
#include <GLFW/glfw3.h>
#include <iostream>
#include "Game.h"

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;

// Instância global do jogo para que o callback possa acessá-la
Game* JogoIsometrico;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Meu Jogo Isometrico", NULL, NULL);
    if (window == NULL) { std::cout << "Failed to create GLFW window" << std::endl; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    // Inicializa a GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Cria o objeto do jogo
    JogoIsometrico = new Game(SCREEN_WIDTH, SCREEN_HEIGHT, window);
    JogoIsometrico->init();

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        
        JogoIsometrico->processInput(deltaTime);
        JogoIsometrico->update(deltaTime);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        JogoIsometrico->render();

        glfwSwapBuffers(window);
    }

    delete JogoIsometrico;
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
