#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 voxelCoord;

uniform mat4 u_MVP;
uniform int u_XCount;
uniform int u_YCount;
uniform int u_ZCount;
uniform float u_VoxelSize;

uniform vec3 u_Ellipsoid;

void main()
{
    float xPos = (gl_InstanceID % u_ZCount) * u_VoxelSize;
    float yPos = (int((gl_InstanceID / u_XCount)) % u_YCount) * u_VoxelSize;
    float zPos = int(gl_InstanceID / (u_XCount * u_YCount)) * u_VoxelSize;
    vec3 offset = vec3(xPos, yPos, zPos);
    gl_Position = u_MVP * vec4(aPos + offset, 1.0);
    voxelCoord = uvec3(offset);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 voxelCoord;

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
    FragColor = vec4(hash(uvec3(voxelCoord)), 1.0);
}

