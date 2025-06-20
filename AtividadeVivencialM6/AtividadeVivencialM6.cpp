#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <fstream>

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "gl_utils.h"
#include "TileMap.h"
#include "DiamondView.h"

using namespace std;

// --- Variáveis Globais ---
int g_gl_width = 800;
int g_gl_height = 600;
GLFWwindow* g_window = NULL;

TileMap* tmap = NULL;
TilemapView* tview = new DiamondView();

int player_col = 1;
int player_row = 1;

int tileSetCols = 7;
float tileW_tex, tileH_tex;

// --- Funções Auxiliares ---
TileMap* readMap(const char* filename) {
    ifstream arq(filename);
    if (!arq.is_open()) {
        cout << "ERRO: Não foi possível abrir o arquivo de mapa: " << filename << endl;
        return NULL;
    }
    int w, h;
    arq >> w >> h;
    TileMap* tmap = new TileMap(w, h, 0);
    player_col = w / 2;
    player_row = h / 2;
    for (int r = 0; r < h; r++) {
        for (int c = 0; c < w; c++) {
            int tid;
            arq >> tid;
            tmap->setTile(c, (h - 1) - r, tid);
        }
    }
    arq.close();
    return tmap;
}

int loadTexture(unsigned int& texture, const char* filename) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // --- CORREÇÃO DE TEXTURA: Impede a repetição da textura ---
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        if (nrChannels == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Falha ao carregar a textura: " << filename << std::endl;
    }
    stbi_image_free(data);
    return 1;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        int next_col = player_col;
        int next_row = player_row;
        int direction = -1;
        if (key == GLFW_KEY_W) direction = DIRECTION_SOUTH;
        if (key == GLFW_KEY_S) direction = DIRECTION_NORTH;
        if (key == GLFW_KEY_D) direction = DIRECTION_EAST;
        if (key == GLFW_KEY_A) direction = DIRECTION_WEST;
        if (key == GLFW_KEY_Q) direction = DIRECTION_NORTHWEST;
        if (key == GLFW_KEY_E) direction = DIRECTION_NORTHEAST;
        if (key == GLFW_KEY_Z) direction = DIRECTION_SOUTHWEST;
        if (key == GLFW_KEY_C) direction = DIRECTION_SOUTHEAST;
        if (direction != -1) {
            tview->computeTileWalking(next_col, next_row, direction);
            if (next_col >= 0 && next_col < tmap->getWidth() && next_row >= 0 && next_row < tmap->getHeight()) {
                player_col = next_col;
                player_row = next_row;
            }
        }
    }
}

int main() {
    start_gl();
    glfwSetKeyCallback(g_window, key_callback);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    tmap = readMap("terrain1.tmap");
    if (tmap == NULL) return -1;
    
    GLuint tileset_texture;
    loadTexture(tileset_texture, "terrain.png");
    tmap->setTid(tileset_texture);

    float w_world = 2.0f;
    float tile_render_width = w_world / 10.0f;
    float tile_render_height = tile_render_width / 2.0f;
    tileW_tex = 1.0f / (float)tileSetCols;
    tileH_tex = 1.0f;
    
    float vertices[] = {
        -tile_render_width / 2.0f, 0.0f,                       0.0f, 0.5f,
        0.0f,                     -tile_render_height / 2.0f,    0.5f, 0.0f,
        tile_render_width / 2.0f,  0.0f,                       1.0f, 0.5f,
        0.0f,                      tile_render_height / 2.0f,    0.5f, 1.0f,
    };
    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint shader_programme = create_programme_from_files("_geral_vs.glsl", "_geral_fs.glsl");

    while (!glfwWindowShouldClose(g_window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_programme);
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, tmap->getTileSet());
        glUniform1i(glGetUniformLocation(shader_programme, "ourTexture"), 0);
        
        float map_offset_y = ( (float)tmap->getHeight() * tile_render_height ) / 2.0f;

        for (int r = 0; r < tmap->getHeight(); r++) {
            for (int c = 0; c < tmap->getWidth(); c++) {
                int tile_id = (int)tmap->getTile(c, r);
                int u = tile_id % tileSetCols;

                float screen_x, screen_y;
                tview->computeDrawPosition(c, r, tile_render_width, tile_render_height, screen_x, screen_y);
                
                glUniform1f(glGetUniformLocation(shader_programme, "offsetX"), u * tileW_tex);
                glUniform1f(glGetUniformLocation(shader_programme, "tileW"), tileW_tex);
                glUniform1f(glGetUniformLocation(shader_programme, "tx"), screen_x);
                glUniform1f(glGetUniformLocation(shader_programme, "ty"), screen_y - map_offset_y);
                
                float highlight_weight = (c == player_col && r == player_row) ? 0.5f : 0.0f;
                glUniform1f(glGetUniformLocation(shader_programme, "weight"), highlight_weight);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }

        glfwPollEvents();
        if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(g_window, 1);
        }
        
        glfwSwapBuffers(g_window);
    }

    glfwTerminate();
    delete tmap;
    delete tview;
    return 0;
}