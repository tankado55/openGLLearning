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
uniform int u_RowCount;
uniform int u_ColCount;
uniform float u_Distance;

void main()
{
    TexCoords = aTexCoords;
    float xPos = (gl_InstanceID % u_ColCount) * u_Distance;
    float zPos = int(gl_InstanceID / u_ColCount);
    gl_Position = u_MVP * vec4(aPos, 1.0);
    //gl_Position = u_MVP * vec4(aTexCoords.x, aTexCoords.y, 0.0, 1.0);

}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}