#include "Sprite.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

// Incluímos o stb_image.h aqui também, mas sem o #define
#include "stb_image.h" 

// Assumimos que o gl_utils.h tem a função loadTexture, por isso o incluímos
#include "gl_utils.h" 

// Construtor da classe Sprite
Sprite::Sprite(int start_grid_x, int start_grid_y)
    : grid_x(start_grid_x), grid_y(start_grid_y),
      screen_x(0.0f), screen_y(0.0f),
      texture_id(0), sheet_width(0), sheet_height(0),
      num_frames(1), current_frame(0),
      frame_time(0.1f), // Animação com 10 frames por segundo por padrão
      time_counter(0.0f) {

    // --- Geometria do Sprite ---
    float vertices[] = {
        // Posições (x, y)   // Coordenadas de Textura (u, v)
         0.5f,  0.5f,        1.0f, 1.0f, // Topo Direita
         0.5f, -0.5f,        1.0f, 0.0f, // Base Direita
        -0.5f, -0.5f,        0.0f, 0.0f, // Base Esquerda
        -0.5f,  0.5f,        0.0f, 1.0f  // Topo Esquerda
    };
    unsigned int indices[] = {
        0, 1, 3, // Primeiro triângulo
        1, 2, 3  // Segundo triângulo
    };

    // --- Configuração do VAO e VBO ---
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    GLuint EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Atributos de Vértice
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

/**
 * @brief Carrega a textura do sprite e define os parâmetros de animação.
 */
void Sprite::loadTexture(const std::string& filename, int num_total_frames) {
    ::loadTexture(this->texture_id, filename.c_str());
    this->num_frames = num_total_frames;
    int channels;
    stbi_info(filename.c_str(), &this->sheet_width, &this->sheet_height, &channels);
}

/**
 * @brief Atualiza a lógica da animação do sprite.
 */
void Sprite::update(float delta_time) {
    this->time_counter += delta_time;

    if (this->time_counter >= this->frame_time) {
        this->current_frame++;
        this->time_counter = 0.0f;

        if (this->current_frame >= this->num_frames) {
            this->current_frame = 0;
        }
    }
}

/**
 * @brief Desenha o sprite no ecrã na sua posição atual.
 */
void Sprite::draw(GLuint shader_programme, const glm::mat4& projection, const glm::mat4& view) {
    glUseProgram(shader_programme);

    // --- Matriz de Modelo (Model Matrix) ---
    glm::mat4 model = glm::mat4(1.0f);
    // 1. Translação
    float z_depth = (float)this->grid_y + 0.5f; // Garante que o sprite desenha sobre o tile
    model = glm::translate(model, glm::vec3(this->screen_x, this->screen_y, z_depth));
    
    // 2. Escala
    float frame_width = (float)this->sheet_width / this->num_frames;
    model = glm::scale(model, glm::vec3(frame_width, this->sheet_height, 1.0f));

    // Envia matrizes para o shader
    glUniformMatrix4fv(glGetUniformLocation(shader_programme, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader_programme, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader_programme, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // --- Lógica da Animação no Shader ---
    float offset_x = (float)this->current_frame / this->num_frames;
    float tile_width = 1.0f / this->num_frames;
    glUniform1f(glGetUniformLocation(shader_programme, "texOffset"), offset_x);
    glUniform1f(glGetUniformLocation(shader_programme, "texTileWidth"), tile_width);

    // --- Desenho ---
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture_id); // <-- CORREÇÃO APLICADA AQUI
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}