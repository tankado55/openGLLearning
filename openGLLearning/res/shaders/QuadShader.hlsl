#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 uv;

uniform mat4 u_Projection;
uniform mat4 u_View;

out vec4 worldPos;


void main()
{
   gl_Position = vec4(aPos.x, aPos.y, 1.0, 1.0);

   vec4 eye_coords = inverse(u_Projection) * gl_Position;
   vec4 world_coords = inverse(u_View) * eye_coords;
   worldPos = world_coords;
   gl_Position = u_Projection * u_View * world_coords;
};


#shader fragment
#version 330 core

float near = 0.1;
float far = 100.0;

uniform vec4 u_CameraWorldPos;
uniform samplerBuffer voxelBuffer;
uniform mat4 toVoxelLocal;
uniform vec3 resolution;

in vec4 worldPos;
out vec4 color;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));
}

float stepSize = 0.1;

float calcFogFactor() {
    float fogFactor = 0.0;
    float cameraToPixelDist = length(worldPos - u_CameraWorldPos);
    vec3 rayDir = vec3(normalize(worldPos - u_CameraWorldPos));

    for (int i = 0; i * stepSize < cameraToPixelDist; i++)
    {
        vec3 worldPointToCheck = vec3(u_CameraWorldPos) + (rayDir * i * stepSize);
        vec4 localOrigin = toVoxelLocal * vec4(worldPointToCheck, 1.0);
        int index1D = int(int(localOrigin.x) + (int(localOrigin.y) * resolution.x) + (int(localOrigin.z) * resolution.x * resolution.y));

        float texelData = texelFetch(voxelBuffer, index1D).r;
        if (texelData >= 0.99)
        {
            return 1.00;
        }
    }
    return 0.00;

}

void main()
{
    //float depth = LinearizeDepth(gl_FragDepth) / far;
    //color = vec4(vec2(depth), 1.0, 1.0);
    //if (gl_FragCoord.x > 480)
    //{
    //    color = vec4(0.0, 1.0, 0.0, 1.0);
    //}
    //else {
    //    discard;
    //
    //}

    float fogFactor = calcFogFactor();
    if (fogFactor >= 0.99)
    {
        color = vec4(0.0, 1.0, 0.0, 1.0);
    }
    else
    {
        discard;
    }
};