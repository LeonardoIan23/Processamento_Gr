/* Leonardo Ian de Oliveira */

// Inclusão das bibliotecas necessárias
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

#include <glad/glad.h>  // GLAD: Gerenciador de extensões do OpenGL
#include <GLFW/glfw3.h> // GLFW: Biblioteca para criar janelas e capturar entrada do usuário
#include <glm/glm.hpp>  // GLM: Biblioteca para operações matemáticas
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

// Estrutura que representa um triângulo com posição (x, y) e cor (r, g, b)
struct Triangle
{
    float x, y;    // Posição onde o triângulo será desenhado
    float r, g, b; // Cor RGB do triângulo
};

// Vetor que armazena todos os triângulos criados com clique
vector<Triangle> triangles;

// Identificadores para o VAO e o Shader
GLuint VAO;
GLuint shaderID;

// Matriz de projeção ortográfica
mat4 projection;

// Constantes para largura e altura da janela
const GLuint WIDTH = 800, HEIGHT = 600;

// Código-fonte do shader de vértice
const GLchar *vertexShaderSource = "#version 400\n"
                                   "layout (location = 0) in vec3 position;\n"
                                   "uniform mat4 projection;\n"
                                   "uniform mat4 model;\n"
                                   "void main()\n"
                                   "{\n"
                                   "gl_Position = projection * model * vec4(position, 1.0);\n"
                                   "}\0";

// Código-fonte do shader de fragmento
const GLchar *fragmentShaderSource = "#version 400\n"
                                     "uniform vec4 inputColor;\n"
                                     "out vec4 color;\n"
                                     "void main()\n"
                                     "{\n"
                                     "color = inputColor;\n"
                                     "}\n\0";

// Protótipos das funções de callback e de setup
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
GLuint setupShader();
GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2);

int main()
{
    // Inicializa gerador de números aleatórios
    srand(static_cast<unsigned int>(time(0)));

    // Inicializa o GLFW e cria a janela
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Exercicio 3 - Triangulos com clique", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Define as funções de callback para teclado e mouse
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Configura e ativa o shader
    shaderID = setupShader();
    glUseProgram(shaderID);

    // Configura a matriz de projeção ortográfica
    projection = ortho(0.0f, float(WIDTH), float(HEIGHT), 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

    // Cria o VAO de um triângulo padrão (com largura e altura de 100 unidades)
    VAO = createTriangle(-50.0f, -50.0f, 50.0f, -50.0f, 0.0f, 50.0f);

    // Loop principal do programa
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents(); // Processa eventos

        // Limpa a tela com cor de fundo escura
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);

        // Itera sobre todos os triângulos criados
        for (const Triangle &t : triangles)
        {
            mat4 model = mat4(1.0f); // Inicializa a matriz model como identidade

            // Aplica translação para posicionar o triângulo na posição clicada
            model = translate(model, vec3(t.x, t.y, 0.0f));

            // Envia a matriz model para o shader
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

            // Envia a cor do triângulo para o shader
            glUniform4f(glGetUniformLocation(shaderID, "inputColor"), t.r, t.g, t.b, 1.0f);

            // Desenha o triângulo
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glBindVertexArray(0);
        glfwSwapBuffers(window); // Troca os buffers, exibindo o conteúdo renderizado
    }

    // Libera os recursos e fecha o GLFW
    glfwTerminate();
    return 0;
}

// Função de callback para teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    // Se pressionar ESC, fecha a janela
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// Função de callback para clique do mouse
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    // Se o botão esquerdo do mouse for pressionado
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;

        // Obtém a posição do cursor do mouse
        glfwGetCursorPos(window, &xpos, &ypos);

        // Cria um novo triângulo na posição clicada
        Triangle tri;
        tri.x = static_cast<float>(xpos);
        tri.y = static_cast<float>(ypos);

        // Gera uma cor aleatória para o triângulo
        tri.r = static_cast<float>(rand() % 100) / 100.0f;
        tri.g = static_cast<float>(rand() % 100) / 100.0f;
        tri.b = static_cast<float>(rand() % 100) / 100.0f;

        // Adiciona o triângulo ao vetor
        triangles.push_back(tri);
    }
}

// Função para configurar e compilar os shaders
GLuint setupShader()
{
    // Cria e compila o vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Cria e compila o fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Cria o programa de shader e liga os shaders compilados
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Deleta os shaders pois já foram vinculados ao programa
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram; // Retorna o ID do programa de shader
}

// Função para criar o VAO de um triângulo com coordenadas especificadas
GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2)
{
    // Define os vértices do triângulo
    GLfloat vertices[] = {
        x0, y0, 0.0f,
        x1, y1, 0.0f,
        x2, y2, 0.0f};

    GLuint VBO, VAO;

    // Gera e configura o VAO e o VBO
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Envia os dados dos vértices para o buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configura o atributo de vértice (posição)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Desvincula o buffer e o VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO; // Retorna o identificador do VAO
}
