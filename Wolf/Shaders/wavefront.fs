#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D floorTexture;

uniform vec3 lightPos;
uniform bool useLight;

uniform vec3 viewPos;
uniform bool blinn;
uniform float textureFactor;
uniform vec3 kd;

uniform int colselector_x;
uniform int colselector_y;

uniform float sprite_texture_width;
uniform float sprite_texture_height;



void main()
{           
    
    vec2 TexCoords = vec2(fs_in.TexCoords.x * sprite_texture_width + colselector_x * sprite_texture_width, fs_in.TexCoords.y * sprite_texture_height + colselector_y * sprite_texture_height);
    vec3 color = textureFactor * texture(floorTexture, TexCoords).rgb;
    
    vec3 k_dw = kd * (1.0f - textureFactor);
    color = k_dw + color;

    // ambient
    vec3 ambient = 0.05 * color;

    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    if (useLight){
        diffuse = diff * color;
    } else {
        diffuse = color;
    }

    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = 0.0;

    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    vec3 specular = vec3(0.3) * spec; // assuming bright white light color
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}