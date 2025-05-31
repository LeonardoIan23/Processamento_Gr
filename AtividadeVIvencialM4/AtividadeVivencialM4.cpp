#include <iostream>
#include <vector>
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLuint WIDTH = 800, HEIGHT = 600;
float playerX = 400.0f, playerY = 100.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
GLuint loadTexture(const char* path);
GLuint setupShader();
GLuint createQuad();

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
uniform mat4 model;
uniform mat4 projection;
out vec2 TexCoord;
void main() {
    gl_Position = projection * model * vec4(position, 1.0);
    TexCoord = texCoord;
})";

const char* fragmentShaderSource = R"(
#version 330 core
in vec2 TexCoord;
out vec4 color;
uniform sampler2D tex;
void main() {
    color = texture(tex, TexCoord);
})";

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Paralaxe - Leonardo", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    GLuint shaderID = setupShader();
    GLuint quadVAO = createQuad();
    glm::mat4 projection = glm::ortho(0.0f, float(WIDTH), 0.0f, float(HEIGHT));

    std::map<std::string, GLuint> textures = {
        {"sky", loadTexture("sky.png")},
        {"clouds", loadTexture("clouds_1.png")},
        {"rocks", loadTexture("rocks.png")},
        {"plant", loadTexture("plant.png")},
        {"ground1", loadTexture("ground_1.png")},
        {"ground2", loadTexture("ground_2.png")},
        {"ground3", loadTexture("ground_3.png")},
        {"player", loadTexture("player.png")} // coloque sua imagem do personagem aqui
    };

    glUseProgram(shaderID);
    glUniform1i(glGetUniformLocation(shaderID, "tex"), 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.3f, 0.6f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderID);
        glBindVertexArray(quadVAO);
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        struct Layer { const char* name; float speed; float y; float scaleY; };
        std::vector<Layer> layers = {
            {"sky",     0.05f, 450, 600},
            {"clouds",  0.10f, 400, 300},
            {"rocks",   0.25f, 300, 300},
            {"plant",   0.40f, 200, 200},
            {"ground1", 0.60f, 100, 200},
            {"ground2", 0.80f,  50, 200},
            {"ground3", 1.00f,   0, 200}
        };

        for (const auto& layer : layers) {
            glBindTexture(GL_TEXTURE_2D, textures[layer.name]);
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-playerX * layer.speed + 400, layer.y, 0.0f));
            model = glm::scale(model, glm::vec3(800, layer.scaleY, 1));
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        glBindTexture(GL_TEXTURE_2D, textures["player"]);
        glm::mat4 playerModel = glm::translate(glm::mat4(1.0f), glm::vec3(playerX, playerY, 0));
        playerModel = glm::scale(playerModel, glm::vec3(100, 100, 1));
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(playerModel));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_LEFT)  playerX -= 10;
        if (key == GLFW_KEY_RIGHT) playerX += 10;
        if (key == GLFW_KEY_UP)    playerY += 10;
        if (key == GLFW_KEY_DOWN)  playerY -= 10;
    }
}

GLuint setupShader() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

GLuint createQuad() {
    float vertices[] = {
        0, 0, 0, 0, 0,
        1, 0, 0, 1, 0,
        0, 1, 0, 0, 1,
        1, 1, 0, 1, 1
    };
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    return VAO;
}

GLuint loadTexture(const char* path) {
    std::string basePath = "../src/AtividadeVivencialM4"; // Caminho onde está o .cpp
    std::string fullPath = basePath + path;

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Erro ao carregar imagem: " << fullPath << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}
