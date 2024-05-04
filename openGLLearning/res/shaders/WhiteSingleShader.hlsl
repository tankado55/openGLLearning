#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;


uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_Model;


void main()
{
    mat4 mvp = u_Projection * u_View * u_Model;
    gl_Position = mvp * vec4(aPos, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

uniform float green;


void main()
{
    FragColor = vec4(1.0, green, 0.1, 1.0);
}