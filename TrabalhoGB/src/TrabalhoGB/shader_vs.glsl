#version 330 core
layout (location = 0) in vec2 aPos;      // Posição dos vértices do nosso quadrado base
layout (location = 1) in vec2 aTexCoord; // Coordenadas de textura do quadrado base

out vec2 TexCoord; // Passa a coordenada de textura para o fragment shader

// Uniforms: variáveis enviadas pelo C++
uniform vec2 screenPos;    // Posição final (em pixels) do tile na tela
uniform vec2 tileSize;     // Tamanho do tile em pixels
uniform float texOffset;   // Deslocamento horizontal no tileset para escolher o tile
uniform float texTileWidth; // Largura de um tile na textura (ex: 1/7 = 0.14)

void main()
{
    // 1. Pega a posição base do vértice (de 0.0 a 1.0) e a escala pelo tamanho do tile em pixels.
    // 2. Adiciona a posição final na tela para mover o tile para o lugar certo.
    vec2 finalPos = aPos * tileSize + screenPos;

    // Converte a posição final de pixels para o sistema de coordenadas do OpenGL (-1 a 1)
    finalPos.x = (finalPos.x / 800.0) * 2.0 - 1.0; // 800 = largura da tela
    finalPos.y = (finalPos.y / 600.0) * 2.0 - 1.0; // 600 = altura da tela
    
    gl_Position = vec4(finalPos, 0.0, 1.0);

    // Calcula a coordenada da textura final para recortar o tile correto do tileset
    TexCoord = vec2(aTexCoord.x * texTileWidth + texOffset, aTexCoord.y);
}