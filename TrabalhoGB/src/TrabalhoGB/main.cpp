#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Inclusões dos seus próprios ficheiros de utilidades e do mapa
#include "gl_utils.h"
#include "Map.h"

// Declaração da função de projeção isométrica
void project_iso(int col, int row, float& screen_x, float& screen_y, float tile_width, float tile_height);

int main() {
    // Carrega os dados do mapa a partir do arquivo
    MapData mapa;
    if (!loadMap("assets/mapa/mapa.txt", mapa)) { // Assumindo que o mapa.txt está na pasta assets
        return -1; 
    }
    std::cout << "Mapa " << mapa.width << "x" << mapa.height << " carregado." << std::endl;

    // Inicializa OpenGL, GLFW e a janela (função de gl_utils.cpp)
    start_gl();

    // Configurações do OpenGL
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST); // Ativa o teste de profundidade

    // Carrega a textura do tileset (função de gl_utils.cpp)
    std::string tilesetPath = "assets/mapa/" + mapa.tileset.filename;
    loadTexture(mapa.tileset.texture_id, tilesetPath.c_str());

    // Cria o programa de shader (função de gl_utils.cpp)
    GLuint shader_programme = create_programme_from_files("src/TrabalhoGB/shader_vs.glsl", "src/TrabalhoGB/shader_fs.glsl");

    // Define a geometria de um único tile
    float tile_w = (float)mapa.tileset.tile_width;
    float tile_h = (float)mapa.tileset.tile_height;
    float tile_vertices[] = {
         // posições (x,y)      // coordenadas de textura (u,v)
         0.5f * tile_w, tile_h,          0.5f, 1.0f,
         tile_w,        0.5f * tile_h,  1.0f, 0.5f,
         0.5f * tile_w, 0.0f,           0.5f, 0.0f,
         0.0f,          0.5f * tile_h,  0.0f, 0.5f
    };
    unsigned int tile_indices[] = { 0, 1, 3, 1, 2, 3 };

    // Configuração do VAO, VBO, e EBO para a geometria do tile
    unsigned int tile_VAO, tile_VBO, tile_EBO;
    glGenVertexArrays(1, &tile_VAO);
    glGenBuffers(1, &tile_VBO);
    glGenBuffers(1, &tile_EBO);
    glBindVertexArray(tile_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, tile_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tile_vertices), tile_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tile_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tile_indices), tile_indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Matriz de projeção ortográfica 2D
    // Aumentamos o Z-far/near para dar espaço para a nossa lógica de profundidade
    glm::mat4 projection = glm::ortho(0.0f, (float)g_gl_width, 0.0f, (float)g_gl_height, -100.0f, 100.0f);

    // Loop principal do jogo
    while (!glfwWindowShouldClose(g_window)) {
        _update_fps_counter(g_window);
        
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, g_gl_width, g_gl_height);
        
        glUseProgram(shader_programme);
        
        // Envia a matriz de projeção para o shader (só precisa ser feito uma vez por frame)
        glUniformMatrix4fv(glGetUniformLocation(shader_programme, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Ativa e vincula a textura do tileset
        glBindVertexArray(tile_VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mapa.tileset.texture_id);
        glUniform1i(glGetUniformLocation(shader_programme, "ourTexture"), 0);

        // Loop para desenhar cada tile do mapa
        for (int r = 0; r < mapa.height; r++) {
            for (int c = 0; c < mapa.width; c++) {
                int tile_id = mapa.tile_grid[r * mapa.width + c];
                
                // Converte coordenadas do grid para isométricas
                float iso_x, iso_y;
                project_iso(c, r, iso_x, iso_y, tile_w, tile_h);

                // Cria a matriz de modelo para posicionar o tile no mundo
                glm::mat4 model = glm::mat4(1.0f);
                float center_offset_x = (g_gl_width / 2.0f) - (tile_w / 2.0f);
                float map_total_height = (mapa.height + mapa.width) / 2.0f * (tile_h / 2.0f);
                float center_offset_y = (g_gl_height / 2.0f) + (map_total_height / 2.0f) - tile_h;
                
                // A correção da profundidade para o Z-Test funcionar corretamente
                float z_depth = (float)r;
                model = glm::translate(model, glm::vec3(iso_x + center_offset_x, center_offset_y - iso_y, z_depth));
                
                // Envia a matriz de modelo e os dados da textura para o shader
                glUniformMatrix4fv(glGetUniformLocation(shader_programme, "model"), 1, GL_FALSE, glm::value_ptr(model));
                float tex_offset_x = (float)(tile_id % mapa.tileset.num_tiles) / mapa.tileset.num_tiles;
                glUniform1f(glGetUniformLocation(shader_programme, "texOffset"), tex_offset_x);
                glUniform1f(glGetUniformLocation(shader_programme, "texTileWidth"), 1.0f / mapa.tileset.num_tiles);

                // Desenha o tile
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }
        
        glfwPollEvents();
        glfwSwapBuffers(g_window);
    }

    // Finaliza o GLFW
    glfwTerminate();
    return 0;
}

/**
 * @brief Converte coordenadas de grid (col, row) para coordenadas de ecrã isométricas.
 */
void project_iso(int col, int row, float& screen_x, float& screen_y, float tile_width, float tile_height) {
    screen_x = (col - row) * (tile_width / 2.0f);
    screen_y = (row + col) * (tile_height / 2.0f); // Correção: era (col + row), mas (row + col) é o mesmo e mais comum
}