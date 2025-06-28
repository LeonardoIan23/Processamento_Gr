#version 330 core
layout (location = 0) in vec4 vertex; // vec2 aPos, vec2 aTexCoords

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

// Uniforms para selecionar um trecho da spritesheet
uniform vec2 uvOffset;
uniform vec2 uvScale;

void main()
{
    // Transforma a coordenada de textura do quad (0-1) para o trecho correto
    TexCoords = vertex.zw * uvScale + uvOffset;
    gl_Position = projection * view * model * vec4(vertex.xy, 0.0, 1.0);
}
