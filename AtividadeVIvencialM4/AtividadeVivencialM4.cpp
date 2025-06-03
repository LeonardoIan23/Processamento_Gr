// Leonardo Ian de Oliviera
// Gustavo Scheffel

//Atividade Vivencial Modulo 4

// Bibliotecas padrão e para gráficos/imagem
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION // Define a implementação do stb_image
#include <stb_image.h>

#include <glm/glm.hpp> // GLM para matemática
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Constantes Globais
const GLuint WIDTH = 800, HEIGHT = 600; // Dimensões da janela
float playerX = 400.0f; // Posição X lógica do jogador (controla o scroll)
float playerY = 20.0f;  // Posição Y do sprite do jogador na tela
const float PLAYER_SPRITE_WIDTH = 100.0f; // Largura do sprite do jogador
const float PLAYER_SPEED = 10.0f;         // Velocidade do jogador
const float PLAYER_SCREEN_X = (WIDTH / 2.0f) - (PLAYER_SPRITE_WIDTH / 2.0f); // Posição X fixa do sprite na tela

// Protótipos das Funções
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
GLuint loadTexture(const char* path);
GLuint setupShader();
GLuint createQuad();

// Estrutura para dados de cada camada de parallax
struct Layer {
    const char* name_key; // Chave da textura
    float speed;          // Velocidade de parallax
    float y_pos;          // Posição Y da base da camada
    float height;         // Altura da camada
};

// Vertex Shader: processa vértices, aplica transformações e scroll de textura
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord; // Coordenadas de textura UV originais

uniform mat4 model;
uniform mat4 projection;
uniform float u_TexScrollX; // Fator de scroll para parallax

out vec2 TexCoord; // Coordenadas de textura para o Fragment Shader

void main() {
    gl_Position = projection * model * vec4(position, 1.0);
    // Aplica scroll na coordenada X e inverte Y
    TexCoord = vec2(texCoord.x + u_TexScrollX, 1.0 - texCoord.y);
})";

// Fragment Shader: calcula a cor final de cada pixel (fragmento)
const char* fragmentShaderSource = R"(
#version 330 core
in vec2 TexCoord; // Coordenada de textura interpolada
out vec4 color;   // Cor final do fragmento
uniform sampler2D tex; // Sampler da textura

void main() {
    color = texture(tex, TexCoord); // Amostra a cor da textura
})";

// Função Principal
int main() {
    // Inicialização GLFW e GLAD
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Paralaxe - Leonardo (Ajustando Camadas)", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Falha ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Falha ao inicializar GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, WIDTH, HEIGHT); // Define a área de renderização

    // Setup de recursos gráficos
    GLuint shaderID = setupShader(); // Compila e linka shaders
    GLuint quadVAO = createQuad();   // Cria VAO/VBO para um quad
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WIDTH), 0.0f, static_cast<float>(HEIGHT)); // Projeção 2D

    // Carregamento de texturas usando um mapa
    // ATENÇÃO: "ceu.png" - verificar se o arquivo existe ou se era "ceu.jpg"
    std::map<std::string, GLuint> textures = {
        {"sky",                 loadTexture("ceu.png")}, 
        {"clouds1",             loadTexture("nuvem_1.png")},
        {"mountains",           loadTexture("pedras.png")},
        {"hills_far",           loadTexture("terra_2.png")},
        {"hills_near",          loadTexture("terra_1.png")},
        {"foreground_elements", loadTexture("terra_3.png")},
        {"plants_close",        loadTexture("plaanta.png")},
        {"player",              loadTexture("player.png")}
    };

    // Definição das camadas do cenário parallax (ajustadas pelo usuário)
    std::vector<Layer> layers = {
        {"sky",                 0.05f,  0.0f,   HEIGHT},//camda do ceu cobre toda a tela e se move muito devagar
        {"clouds1",             0.12f,  300.0f, 300.0f},//camada da nuvem se move um pouco mais rapido que o ceu
        {"mountains",           0.25f,  100.0f, 450.0f}, //camada da montanha ta ai igual as outras e se mais rapido 
        {"hills_far",           0.4f,   50.0f,  500.0f},//camada de terra 1 igual as outras coisas se move mais rapido tambem
        {"hills_near",          0.6f,   30.0f,  500.0f},//camada de terra 2 igual a de cima
        {"foreground_elements", 0.85f,  0.0f,   500.0f}, //camda de terra 3 mesma coisa da de cima e o tamanho é o mesmo 
        {"plants_close",        1.0f,   0.0f,   300.0f} // não sei pq a grama se move mais rapido mas ta se movendo 
    };

    // Configurações iniciais do OpenGL e uniforms
    glUseProgram(shaderID);
    glUniform1i(glGetUniformLocation(shaderID, "tex"), 0); // Associa sampler 'tex' à unidade de textura 0
    GLint texScrollX_location = glGetUniformLocation(shaderID, "u_TexScrollX");
    GLint model_location = glGetUniformLocation(shaderID, "model");
    GLint projection_location = glGetUniformLocation(shaderID, "projection");

    glEnable(GL_BLEND); // Habilita transparência
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection)); // Envia matriz de projeção

    // Loop Principal de Renderização
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // Processa eventos

        glClearColor(0.1f, 0.1f, 0.2f, 1.0f); // Limpa tela com cor de fundo
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(quadVAO); // Ativa VAO do quad

        // Renderiza camadas do cenário
        for (const auto& layer : layers) {
            if (textures.count(layer.name_key)) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textures[layer.name_key]);
                
                // Calcula e envia scroll da textura para parallax
                float scrollAmountPixels = (playerX - (WIDTH / 2.0f)) * layer.speed;
                float texScrollX = scrollAmountPixels / WIDTH;
                glUniform1f(texScrollX_location, texScrollX);

                // Define posição e escala da camada (estática horizontalmente)
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, layer.y_pos, 0.0f));
                model = glm::scale(model, glm::vec3(WIDTH, layer.height, 1.0f));
                glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
                
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Desenha a camada
            } else {
                 std::cerr << "Textura não encontrada para a chave: " << layer.name_key << std::endl;
            }
        }

        // Renderiza jogador
        glUniform1f(texScrollX_location, 0.0f); // Reseta scroll da textura para o jogador
        if (textures.count("player")) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures["player"]);
            
            // Define posição (fixa na tela) e escala do jogador
            glm::mat4 playerModel = glm::mat4(1.0f);
            playerModel = glm::translate(playerModel, glm::vec3(PLAYER_SCREEN_X, playerY, 0.0f));
            playerModel = glm::scale(playerModel, glm::vec3(PLAYER_SPRITE_WIDTH, 100.0f, 1.0f));
            glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(playerModel));
            
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Desenha o jogador
        }
        
        glfwSwapBuffers(window); // Troca buffers (double buffering)
    }

    // Limpeza de recursos
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteProgram(shaderID);
    for(auto const& pair_tex : textures) {
        glDeleteTextures(1, &pair_tex.second);
    }
    glfwTerminate(); // Finaliza GLFW
    return 0;
}

