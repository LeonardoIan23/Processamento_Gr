#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec2 pos;
uniform float offsetX;
uniform float offsetY;
uniform float frameW;
uniform float frameH;

void main()
{
    gl_Position = vec4(aPos + pos, 0.0, 1.0);
    TexCoord = vec2(aTexCoord.x * frameW + offsetX, aTexCoord.y * frameH + offsetY);
}