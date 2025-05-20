#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>

// Estrutura de um triângulo: posição (x, y) e cor RGB
struct Triangle {
    glm::vec2 position;
    glm::vec3 color;
};

// Triângulo base (mesma geometria para todos, será transformado)
const float baseTriangle[] = {
     0.0f,  0.0f,
     0.1f,  0.1f,
     0.2f,  0.0f
};

GLuint VAO, VBO, shaderProgram;

// Cria buffers e configura atributos de vértice
void createBaseTriangle() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(baseTriangle), baseTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

// Vertex Shader: aplica a matriz de transformação
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
uniform mat4 transform;
void main() {
    gl_Position = transform * vec4(aPos, 0.0, 1.0);
}
)";

// Fragment Shader: recebe cor por uniform
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 uColor;
void main() {
    FragColor = vec4(uColor, 1.0);
}
)";

// Compila os shaders e retorna o ID do programa
GLuint compileShaders() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

int main() {
    // Inicializa GLFW + janela
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Ex3 - Struct Triangle", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Cria shaders e triângulo base
    shaderProgram = compileShaders();
    createBaseTriangle();

    // Lista de triângulos com posição e cor
    std::vector<Triangle> triangles = {
        {{-0.8f, -0.6f}, {1.0f, 0.0f, 0.0f}},  // vermelho
        {{-0.4f, -0.2f}, {0.0f, 1.0f, 0.0f}},  // verde
        {{ 0.0f,  0.2f}, {0.0f, 0.0f, 1.0f}}   // azul
    };

    // Loop de renderização
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f); // fundo escuro
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        for (auto& t : triangles) {
            // Cria matriz de transformação (translada para a posição)
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(t.position, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transform));
            glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, glm::value_ptr(t.color));

            // Desenha o triângulo
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Libera recursos
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
