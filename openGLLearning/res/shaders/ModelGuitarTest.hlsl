#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 u_MVP;
uniform float u_Interpolation;

void main()
{
    TexCoords = aTexCoords;
    //gl_Position = u_MVP * vec4(aPos, 1.0);
    //gl_Position = u_MVP * vec4(aTexCoords.x, aTexCoords.y, 0.0, 1.0);

    vec3 fromTo = vec3(vec3(aTexCoords, 0.0) - aPos);

    gl_Position = u_MVP * vec4(aPos + (u_Interpolation * fromTo), 1.0);

}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
    FragColor = texture(texture_diffuse1, TexCoords);
}