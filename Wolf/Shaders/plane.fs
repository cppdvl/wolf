#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform vec3 color;

void main()
{           
    FragColor = vec4(color, 1.0);
}