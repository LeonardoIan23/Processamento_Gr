// Leonardo Ian de Oliveira
// Mauricio Costa
// AtividadeVivencialM2.cpp

// Bibliotecas para OpenGL, GLFW e GLM
#include <glad/glad.h>                  // Gerencia as funções OpenGL
#include <GLFW/glfw3.h>                 // Biblioteca para criar janelas e capturar entrada do teclado/mouse
#include <glm/glm.hpp>                  // Biblioteca matemática para vetores/matrizes
#include <glm/gtc/matrix_transform.hpp> // Transformações com GLM
#include <vector>                       // Armazena os vértices
#include <iostream>                     // Saída no console
#include <fstream>                      // Leitura de arquivos
#include <sstream>                      // Manipulação de strings
#include <cstdlib>                      // Funções de número aleatório

// Define o tamanho da janela (800x600 unidades)
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Estrutura para representar um vértice com posição e cor
struct Vertex
{
    glm::vec2 position; // Posição do vértice (x, y)
    glm::vec3 color;    // Cor do vértice (RGB)
};

// Lista de vértices clicados pelo usuário
std::vector<Vertex> vertices;

// Identificadores dos objetos OpenGL
GLuint VAO, VBO;
GLuint shaderProgram;

// Função que lê o conteúdo de um arquivo de shader (.glsl)
std::string loadShaderSource(const char *path)
{
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf(); // Lê tudo para a string
    return buffer.str();
}

// Função que compila um shader (vertex ou fragment)
GLuint compileShader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Verifica se a compilação teve sucesso
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Erro de compilacao de shader: " << infoLog << std::endl;
    }

    return shader;
}

// Cria e retorna o shader program final (vertex + fragment)
GLuint createShaderProgram()
{
    // Carrega os arquivos de shader
    std::string vertexCode = loadShaderSource("src/shaders/vertex.glsl");
    std::string fragmentCode = loadShaderSource("src/shaders/fragment.glsl");

    // Compila os shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode.c_str());
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());

    // Cria o programa de shader e linka os dois
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Libera os shaders intermediários
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

// Função de callback para ajustar o tamanho do viewport quando a janela for redimensionada
void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Função chamada quando o botão do mouse for pressionado
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos); // Captura a posição do clique

        // Converte a posição do clique para coordenadas de mundo (de -1 a 1)
        float x = (float)xpos;
        float y = SCREEN_HEIGHT - (float)ypos;

        float ndcX = (x / SCREEN_WIDTH) * 2.0f - 1.0f;
        float ndcY = (y / SCREEN_HEIGHT) * 2.0f - 1.0f;

        // Gera uma cor RGB aleatória
        glm::vec3 color = glm::vec3(
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX);

        // Adiciona o vértice à lista
        vertices.push_back({glm::vec2(ndcX, ndcY), color});

        // A cada 3 vértices, cria um novo triângulo
        if (vertices.size() % 3 == 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        }
    }
}

int main()
{
    // Inicializa a GLFW e configura a versão do OpenGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Cria a janela principal
    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Atividade Vivencial", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Erro ao criar janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Torna o contexto OpenGL atual e configura os callbacks
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    // Carrega as funções OpenGL com GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Erro ao inicializar GLAD" << std::endl;
        return -1;
    }

    // Cria o programa de shader
    shaderProgram = createShaderProgram();

    // Gera VAO e VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    // Inicializa o VBO com tamanho zero
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

    // Configura os atributos dos vértices
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0); // posição
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color)); // cor
    glEnableVertexAttribArray(1);

    // Desvincula os buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Loop principal da aplicação
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Cor de fundo
        glClear(GL_COLOR_BUFFER_BIT);         // Limpa o frame

        glUseProgram(shaderProgram);                    // Usa o shader program
        glBindVertexArray(VAO);                         // Usa o VAO
        glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // Desenha os triângulos criados

        glfwSwapBuffers(window); // Troca os buffers (double buffering)
        glfwPollEvents();        // Trata eventos do sistema (teclado/mouse/etc)
    }

    // Libera recursos
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // Finaliza a GLFW
    glfwTerminate();
    return 0;
}
