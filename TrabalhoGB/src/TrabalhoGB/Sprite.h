#ifndef SPRITE_H
#define SPRITE_H

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

// A nossa classe Sprite
class Sprite {
public:
    // --- Variáveis de Posição e Estado ---
    int grid_x; // Posição X no grid do mapa
    int grid_y; // Posição Y no grid do mapa
    
    // Posição em píxeis no ecrã (será calculada)
    float screen_x;
    float screen_y;

    // --- Variáveis Gráficas ---
    GLuint texture_id; // O ID da textura (spritesheet) no OpenGL
    int sheet_width;   // Largura da imagem do spritesheet em píxeis
    int sheet_height;  // Altura da imagem do spritesheet em píxeis

    // --- Variáveis de Animação ---
    int num_frames;      // Número total de frames na animação
    int current_frame;   // O frame atual que está a ser desenhado (ex: 0, 1, 2...)
    float frame_time;    // Quanto tempo cada frame deve ficar no ecrã
    float time_counter;  // Acumulador de tempo para controlar a mudança de frames

    // --- Construtor ---
    // Construtor que inicializa o sprite numa posição do grid.
    Sprite(int start_grid_x, int start_grid_y);

    // --- Funções ---

    /**
     * @brief Carrega a textura do sprite a partir de um ficheiro.
     * @param filename O caminho para a imagem (ex: "player.png").
     * @param num_total_frames O número de frames de animação contidos na imagem.
     */
    void loadTexture(const std::string& filename, int num_total_frames);

    /**
     * @brief Atualiza a lógica do sprite, principalmente a animação.
     * @param delta_time O tempo decorrido desde o último frame do jogo.
     */
    void update(float delta_time);

    /**
     * @brief Desenha o sprite no ecrã.
     * @param shader_programme O ID do programa de shader a ser usado.
     * @param projection A matriz de projeção da câmara.
     * @param view A matriz de visão (se tiver uma câmara que se move).
     */
    void draw(GLuint shader_programme, const glm::mat4& projection, const glm::mat4& view);

private:
    // VAO e VBO para a geometria do sprite (um simples rectângulo)
    GLuint VAO;
    GLuint VBO;
};

#endif // SPRITE_H