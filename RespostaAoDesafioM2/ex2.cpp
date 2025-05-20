#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

// Declaração da função definida em ex1.cpp
extern GLuint createTriangle(float, float, float, float, float, float);

int main() {
    // Inicialização do GLFW e criação da janela
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Ex2 - 5 Triângulos", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Criação de 5 triângulos com posições distintas
    std::vector<GLuint> vaos;
    vaos.push_back(createTriangle(-0.9f, -0.5f, -0.7f, 0.0f, -0.5f, -0.5f));
    vaos.push_back(createTriangle(-0.4f, -0.5f, -0.2f, 0.0f,  0.0f, -0.5f));
    vaos.push_back(createTriangle( 0.1f, -0.5f,  0.3f, 0.0f,  0.5f, -0.5f));
    vaos.push_back(createTriangle(-0.8f,  0.2f, -0.6f, 0.7f, -0.4f,  0.2f));
    vaos.push_back(createTriangle( 0.4f,  0.4f,  0.6f, 0.9f,  0.8f,  0.4f));

    // Loop de renderização
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT); // Limpa a tela com a cor atual

        // Renderiza cada triângulo
        for (GLuint vao : vaos) {
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glfwSwapBuffers(window); // Exibe o framebuffer
        glfwPollEvents();        // Trata eventos de entrada
    }

    glfwTerminate();
    return 0;
}
