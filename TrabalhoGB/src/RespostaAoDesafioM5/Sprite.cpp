#include "Sprite.h"
#include "gl_utils.h"
#include "stb_image.h"
#include <iostream>

// --- Construtor (apenas uma definição) ---
Sprite::Sprite() {
    shader_programme = 0;
    texture = 0;
    VAO = 0; 
    VBO = 0; 
    EBO = 0; 

    posx = 0.0f;
    posy = 0.0f;
    speed = 0.8f; 

    nAnimations = 0; 
    nFrames = 0;
    currentFrame = 0; 
    currentAnimation = 0;
    
    isMoving = false; // Começa parado

    animation_speed = 0.15f;
    last_frame_time = 0.0;

    frame_width = 0.0f; 
    frame_height = 0.0f;
}

// --- Novo Método ---
void Sprite::setMoving(bool moving) {
    isMoving = moving;
}

void Sprite::setup(const char* textureFile, int numAnimations, int numFrames) {
    nAnimations = numAnimations;
    nFrames = numFrames;
    frame_width = 1.0f / nFrames;
    frame_height = 1.0f / nAnimations;
    
    shader_programme = create_programme_from_files("_sprites_vs.glsl", "_sprites_fs.glsl");

    glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, nrChannels;
	unsigned char *data = stbi_load(textureFile, &width, &height, &nrChannels, 0);
	if (data) {
		if (nrChannels == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

    float vertices[] = {
		 0.1f,  0.1f,   1.0f, 0.0f,
		 0.1f, -0.1f,   1.0f, 1.0f,
		-0.1f, -0.1f,   0.0f, 1.0f,
		-0.1f,  0.1f,   0.0f, 0.0f 
	};
	unsigned int indices[] = { 0, 1, 3, 1, 2, 3 };

    glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void Sprite::update(double deltaTime) {
    if (isMoving) {
        double current_time = glfwGetTime();
        if ((current_time - last_frame_time) > animation_speed) {
            last_frame_time = current_time;
            currentFrame = (currentFrame + 1) % nFrames;
        }
    } else {
        currentFrame = 0;
    }
}

void Sprite::move(int direction, double deltaTime) {
    if (direction == 3) { posy += speed * deltaTime; currentAnimation = 3; } 
    else if (direction == 0) { posy -= speed * deltaTime; currentAnimation = 0; } 
    else if (direction == 1) { posx -= speed * deltaTime; currentAnimation = 1; } 
    else if (direction == 2) { posx += speed * deltaTime; currentAnimation = 2; }
}

void Sprite::draw() {
    glUseProgram(shader_programme);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    float offsetx = frame_width * (float)currentFrame;
    float offsety = frame_height * (float)currentAnimation;

    glUniform1f(glGetUniformLocation(shader_programme, "offsetX"), offsetx);
    glUniform1f(glGetUniformLocation(shader_programme, "offsetY"), offsety);
    glUniform2f(glGetUniformLocation(shader_programme, "pos"), posx, posy);
    glUniform1f(glGetUniformLocation(shader_programme, "frameW"), frame_width);
    glUniform1f(glGetUniformLocation(shader_programme, "frameH"), frame_height);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}