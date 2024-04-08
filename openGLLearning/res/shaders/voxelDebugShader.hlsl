#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform int u_XCount;
uniform int u_YCount;
uniform int u_ZCount;

uniform vec3 u_Ellipsoid;
uniform samplerBuffer voxelBuffer;

out float isFull;
out vec4 color;

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
    float xPos = gl_InstanceID % u_ZCount;
    float yPos = (int((gl_InstanceID / u_XCount)) % u_YCount);
    float zPos = int(gl_InstanceID / (u_XCount * u_YCount));
    vec3 offset = vec3(xPos, yPos, zPos);
    gl_Position = u_Projection * u_View * u_Model * vec4(aPos + offset, 1.0);

    color = vec4(hash(uvec3(offset)), 1.0);
    if (gl_InstanceID == 1058)
    {
        color = vec4(1.0, 0.0, 0.0, 0.5);
    }
    else
    {
        color = vec4(0.0, 0.0, 1.0, 0.1);
    }
    if (gl_InstanceID == 0)
    {
        color = vec4(0.0, 1.0, 0.0, 0.5);
    }

    float data = texelFetch(voxelBuffer, gl_InstanceID).r;
    isFull = data;
}


#shader fragment
#version 330 core
out vec4 FragColor;

in float isFull;
in vec4 color;


void main()
{
    if (isFull > 0.99 && isFull < 1.01) {
        FragColor = color;
        //FragColor = vec4(0.2, 1.0, 0.2, 1.0);
    }
    else {
        //FragColor = vec4(1.0,0.0,0.0, 0.1);
        //FragColor = color;
        discard;
    }
}

