#include "gl_utils.h"
#include "Sprite.h" 
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define DIRECTION_SOUTH 0
#define DIRECTION_WEST  1
#define DIRECTION_EAST  2
#define DIRECTION_NORTH 3

int g_gl_width = 800;
int g_gl_height = 600;
GLFWwindow *g_window = NULL;


int main() {
    start_gl();
    glDisable(GL_DEPTH_TEST);

    // --- CORREÇÃO ADICIONADA AQUI ---
    // Vira todas as imagens carregadas na vertical para corrigir a orientação
    stbi_set_flip_vertically_on_load(true);

    Sprite playerSprite;
    // O seu código estava usando "player.png", mantive este nome.
    // Se o arquivo for "sully.png", é só trocar o nome aqui.
    playerSprite.setup("player.png", 4, 4);

    double last_time = glfwGetTime();

    while (!glfwWindowShouldClose(g_window)) {
        double current_time = glfwGetTime();
        double delta_time = current_time - last_time;
        last_time = current_time;

        _update_fps_counter(g_window);
        
        bool movement_key_pressed = false; 

        if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_W)) {
            playerSprite.move(DIRECTION_NORTH, delta_time);
            movement_key_pressed = true;
        }
        if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_S)) {
            playerSprite.move(DIRECTION_SOUTH, delta_time);
            movement_key_pressed = true;
        }
        if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_A)) {
            playerSprite.move(DIRECTION_WEST, delta_time);
            movement_key_pressed = true;
        }
        if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_D)) {
            playerSprite.move(DIRECTION_EAST, delta_time);
            movement_key_pressed = true;
        }
        
        playerSprite.setMoving(movement_key_pressed);
        
        if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(g_window, 1);
        }

        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, g_gl_width, g_gl_height);

        playerSprite.update(delta_time);
        playerSprite.draw();

        glfwPollEvents();
        glfwSwapBuffers(g_window);
    }

    glfwTerminate();
    return 0;
}