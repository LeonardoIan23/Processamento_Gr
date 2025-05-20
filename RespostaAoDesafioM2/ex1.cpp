#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Função que cria um triângulo com os 3 vértices fornecidos
GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2) {
    // Define as coordenadas dos 3 vértices
    float vertices[] = {
        x0, y0,
        x1, y1,
        x2, y2
    };

    // Declaração dos objetos de buffer
    GLuint VAO, VBO;

    // Geração dos identificadores dos buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind do VAO e VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Envio dos dados dos vértices para o VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configura o atributo de posição (location = 0 no shader)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Retorna o identificador do VAO para renderização futura
    return VAO;
}
