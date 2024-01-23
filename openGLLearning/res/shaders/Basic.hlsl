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

layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec4 v_Pos;

uniform vec4 u_Color;
uniform vec3 u_LightColor;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPosition;


void main()
{
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * u_LightColor;
    
    // Diffusive
    vec3 lightDir = normalize(u_LightPos - vec3(v_Pos));
    float diffusiveScalar = max(dot(v_Normal, lightDir), 0.0);
    vec3 diffuse = diffusiveScalar * u_LightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(u_ViewPosition - vec3(v_Pos));
    vec3 reflectDir = reflect(-lightDir, v_Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // This 32 value is the shininess value of the highlight.
    vec3 specular = specularStrength * spec * u_LightColor;


    // Result
    vec3 result = (ambient + diffuse + specular) * vec3(u_Color);
    color = vec4(result, u_Color.w);
};