// Callback para tratamento de input do teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_LEFT) {
            playerX -= PLAYER_SPEED; // Atualiza posição lógica X do jogador/mundo
        }
        if (key == GLFW_KEY_RIGHT) {
            playerX += PLAYER_SPEED;
        }
    }
    // Limites de tela para playerX foram removidos para scroll infinito
}

// Configura, compila e linka os shaders (Vertex e Fragment)
GLuint setupShader() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Verificação de erros de compilação do Vertex Shader
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERRO::SHADER::VERTEX::COMPILACAO_FALHOU\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Verificação de erros de compilação do Fragment Shader
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERRO::SHADER::FRAGMENT::COMPILACAO_FALHOU\n" << infoLog << std::endl;
    }

    // Cria e linka o programa shader
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Verificação de erros de linkagem do programa
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERRO::SHADER::PROGRAMA::LINKAGEM_FALHOU\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader); // Deleta shaders individuais após linkagem
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

// Cria um quad (dois triângulos) com posições e coordenadas de textura
GLuint createQuad() {
    float vertices[] = { // Posição (x,y,z), Textura (u,v)
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        1.0f, 1.0f, 0.0f,  1.0f, 1.0f
    };
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO); // Cria Vertex Array Object
    glGenBuffers(1, &VBO);      // Cria Vertex Buffer Object

    glBindVertexArray(VAO);     // Ativa VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Ativa VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Envia dados dos vértices para VBO

    // Define atributos dos vértices (como ler os dados do VBO)
    // Atributo 0: Posição (3 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Atributo 1: Coordenada de Textura (2 floats)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Desvincula VBO
    glBindVertexArray(0);             // Desvincula VAO
    return VAO;
}

// Carrega uma textura de um arquivo de imagem usando stb_image
GLuint loadTexture(const char* path) {
    std::string basePath = "../src/AtividadeVivencialM4/"; // Caminho base para as texturas
    std::string fullPath = basePath + path;
    std::cout << "Carregando imagem: " << fullPath << std::endl;

    GLuint textureID;
    glGenTextures(1, &textureID);         // Gera ID da textura
    glBindTexture(GL_TEXTURE_2D, textureID); // Ativa textura

    // Define parâmetros de wrapping (repetição) e filtering (interpolação linear com mipmaps)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Carrega imagem
    int width, height, nrChannels;
    unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        // Determina formato (RGB, RGBA) e envia dados para GPU
        GLenum format = GL_RGB;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D); // Gera mipmaps
    } else {
        std::cerr << "Erro ao carregar imagem: " << fullPath << std::endl;
        std::cerr << "STB Error: " << stbi_failure_reason() << std::endl;
    }
    stbi_image_free(data); // Libera memória da imagem (CPU)
    glBindTexture(GL_TEXTURE_2D, 0); // Desvincula textura
    return textureID;
}