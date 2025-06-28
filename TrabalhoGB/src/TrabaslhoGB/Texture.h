
#pragma once
#include <glad/glad.h> 

class Texture2D {
public:
    unsigned int ID;
    int Width, Height;
    Texture2D();
    void load(const char* file, bool alpha);
    void create(unsigned char* data, int width, int height, GLenum format);
    void bind() const;
};
