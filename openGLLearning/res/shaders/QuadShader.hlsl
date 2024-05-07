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

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};


uniform vec4 u_CameraWorldPos;
uniform samplerBuffer voxelBuffer;
uniform mat4 toVoxelLocal;
uniform vec3 resolution;

uniform vec3 u_Ellipsoid;
uniform vec3 explosionPos;

uniform DirectionalLight u_DirLight;
uniform float u_AbsorptionCoefficient;
uniform float u_ScatteringCoefficient;
uniform vec3 u_ExtinctionColor;

in vec4 worldPos;
out vec4 color;

vec3 smokeColor = vec3(0.33, 0.34, 0.33);
float maxDistance = 100.0;
float toLightMaxDistance = 5.0f;

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


float densityDefaultSample = 1.0;
float stepSize = 0.05;
float densityModificator = densityDefaultSample * stepSize; // TODO: change name

float shadowDensityDefault = 1.0;
float lightStepSize = 0.25;
float shadowDensity = shadowDensityDefault * lightStepSize;

float extinctionCoefficient = u_AbsorptionCoefficient + u_ScatteringCoefficient;

vec4 calcFogColor()
{
    vec3 col = smokeColor;
    float alpha = 1.0f;
    vec3 rayDir = vec3(normalize(worldPos));

    float accumDensity = 0.0f;
    float thickness = 0.0;
    for (int i = 0; i * stepSize < maxDistance; i++)
    {
        vec3 worldPointToCheck = vec3(u_CameraWorldPos) + (rayDir * i * stepSize);
        // check ellipsoid
        vec3 distanceVectorFromExplosion = vec3(worldPointToCheck - explosionPos);
        float distanceFromExplosion = length(distanceVectorFromExplosion / u_Ellipsoid);
        if (distanceFromExplosion > 1.0) {
            continue;
        }

        int index1D = getVoxelIndex(worldPointToCheck);

        if (index1D != -1) // check if the index is valid
        {
            float texelData = texelFetch(voxelBuffer, index1D).r;
            if (texelData < 0.00) // there is a scene object
            {
                break;
            }

            if (texelData >= 0.99) // there is smoke
            {
                float sampledDensity = 1.0;
                accumDensity += sampledDensity * densityModificator;
                thickness += stepSize * sampledDensity;
                alpha = exp(-thickness * accumDensity * extinctionCoefficient);

                // To the light
                if (sampledDensity > 0.001)
                {
                    float tau = 0.0f;
                    float accumDensityToLight = 0.0f;
                    float thicknessToLight = 0.0;
                    for (int j = 0; j * lightStepSize < toLightMaxDistance; j++)
                    {
                        vec3 worldPointToCheckToLight = vec3(worldPointToCheck) - (u_DirLight.direction * j * lightStepSize);
                        vec3 distanceVectorFromExplosion = vec3(worldPointToCheckToLight - explosionPos);
                        float distanceFromExplosion = length(distanceVectorFromExplosion / u_Ellipsoid);
                        if (distanceFromExplosion > 1.0) {
                            break;
                        }
                        int index1D = getVoxelIndex(worldPointToCheckToLight);
                        if (index1D != -1) // check if the index is valid
                        {
                            float texelData = texelFetch(voxelBuffer, index1D).r;
                            if (texelData < 0.00) // wall
                            {
                                break;
                            }
                            if (texelData >= 0.99) // there is smoke
                            {
                                float sampledDensity = 1.0;
                                tau += sampledDensity * shadowDensity;
                            }
                        }
                    }
                    vec3 lightAttenuation = exp(-(tau / u_ExtinctionColor) * extinctionCoefficient * shadowDensity);
                    col += u_DirLight.color * lightAttenuation * alpha * u_ScatteringCoefficient * densityModificator; // TODO: reintroduce param p
                }
            }
        }
    }
    return vec4(col, 1.0 - alpha);

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

    vec4 fogFactor = calcFogColor();
    if (fogFactor.w <= 0.1)
    {
        discard;
    }
    else
    {
        color = fogFactor;
    }
};