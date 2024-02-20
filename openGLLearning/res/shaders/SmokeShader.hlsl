#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 fragCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 u_MVP;
uniform int u_XCount;
uniform int u_YCount;
uniform int u_ZCount;
uniform float u_Distance;

void main()
{
    TexCoords = aTexCoords;
    float xPos = (gl_InstanceID % u_ZCount) * u_Distance;
    float yPos = (int((gl_InstanceID / u_XCount)) % u_YCount) * u_Distance;
    float zPos = int(gl_InstanceID / (u_XCount * u_YCount)) * u_Distance;
    vec4 offset = vec4(xPos, yPos, zPos, 1.0);
    gl_Position = u_MVP * (vec4(aPos, 1.0) + offset);
    fragCoord = offset;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 fragCoord;

uniform sampler2D texture_diffuse1;
const uint k = 1103515245U;  // GLIB C

vec3 hash(uvec3 x)
{
    x = ((x >> 8U) ^ x.yzx) * k;
    x = ((x >> 8U) ^ x.yzx) * k;
    x = ((x >> 8U) ^ x.yzx) * k;

    return vec3(x) * (1.0 / float(0xffffffffU));
}

void main()
{
    FragColor = vec4(hash(uvec3(fragCoord)), 1.0);
}

