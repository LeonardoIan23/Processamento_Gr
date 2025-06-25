// --- INCLUDES E DEFINIÇÕES INICIAIS ---
// Inclui os cabeçalhos das nossas classes (gl_utils, Sprite) e das bibliotecas que usamos.
#include "gl_utils.h"
#include "Sprite.h" 
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Define que a implementação da biblioteca de imagens stb_image será feita neste arquivo.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Define constantes numéricas para as direções, para deixar o código mais legível.
#define DIRECTION_SOUTH 0
#define DIRECTION_WEST  1
#define DIRECTION_EAST  2
#define DIRECTION_NORTH 3

// --- VARIÁVEIS GLOBAIS ---
// Variáveis para controlar o tamanho da janela.
// São globais para que as funções em gl_utils.cpp possam acessá-las.
int g_gl_width = 800;
int g_gl_height = 600;
GLFWwindow *g_window = NULL;


int main() {
    // --- INICIALIZAÇÃO ---
    // Prepara tudo o que é necessário para o programa rodar.
    start_gl(); // Cria a janela e inicializa o OpenGL.
    glDisable(GL_DEPTH_TEST); // Desativa o teste de profundidade, que não é necessário em 2D.
    stbi_set_flip_vertically_on_load(true); // "Desvira" as imagens ao carregar.

    // Cria o nosso objeto personagem e o configura com a imagem e o layout da animação.
    Sprite playerSprite;
    playerSprite.setup("player.png", 4, 4);

    // Guarda o tempo inicial para calcular o tempo decorrido entre os frames.
    double last_time = glfwGetTime();

    // --- LOOP PRINCIPAL DO JOGO ---
    // Tudo que acontece no jogo (movimento, desenho, etc.) fica dentro deste loop.
    // Ele continua rodando até o usuário fechar a janela.
    while (!glfwWindowShouldClose(g_window)) {
        
        // --- GERENCIAMENTO DE TEMPO E FPS ---
        // Calcula quanto tempo se passou desde o último frame (delta_time).
        double current_time = glfwGetTime();
        double delta_time = current_time - last_time;
        last_time = current_time;

        // Atualiza o contador de FPS no título da janela.
        _update_fps_counter(g_window);
        
        // --- LEITURA DO TECLADO (INPUT) ---
        // Verifica a cada frame se alguma tecla de movimento está sendo pressionada.
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
        
        // Informa ao sprite se ele deve tocar a animação de andar ou ficar parado.
        playerSprite.setMoving(movement_key_pressed);
        
        // Verifica se a tecla ESC foi pressionada para fechar o jogo.
        if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(g_window, 1);
        }

        // --- RENDERIZAÇÃO (DESENHO) ---
        // Prepara e desenha tudo na tela a cada frame.
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f); // Define a cor de fundo.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpa a tela.
        glViewport(0, 0, g_gl_width, g_gl_height); // Define a área de desenho.

        playerSprite.update(delta_time); // Atualiza a lógica de animação do sprite.
        playerSprite.draw();             // Desenha o sprite na tela.

        // --- FINALIZAÇÃO DO FRAME ---
        glfwPollEvents(); // Processa eventos da janela (como clicar no 'X' para fechar).
        glfwSwapBuffers(g_window); // Mostra na tela o que foi desenhado.
    }

    // --- FINALIZAÇÃO DO PROGRAMA ---
    // Libera os recursos do GLFW e encerra o programa.
    glfwTerminate();
    return 0;
}