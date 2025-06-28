#pragma once
#include <glad/glad.h> 
#include "Shader.h"
#include "Texture.h"
#include <glm/glm.hpp>

class Renderer {
public:
    Renderer(Shader &shader);
    ~Renderer();
    void draw(const Texture2D &texture, glm::vec2 position, glm::vec2 size, 
              float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f), float alpha = 1.0f,
              glm::vec2 uvOffset = glm::vec2(0.0f, 0.0f), glm::vec2 uvScale = glm::vec2(1.0f, 1.0f));
              
    void drawOverlay(glm::vec3 color, float alpha, unsigned int screenWidth, unsigned int screenHeight);
private:
    Shader shader;
    Texture2D whiteTexture;
    unsigned int quadVAO;
    void initRenderData();
};