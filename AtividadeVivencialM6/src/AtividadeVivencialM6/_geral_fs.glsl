#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform float weight; // 0.0 para normal, > 0.0 para destaque

void main()
{
    vec4 texColor = texture(ourTexture, TexCoord);
    // Se a textura for transparente em algum ponto, descarta o pixel
    if(texColor.a < 0.1)
        discard;
        
    // Mistura a cor da textura com uma cor de destaque (azul)
    FragColor = mix(texColor, vec4(0.2, 0.2, 1.0, 1.0), weight);
}