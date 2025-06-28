#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer(Shader &shader) {
    this->shader = shader;
    this->initRenderData();
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &this->quadVAO);
}

void Renderer::initRenderData() {
    unsigned int VBO;
    float vertices[] = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned char whiteData[] = { 255, 255, 255, 255 };
    this->whiteTexture.create(whiteData, 1, 1, GL_RGBA);
}

void Renderer::draw(const Texture2D &texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color, float alpha, glm::vec2 uvOffset, glm::vec2 uvScale) {
    this->shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    this->shader.setMat4("model", model);
    this->shader.setVec3("spriteColor", color);
    this->shader.setFloat("alpha", alpha);
    this->shader.setVec2("uvOffset", uvOffset);
    this->shader.setVec2("uvScale", uvScale);
    
    glActiveTexture(GL_TEXTURE0);
    texture.bind();
    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Renderer::drawOverlay(glm::vec3 color, float alpha, unsigned int screenWidth, unsigned int screenHeight) {
    this->draw(this->whiteTexture, glm::vec2(0,0), glm::vec2(screenWidth, screenHeight), 0.0f, color, alpha);
}