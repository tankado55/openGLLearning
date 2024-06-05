#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec2 TexCoords;
out vec3 Normal;
out vec4 FragPos;

void main()
{
    mat4 mvp = u_Projection * u_View * u_Model;
    TexCoords = aTexCoords;
    Normal = mat3(transpose(inverse(u_Model))) * aNormal;
    FragPos = u_Model * vec4(aPos, 1.0);

    gl_Position = mvp * vec4(aPos, 1.0);
}


#shader fragment
#version 330 core

in vec2 TexCoords;
in vec3 Normal;
in vec4 FragPos;

out vec4 FragColor;


uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;

void main()
{
    vec3 lightColor = vec3(1.0,1.0,1.0);
    vec3 lightPos = vec3(-100.0, 0.0, 0.0);

    // ambient
    float ambientStrength = 0.9;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - vec3(FragPos));
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - vec3(FragPos));
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 objectColor = vec3(texture(texture_diffuse1, TexCoords));

    vec3 result = clamp((ambient + diffuse + specular), 0.0, 1.0) * objectColor;
    //vec3 result = 1.0 * objectColor;
    FragColor = vec4(result, 1.0);
}