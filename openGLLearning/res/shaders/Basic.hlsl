#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

out vec3 v_Normal;
out vec4 v_Pos;

uniform mat4 u_MVP;
uniform mat4 u_Model;

void main()
{
   gl_Position = u_MVP * position;

   v_Normal = mat3(transpose(inverse(u_Model))) * normal; // Inversing matrices is a costly operation for shaders, do it in the CPU
   v_Pos = position * u_Model;
};

#shader fragment
#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec4 v_Pos;

uniform vec4 u_Color;
uniform vec3 u_ViewPosition;
uniform Material u_Material;
uniform Light u_Light;


void main()
{
    // Ambient
    vec3 ambient = u_Material.ambient * u_Light.ambient;
    
    // Diffusive
    vec3 lightDir = normalize(u_Light.position - vec3(v_Pos));
    float diffusiveScalar = max(dot(v_Normal, lightDir), 0.0);
    vec3 diffuse = (diffusiveScalar * u_Material.diffuse) * u_Light.diffuse;

    // Specular
    vec3 viewDir = normalize(u_ViewPosition - vec3(v_Pos));
    vec3 reflectDir = reflect(-lightDir, v_Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
    vec3 specular = (spec * u_Material.specular) * u_Light.specular;

    // Result
    vec3 result = ambient + diffuse + specular;
    color = vec4(result, 1.0);
};