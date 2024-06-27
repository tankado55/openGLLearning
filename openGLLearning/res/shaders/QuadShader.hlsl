#shader vertex
#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 uv;

uniform mat4 u_Projection;
uniform mat4 u_View;

out vec4 worldPos;
out vec2 uvCoord;
out mat4 projMatrix;
out mat4 viewMatrix;


void main()
{
   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);

   vec4 world_coords = inverse(u_Projection) * gl_Position;
   world_coords = inverse(u_View) * world_coords;
   //world_coords = world_coords.xyzw / world_coords.w;
   worldPos = world_coords;
   
   uvCoord = uv;
   viewMatrix = u_View;
   projMatrix = u_Projection;
};


#shader fragment
#version 420 core

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

layout(rgba32f, binding = 0) uniform image3D _NoiseTex;

uniform sampler2D _DepthMap;
uniform vec4 u_CameraWorldPos;
uniform samplerBuffer voxelBuffer;
uniform mat4 toVoxelLocal;
uniform vec3 resolution;
uniform float iTime;

uniform vec3 u_Radius;
uniform vec3 explosionPos;

uniform DirectionalLight u_DirLight;
uniform float u_AbsorptionCoefficient;
uniform float u_ScatteringCoefficient;
uniform vec3 u_ExtinctionColor;
uniform float _DensityFalloff;
uniform vec3 u_VoxelSpaceBounds;

uniform float _SmokeSize;
uniform vec3 _AnimationDirection;
// Smoke parameters
uniform vec3 u_SmokeColor;
uniform float u_StepSize;
uniform float u_LigthStepSize;
uniform float u_VolumeDensity;
uniform float u_ShadowDensity; //2.5

in vec4 worldPos;
in vec2 uvCoord;
in mat4 projMatrix;
in mat4 viewMatrix;
out vec4 color;

float maxDistance = 7.5;
const float MAX_DISTANCE = 200;

float toLightMaxDistance = 4.0f;

float volumeDensity = u_VolumeDensity * u_StepSize;

float shadowDensity = u_ShadowDensity * u_LigthStepSize;

float extinctionCoefficient = u_AbsorptionCoefficient + u_ScatteringCoefficient;

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


int to1D(vec3 pos) {
    return int(pos.x + pos.y * resolution.x + pos.z * resolution.x * resolution.y);
}


int getVoxelValue(vec3 pos) {
    pos.y -= u_VoxelSpaceBounds.y;

    if (abs(dot(pos, vec3(1, 0, 0))) <= u_VoxelSpaceBounds.x &&
        abs(dot(pos, vec3(0, 1, 0))) <= u_VoxelSpaceBounds.y &&
        abs(dot(pos, vec3(0, 0, 1))) <= u_VoxelSpaceBounds.z)
    {
        pos.y += u_VoxelSpaceBounds.y;
        //vec3 seedPos = pos;
        //seedPos.xz += u_VoxelSpaceBounds.xz;
        //seedPos /= u_VoxelSpaceBounds * 2;
        //seedPos *= resolution;

        int index = getVoxelIndex(pos);
        if (index == -1)
        {
            return -1;
        }
        float v = texelFetch(voxelBuffer, index).r;
        if (v > 0.9)
        {
            v += 0.2;
            return int(v);
        }
        else if (v < -0.9)
        {
            return -1;
        }
    }

    return 0;
}


float getTrilinearVoxel(vec3 pos)
{
    float v = 0;
    pos.y -= u_VoxelSpaceBounds.y;

    if (abs(dot(pos, vec3(1, 0, 0))) <= u_VoxelSpaceBounds.x &&
        abs(dot(pos, vec3(0, 1, 0))) <= u_VoxelSpaceBounds.y &&
        abs(dot(pos, vec3(0, 0, 1))) <= u_VoxelSpaceBounds.z)
    {
        pos.y += u_VoxelSpaceBounds.y;
        vec3 seedPos = pos;
        seedPos.xz += u_VoxelSpaceBounds.xz;
        seedPos /= u_VoxelSpaceBounds * 2;
        seedPos *= resolution;
        seedPos -= 0.5f;

        vec3 vi = floor(seedPos);

        float weight1 = 0.0f;
        float weight2 = 0.0f;
        float weight3 = 0.0f;
        float value = 0.0f;

        for (int i = 0; i < 2; ++i) {
            weight1 = 1 - min(abs(seedPos.x - (vi.x + i)), resolution.x);
            for (int j = 0; j < 2; ++j) {
                weight2 = 1 - min(abs(seedPos.y - (vi.y + j)), resolution.y);
                for (int k = 0; k < 2; ++k) {
                    weight3 = 1 - min(abs(seedPos.z - (vi.z + k)), resolution.z);
                    float texelFetched = texelFetch(voxelBuffer, to1D(vi + vec3(i, j, k))).r;
                    //texelFetched -= 1.0;
                    texelFetched = max(0.0, texelFetched);
                    value += weight1 * weight2 * weight3 * texelFetched;
                }
            }
        }

        v = value;
    }

    return v;
}


float getNoise(vec3 pos) {
    vec3 pos2 = pos + vec3(50.0);
    vec3 uvw = pos2 / _SmokeSize;
    uvw = uvw * 128.0;
    uvw += _AnimationDirection * (iTime) * 200.0;
    uvw = ivec3(uvw) % ivec3(128);
    float result = imageLoad(_NoiseTex, ivec3(uvw)).r;
    return result;
}


