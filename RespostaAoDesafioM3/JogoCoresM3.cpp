// Bibliotecas padrão
#include <iostream> // Entrada e saída
#include <vector>   // Vetores dinâmicos
#include <cmath>    // Funções matemáticas
#include <ctime>    // Tempo para gerar aleatoriedade

// Bibliotecas OpenGL
#include <glad/glad.h>  // Carregador de funções OpenGL
#include <GLFW/glfw3.h> // Biblioteca para criar janelas e eventos

// Biblioteca para álgebra linear
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm; // Facilita o uso de vec3, mat4, etc.

// -------------------------
// Configurações do jogo
// -------------------------

const GLuint WIDTH = 800, HEIGHT = 600;           // Dimensões da janela
const GLuint ROWS = 6, COLS = 8;                  // Linhas e colunas da grade
const GLuint QUAD_WIDTH = 100, QUAD_HEIGHT = 100; // Tamanho de cada quadrado

const float dMax = sqrt(3.0); // Distância máxima para comparação de cores

// Estrutura representando cada quadrado
struct Quad
{
    vec3 position;   // Posição do quadrado
    vec3 dimensions; // Dimensões (largura, altura)
    vec3 color;      // Cor RGB
    bool eliminated; // Estado: eliminado ou não
};

// -------------------------
// Variáveis globais
// -------------------------

Quad grid[ROWS][COLS]; // A grade de quadrados
int iSelected = -1;    // Índice do quadrado selecionado
int score = 0;         // Pontuação
int attempts = 0;      // Número de tentativas

// -------------------------
// Shaders em GLSL
// -------------------------

// Vertex Shader: aplica transformações e envia posição
const char *vertexShaderSource = R"(
#version 400
layout (location = 0) in vec3 position;
uniform mat4 projection;
uniform mat4 model;
void main() {
    gl_Position = projection * model * vec4(position, 1.0);
}
)";

// Fragment Shader: define a cor do quadrado
const char *fragmentShaderSource = R"(
#version 400
uniform vec4 inputColor;
out vec4 color;
void main() {
    color = inputColor;
}
)";

// -------------------------
// Funções de eventos
// -------------------------

// Callback de teclado: ESC fecha a janela
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// Callback de mouse: seleciona quadrado clicado
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos); // Pega posição do clique

        int x = xpos / QUAD_WIDTH;
        int y = ypos / QUAD_HEIGHT;

        // Verifica se o clique está dentro da grade e se não foi eliminado
        if (x >= 0 && x < COLS && y >= 0 && y < ROWS && !grid[y][x].eliminated)
        {
            grid[y][x].eliminated = true;
            iSelected = x + y * COLS; // Armazena o índice selecionado
        }
    }
}

// -------------------------
// Criação do quadrado base
// -------------------------

GLuint createQuad()
{
    GLuint VAO, VBO;

    // Coordenadas do quadrado em espaço normalizado
    GLfloat vertices[] = {
        -0.5, 0.5, 0.0,  // Topo esquerdo
        -0.5, -0.5, 0.0, // Base esquerda
        0.5, 0.5, 0.0,   // Topo direito
        0.5, -0.5, 0.0   // Base direita
    };

    // Criação do buffer e transferência dos dados
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Criação do VAO e configuração do atributo
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Desvincula
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

// -------------------------
// Compilação dos shaders
// -------------------------

int setupShader()
{
    GLint success;
    GLchar infoLog[512];

    // Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "Erro Vertex Shader: " << infoLog << endl;
    }

    // Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "Erro Fragment Shader: " << infoLog << endl;
    }

    // Programa final
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Libera shaders intermediários
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

// -------------------------
// Eliminação de quadrados com cores similares
// -------------------------

int eliminarSimilares(float tolerancia)
{
    int eliminados = 0;

    int x = iSelected % COLS;
    int y = iSelected / COLS;

    vec3 C = grid[y][x].color; // Cor do quadrado selecionado

    // Percorre todos os quadrados
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (!grid[i][j].eliminated)
            {
                vec3 O = grid[i][j].color;

                // Distância euclidiana entre cores
                float d = sqrt(pow(C.r - O.r, 2) + pow(C.g - O.g, 2) + pow(C.b - O.b, 2));

                // Se estiver dentro da tolerância, elimina
                if ((d / dMax) <= tolerancia)
                {
                    grid[i][j].eliminated = true;
                    eliminados++;
                }
            }
        }
    }

    iSelected = -1; // Reseta seleção

    return eliminados;
}

// -------------------------
// Função principal do jogo
// -------------------------

int main()
{
    srand(time(0)); // Inicializa gerador aleatório

    glfwInit();

    // Cria a janela
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Jogo das Cores 🎨", NULL, NULL);
    glfwMakeContextCurrent(window);

    // Configura callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Inicializa GLAD
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);

    // Compila shaders e cria quadrado base
    GLuint shaderID = setupShader();
    GLuint VAO = createQuad();

    // Inicializa a grade com quadrados coloridos aleatoriamente
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            Quad quad;
            quad.position = vec3((j * QUAD_WIDTH) + QUAD_WIDTH / 2.0f,
                                 (i * QUAD_HEIGHT) + QUAD_HEIGHT / 2.0f, 0.0);
            quad.dimensions = vec3(QUAD_WIDTH, QUAD_HEIGHT, 1.0);
            quad.color = vec3(rand() % 256 / 255.0f,
                              rand() % 256 / 255.0f,
                              rand() % 256 / 255.0f);
            quad.eliminated = false;
            grid[i][j] = quad;
        }
    }

    glUseProgram(shaderID);

    GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");

    // Matriz de projeção ortográfica
    mat4 projection = ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

    // Loop principal
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);

        // Se houve clique, tenta eliminar similares
        if (iSelected > -1)
        {
            attempts++;
            int eliminados = eliminarSimilares(0.2f);
            score += eliminados * 10 - (attempts - 1) * 5;
            cout << "Tentativa " << attempts << ": " << eliminados << " eliminados | Pontuação: " << score << endl;
        }

        // Renderiza todos os quadrados não eliminados
        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j < COLS; j++)
            {
                if (!grid[i][j].eliminated)
                {
                    mat4 model = mat4(1.0f);
                    model = translate(model, grid[i][j].position);
                    model = scale(model, grid[i][j].dimensions);

                    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

                    glUniform4f(colorLoc, grid[i][j].color.r, grid[i][j].color.g, grid[i][j].color.b, 1.0f);

                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                }
            }
        }

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    // Fim do jogo
    cout << "\nJogo Finalizado!\nTentativas: " << attempts << "\nPontuação final: " << score << endl;

    glfwTerminate();
    return 0;
}
