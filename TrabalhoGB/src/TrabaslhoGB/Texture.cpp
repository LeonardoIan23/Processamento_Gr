// src/Texture.cpp

#include "Texture.h"
#include <iostream>

// Inclui o cabeçalho da stb_image, mas SEM a diretiva de implementação.
// Assim, este ficheiro apenas "vê" as declarações das funções,
// enquanto a implementação fica em outro lugar.
#include "stb_image.h"

// Construtor padrão.
Texture2D::Texture2D() : Width(0), Height(0), ID(0) {}

// Carrega uma imagem de um ficheiro para criar a textura.
void Texture2D::load(const char* file, bool alpha) {
    std::cout << "--> Tentando carregar a imagem: '" << file << "'" << std::endl;
    stbi_set_flip_vertically_on_load(true);
    int nrChannels;
    unsigned char* data = stbi_load(file, &Width, &Height, &nrChannels, 0);
    if (data) {
        std::cout << ">>> SUCESSO: Imagem '" << file << "' carregada." << std::endl;
        GLenum format = GL_RGB;
        if (nrChannels == 4 || alpha) format = GL_RGBA;
        this->create(data, Width, Height, format);
    } else {
        std::cout << "!!! Falha ao carregar textura: '" << file << "'" << std::endl;
    }
    stbi_image_free(data);
}

// Cria o objeto de textura do OpenGL a partir de dados brutos de píxeis.
void Texture2D::create(unsigned char* data, int width, int height, GLenum format) {
    this->Width = width;
    this->Height = height;
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Ativa (bind) esta textura para uso.
void Texture2D::bind() const {
    glBindTexture(GL_TEXTURE_2D, ID);
}