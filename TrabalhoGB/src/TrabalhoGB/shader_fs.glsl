#version 330 core
out vec4 FragColor;

void main()
{
    // Força a cor de todos os pixels a ser vermelho sólido e 100% opaco
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}