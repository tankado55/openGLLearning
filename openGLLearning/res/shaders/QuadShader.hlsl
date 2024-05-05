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


uniform vec4 u_CameraWorldPos;
uniform samplerBuffer voxelBuffer;
uniform mat4 toVoxelLocal;
uniform vec3 resolution;

in vec4 worldPos;
out vec4 color;



vec3 smokeColor = vec3(0.33, 0.34, 0.33);
float maxDistance = 100.0;

int getVoxelIndex(vec3 pos)
{
    vec4 localPoint = toVoxelLocal * vec4(pos, 1.0);
    if (localPoint.x <= 0 || localPoint.y <= 0 || localPoint.z <= 0)
    {
        return -1;
    }
    if (localPoint.x >= resolution.x || localPoint.y >= resolution.y || localPoint.z >= resolution.z)
    {
        return -1;
    }
    int index1D = int(int(localPoint.x) + (int(localPoint.y) * resolution.x) + (int(localPoint.z) * resolution.x * resolution.y));
    if (index1D < (resolution.x * resolution.y * resolution.z)) // useless doublecheck
    {
        return index1D;
    }
    return -1;
}


float densityDefaultSample = 0.1;
float stepSize = 0.2;
float extinctionCoefficient = 1.0;

float calcFogFactor()
{
    vec3 rayDir = vec3(normalize(worldPos));

    float alpha = 1.0f;
    float accumDensity = 0.0f;
    float thickness = 0.0;
    for (int i = 0; i * stepSize < maxDistance; i++)
    {
        vec3 worldPointToCheck = vec3(u_CameraWorldPos) + (rayDir * i * stepSize);
        int index1D = getVoxelIndex(worldPointToCheck);

        if (index1D != -1)
        {
            float texelData = texelFetch(voxelBuffer, index1D).r;
            if (texelData < 0.00)
            {
                break;
            }

            //sample the point and get a density based on trilinear and noise
            //accumDensity += getDensity(samplePos) * _VolumeDensity;
            if (texelData >= 0.99)
            {
                float sampledDensity = densityDefaultSample;
                accumDensity += sampledDensity;
                thickness += stepSize * sampledDensity;
                alpha = exp(-thickness * accumDensity * extinctionCoefficient);
            }
        }
    }
    return accumDensity;

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
    if (fogFactor >= densityDefaultSample)
    {
        color = vec4(smokeColor, fogFactor);
    }
    else
    {
        discard;
    }
};