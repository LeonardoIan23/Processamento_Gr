// Inclusão das bibliotecas para OpenGL e criação de janela
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Função para criar um triângulo com as coordenadas especificadas
GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2) {
    GLuint VAO, VBO;  // Identificadores do Vertex Array Object e Vertex Buffer Object

    // Array com as coordenadas dos vértices do triângulo
    float vertices[] = {
        x0, y0, 0.0f,
        x1, y1, 0.0f,
        x2, y2, 0.0f
    };

    // Geração dos identificadores para VAO e VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Associação (bind) do VAO
    glBindVertexArray(VAO);

    // Associação do VBO e envio dos dados dos vértices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Definição de como interpretar os dados de vértices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  // Ativa o atributo de vértice

    // Desvincula os buffers para evitar modificações indesejadas
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Retorna o identificador do VAO criado
    return VAO;
}

int main() {
    // Inicializa a biblioteca GLFW
    if (!glfwInit()) return -1;

    // Cria uma janela de 800x600 pixels chamada "Ex1"
    GLFWwindow* window = glfwCreateWindow(800, 600, "Ex1", NULL, NULL);
    glfwMakeContextCurrent(window);  // Torna o contexto da janela atual

    // Inicializa o GLAD para gerenciar funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    // Códigos fontes dos shaders embutidos no código
    const char* vShader = "#version 400\nlayout(location=0) in vec3 position;\nvoid main(){ gl_Position = vec4(position, 1.0); }";
    const char* fShader = "#version 400\nout vec4 color;\nvoid main(){ color = vec4(0.3,0.5,1.0,1.0); }";

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

    // Cria o triângulo especificando as coordenadas
    GLuint vao = createTriangle(-0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f);

    // Loop principal da aplicação (render loop)
    while (!glfwWindowShouldClose(window)) {
        // Limpa o buffer de cor
        glClear(GL_COLOR_BUFFER_BIT);

        // Utiliza o programa de shader compilado
        glUseProgram(program);

        // Ativa o VAO com o triângulo e desenha ele
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);  // Desenha o triângulo com 3 vértices

        // Troca os buffers da janela (double buffering)
        glfwSwapBuffers(window);

        // Processa eventos (teclado, mouse, etc.)
        glfwPollEvents();
    }

    // Libera os recursos e finaliza a execução da GLFW
    glfwTerminate();
    return 0;
}
