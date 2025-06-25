// Diretivas para garantir que o conteúdo deste arquivo seja incluído apenas uma vez pelo compilador.
#ifndef SPRITE_H
#define SPRITE_H

// Inclui as bibliotecas gráficas necessárias.
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Definição da classe Sprite, que encapsula todos os dados e comportamentos do nosso personagem.
class Sprite {
public: // Métodos públicos: as ações que o nosso sprite pode executar.
    
    // Construtor: Inicializa o sprite com valores padrão.
    Sprite();
    
    // Configura o sprite: carrega a textura e define os parâmetros de animação.
    void setup(const char* textureFile, int nAnimations, int nFrames);
    
    // Atualiza a lógica da animação a cada frame.
    void update(double deltaTime);
    
    // Desenha o sprite na tela.
    void draw();
    
    // Move o sprite e atualiza a direção da animação.
    void move(int direction, double deltaTime);
    
    // Define se o sprite está em estado de 'movimento' ou 'parado'.
    void setMoving(bool moving);

private: // Atributos privados: as variáveis que guardam o estado interno do sprite.
    
    // --- Identificadores Gráficos ---
    GLuint shader_programme; // O programa de shader que o sprite usa.
    GLuint texture;          // A textura (spritesheet) do sprite.
    GLuint VAO, VBO, EBO;    // A geometria (o quadrado) do sprite.
    
    // --- Posição & Movimento ---
    float posx, posy; // A posição atual (x,y) do sprite na tela.
    float speed;      // A velocidade de movimento.

    // --- Animação ---
    int nAnimations;      // O número de animações (linhas no spritesheet).
    int nFrames;          // O número de quadros por animação (colunas).
    int currentFrame;     // O quadro atual da animação.
    int currentAnimation; // A animação (linha) atual.
    bool isMoving;        // O estado de movimento (true = andando, false = parado).

    // --- Controle de Tempo ---
    float animation_speed;   // A velocidade da animação.
    double last_frame_time; // O tempo do último quadro que foi trocado.

    // --- Propriedades da Textura ---
    float frame_width;  // A largura de um quadro na textura.
    float frame_height; // A altura de um quadro na textura.
};

#endif // SPRITE_H