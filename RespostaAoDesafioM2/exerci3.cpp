// Inclusão das bibliotecas necessárias
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>                      // Biblioteca para operações matemáticas (vetores, matrizes)
#include <glm/gtc/matrix_transform.hpp>     // Funções para transformações geométricas
#include <glm/gtc/type_ptr.hpp>             // Conversão de matrizes para ponteiros
#include <iostream>
#include <vector>

using namespace glm;  // Facilita o uso das funções do glm

// Estrutura que representa um triângulo com posição, tamanho e cor
struct Triangle {
    vec3 position;    // Posição do triângulo
    vec3 dimensions;  // Dimensões (escala)
    vec3 color;       // Cor RGB
};

// Função que cria um triângulo base (geometria padrão)
GLuint createBaseTriangle() {
    float vertices[] = {
         0.0f, 0.0f, 0.0f,   // Vértice 1
         0.1f, 0.1f, 0.0f,   // Vértice 2
         0.2f, 0.0f, 0.0f    // Vértice 3
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copia os dados dos vértices para o buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configura o atributo de posição
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return VAO;  // Retorna o identificador do VAO
}

int main() {
    glfwInit();  // Inicializa a GLFW

    // Cria uma janela de 800x600 pixels
    GLFWwindow* window = glfwCreateWindow(800, 600, "Ex3", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);  // Inicializa o GLAD

    // Código-fonte do vertex shader: aplica transformações de modelo e projeção
    const char* vShader = 
        "#version 400\n"
        "layout(location=0) in vec3 position;\n"
        "uniform mat4 model;\n"
        "uniform mat4 projection;\n"
        "void main(){ gl_Position = projection * model * vec4(position, 1.0); }";

    // Código-fonte do fragment shader: define a cor de saída
    const char* fShader = 
        "#version 400\n"
        "uniform vec3 inputColor;\n"
        "out vec4 color;\n"
        "void main(){ color = vec4(inputColor, 1.0); }";

    // Compilação do vertex shader
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vShader, NULL);
    glCompileShader(vs);

    // Compilação do fragment shader
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fShader, NULL);
    glCompileShader(fs);

    // Criação do programa de shader e linkagem
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // Cria o VAO de um triângulo base
    GLuint VAO = createBaseTriangle();

    // Vetor de triângulos com diferentes posições, tamanhos e cores
    std::vector<Triangle> triangles = {
        {{100, 100, 0}, {100, 100, 1}, {1.0f, 0.0f, 0.0f}},  // Vermelho
        {{300, 100, 0}, {100, 100, 1}, {0.0f, 1.0f, 0.0f}},  // Verde
        {{500, 100, 0}, {100, 100, 1}, {0.0f, 0.0f, 1.0f}},  // Azul
        {{200, 300, 0}, {100, 100, 1}, {1.0f, 1.0f, 0.0f}},  // Amarelo
        {{400, 300, 0}, {100, 100, 1}, {1.0f, 0.0f, 1.0f}}   // Magenta
    };

    glUseProgram(program);  // Ativa o programa de shaders

    // Define a matriz de projeção ortográfica (mesmas dimensões da janela)
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Loop principal de renderização
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);  // Limpa o buffer de cor

        // Renderiza cada triângulo
        for (auto& tri : triangles) {
            glm::mat4 model = glm::mat4(1.0f);  // Matriz identidade

            // Aplica a translação com base na posição do triângulo
            model = glm::translate(model, tri.position);

            // Aplica a escala com base nas dimensões
            model = glm::scale(model, tri.dimensions);

            // Envia a matriz de modelo para o shader
            glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));

            // Envia a cor para o shader
            glUniform3fv(glGetUniformLocation(program, "inputColor"), 1, glm::value_ptr(tri.color));

            // Ativa o VAO e desenha o triângulo
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glfwSwapBuffers(window);  // Troca os buffers para exibir o resultado
        glfwPollEvents();         // Processa eventos (teclado, mouse, etc.)
    }

    glfwTerminate();  // Finaliza a GLFW e libera recursos
    return 0;
}
