#ifndef SPRITE_H
#define SPRITE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Sprite {
public:
    Sprite();
    void setup(const char* textureFile, int nAnimations, int nFrames);
    void update(double deltaTime);
    void draw();
    void move(int direction, double deltaTime);
    
    // Define se o sprite está se movendo ou não
    void setMoving(bool moving);

private:
    GLuint shader_programme;
    GLuint texture;
    GLuint VAO, VBO, EBO;
    
    float posx, posy;
    float speed;

    int nAnimations;
    int nFrames;
    int currentFrame;
    int currentAnimation;

    // --- NOVA VARIÁVEL ---
    bool isMoving; // Guarda o estado atual: true para movendo, false para parado

    float animation_speed;
    double last_frame_time;

    float frame_width;
    float frame_height;
};

#endif // SPRITE_H