float getDensity(vec3 pos)
{
    float v = 0;
    float n = 0.0f;
    float falloff = 0.0f;
    float heightMod = 0.0f;

    vec3 vp = pos - explosionPos;
    vec3 radius = u_Radius - 0.1f;

    v = getTrilinearVoxel(pos);

    float dist = min(1.0f, length(vp / radius));
    float voxelDist = min(1.0f, 1.0f - (v / 9.0f));
    dist = max(dist, voxelDist);
    dist = max(dist, 1.0 - pos.y);

    dist = smoothstep(_DensityFalloff, 1.0f, dist);

    n = getNoise(pos);
    falloff = min(1.0f, dist + n);
    
    return clamp(clamp(v, 0.0,1.0) * (1.0f - falloff), 0.0, 1.0);
}





vec4 calcFogColor()
{
    vec3 col = u_SmokeColor;
    float alpha = 1.0f;
    vec3 rayDir = vec3(inverse(projMatrix) * vec4(uvCoord * 2 - 1, 0.0f, 1.0f));
    rayDir = vec3(inverse(viewMatrix) * vec4(rayDir, 0.0f));
    rayDir = normalize(rayDir);

    float accumDensity = 0.0f;
    float thickness = 0.0;

    float distanceTraveled = 0.0;
    vec3 worldPointToCheck = vec3(u_CameraWorldPos) + (distanceTraveled * rayDir);
    int vv = getVoxelValue(worldPointToCheck);
    while (vv <= 0 && distanceTraveled < MAX_DISTANCE) {
        distanceTraveled += 0.4;
        worldPointToCheck = vec3(u_CameraWorldPos) + (distanceTraveled * rayDir);
        vv = getVoxelValue(worldPointToCheck);
    }
    
    //if (vv <= 0) // no smoke found debug
    //{
    //    return vec4(vec3(1.0,0.0,0.0), 0.0);
    //}

    distanceTraveled -= 0.4f;
    worldPointToCheck = vec3(u_CameraWorldPos) + (distanceTraveled * rayDir);

    float depth = texture(_DepthMap, uvCoord).r;
    vec4 screenPosNDC = vec4(uvCoord.x, uvCoord.y, depth, 1.0) * 2.0 - 1.0;
    vec4 worldPosition = inverse(projMatrix * viewMatrix) * screenPosNDC;
    worldPosition = worldPosition / worldPosition.w;
    float sceneIntersectDistance = -((u_CameraWorldPos - worldPosition) / vec4(rayDir,1.0)).x;

    for (int i = 0; i * u_StepSize < maxDistance && distanceTraveled < sceneIntersectDistance; i++)
    {
        worldPointToCheck = vec3(u_CameraWorldPos) + (distanceTraveled * rayDir);
        distanceTraveled += u_StepSize;

        float sampledDensity = getDensity(worldPointToCheck);
        accumDensity += sampledDensity * volumeDensity;
        thickness += u_StepSize * sampledDensity;
        alpha = exp(-thickness * accumDensity * extinctionCoefficient);

        // To the light
        if (sampledDensity >= 0.001)
        {
            float tau = 0.0f;
            float accumDensityToLight = 0.0f;
            for (int j = 0; j * u_LigthStepSize < toLightMaxDistance; j++)
            {
                vec3 worldPointToCheckToLight = vec3(worldPointToCheck) - (u_DirLight.direction * j * u_LigthStepSize);
                
                int index1D = getVoxelIndex(worldPointToCheckToLight);
                if (index1D != -1) // check if the index is valid
                {
                    float texelData = texelFetch(voxelBuffer, index1D).r;
                            
                    float sampledDensity = getDensity(worldPointToCheck);
                    tau += sampledDensity * shadowDensity;
                }
            }
            vec3 lightAttenuation = exp(-(tau / u_ExtinctionColor) * extinctionCoefficient * shadowDensity);
            col += u_DirLight.color * lightAttenuation * alpha * u_ScatteringCoefficient * volumeDensity * sampledDensity;
        }
    }
    // Inbetween sample in the case of overshooting scene depth
    if (distanceTraveled > sceneIntersectDistance) {
        worldPointToCheck -= (distanceTraveled - sceneIntersectDistance) * rayDir;
        thickness -= distanceTraveled - sceneIntersectDistance;

        float v = getDensity(worldPointToCheck);
        float sampleDensity = v;
        accumDensity += volumeDensity * sampleDensity;
        alpha = exp(-thickness * accumDensity * extinctionCoefficient);
        if (v > 0.001f) {
            float tau = 0.0f;
            vec3 lightPos = worldPointToCheck;
            for (int j = 0; j * u_LigthStepSize < toLightMaxDistance; j++) {
                tau += v * shadowDensity;
                lightPos -= u_LigthStepSize * vec3(0, -1, 0);
                v = getDensity(lightPos);
            }

            vec3 lightAttenuation = exp(-(tau / u_ExtinctionColor) * extinctionCoefficient * shadowDensity);
            col += u_DirLight.color * lightAttenuation * alpha * u_ScatteringCoefficient * volumeDensity * sampleDensity;
        }
    }
    return vec4(col, 1.0 - alpha);
}


void main()
{
    vec4 fogFactor = calcFogColor();
    if (fogFactor.w <= 0.00001)
    {
        discard;
    }
    else
    {
        color = fogFactor;
    }
};