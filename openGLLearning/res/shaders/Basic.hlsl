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

   v_Normal = normal;
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

void main()
{
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * u_LightColor;
    
    // Diffusive
    vec3 lightDir = normalize(u_LightPos - vec3(v_Pos));
    float diffusiveScalar = max(dot(v_Normal, lightDir), 0.0);
    vec3 diffuse = diffusiveScalar * u_LightColor;

    // Result
    vec3 result = (ambient + diffuse) * vec3(u_Color);
    color = vec4(result, u_Color.w);
};