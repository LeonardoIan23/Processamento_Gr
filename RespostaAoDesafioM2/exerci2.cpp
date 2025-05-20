// Inclusão das bibliotecas para OpenGL, janela e utilitários
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

// Função que cria um triângulo com coordenadas passadas e retorna o VAO
GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2) {
    GLuint VAO, VBO;  // Identificadores do VAO (Vertex Array Object) e VBO (Vertex Buffer Object)

    // Array com as coordenadas dos vértices do triângulo
    float vertices[] = {x0, y0, 0.0f, x1, y1, 0.0f, x2, y2, 0.0f};

    // Gera e configura VAO e VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);  // Ativa o VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);  // Ativa o VBO

    // Copia os dados dos vértices para o buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configura o atributo de posição (layout location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  // Ativa o atributo

    // Desvincula o VBO e o VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;  // Retorna o identificador do VAO
}

int main() {
    // Inicializa a GLFW
    if (!glfwInit()) return -1;

    // Cria a janela e define o contexto OpenGL
    GLFWwindow* window = glfwCreateWindow(800, 600, "Ex2", NULL, NULL);
    glfwMakeContextCurrent(window);

    // Inicializa o GLAD para carregar as funções do OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    // Definição dos shaders como strings
    const char* vShader = "#version 400\nlayout(location=0) in vec3 position;\nvoid main(){ gl_Position = vec4(position, 1.0); }";
    const char* fShader = "#version 400\nout vec4 color;\nvoid main(){ color = vec4(0.8,0.3,0.3,1.0); }";

    // Compilação do Vertex Shader
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vShader, NULL);
    glCompileShader(vs);

    // Compilação do Fragment Shader
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fShader, NULL);
    glCompileShader(fs);

    // Criação do programa de shader e linkagem
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // Cria um vetor de VAOs, cada um representando um triângulo diferente
    std::vector<GLuint> VAOs = {
        createTriangle(-0.8f, -0.5f, -0.6f, 0.0f, -0.4f, -0.5f),  // Triângulo 1
        createTriangle(-0.2f, -0.5f,  0.0f, 0.0f,  0.2f, -0.5f),  // Triângulo 2
        createTriangle( 0.4f, -0.5f,  0.6f, 0.0f,  0.8f, -0.5f),  // Triângulo 3
        createTriangle(-0.6f,  0.2f, -0.4f, 0.7f, -0.2f, 0.2f),   // Triângulo 4
        createTriangle( 0.3f,  0.3f,  0.5f, 0.8f,  0.7f, 0.3f)    // Triângulo 5
    };

    // Loop principal da aplicação
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);  // Limpa o buffer de cor

        glUseProgram(program);  // Usa o programa de shaders

        // Renderiza cada triângulo
        for (GLuint vao : VAOs) {
            glBindVertexArray(vao);          // Ativa o VAO do triângulo
            glDrawArrays(GL_TRIANGLES, 0, 3); // Desenha o triângulo
        }

        // Troca os buffers para mostrar o resultado na tela
        glfwSwapBuffers(window);
        glfwPollEvents();  // Processa eventos (teclado, mouse etc.)
    }

    // Finaliza a GLFW e libera recursos
    glfwTerminate();
    return 0;
}
