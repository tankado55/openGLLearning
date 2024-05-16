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
   gl_Position = vec4(aPos.x, aPos.y, 1.0, 1.0);

   vec4 world_coords = inverse(u_Projection) * gl_Position;
   //eye_coords = eye_coords.xyzw / world_coords.w;
   world_coords = inverse(u_View) * world_coords;
   worldPos = world_coords;
   gl_Position = u_Projection * u_View * world_coords;
   
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

uniform vec3 u_Ellipsoid;
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
uniform mat4 _CameraInvViewProjection;

uniform float near_plane;
uniform float far_plane;

in vec4 worldPos;
in vec2 uvCoord;
in mat4 projMatrix;
in mat4 viewMatrix;
out vec4 color;

float maxDistance = 7.5;
const float MAX_DISTANCE = 200;

float toLightMaxDistance = 4.0f;

float densityDefaultSample = 4.0; //4.0
float volumeDensity = densityDefaultSample * u_StepSize;

float shadowDensityDefault = 2.5; //2.5
float shadowDensity = shadowDensityDefault * u_LigthStepSize;

float extinctionCoefficient = u_AbsorptionCoefficient + u_ScatteringCoefficient;

vec3 ComputeWorldSpacePosition(vec2 positionNDC, float deviceDepth);

vec3 hash33(vec3 p3) {
    vec3 p = fract(p3 * vec3(.1031, .11369, .13787));
    p += dot(p, p.yxz + 19.19);
    return -1.0 + 2.0 * fract(vec3((p.x + p.y) * p.z, (p.x + p.z) * p.y, (p.y + p.z) * p.x));
}

float worley(vec3 p, float scale) {

    vec3 id = floor(p * scale);
    vec3 fd = fract(p * scale);

    float n = 0.;

    float minimalDist = 1.;


    for (float x = -1.; x <= 1.; x++) {
        for (float y = -1.; y <= 1.; y++) {
            for (float z = -1.; z <= 1.; z++) {

                vec3 coord = vec3(x, y, z);
                vec3 rId = hash33(mod(id + coord, scale)) * 0.5 + 0.5;

                vec3 r = coord + rId - fd;

                float d = dot(r, r);

                if (d < minimalDist) {
                    minimalDist = d;
                }

            }//z
        }//y
    }//x

    return 1.0 - minimalDist;
}

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
            v += 0.1;
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
                    value += weight1 * weight2 * weight3 * texelFetch(voxelBuffer, to1D(vi + vec3(i, j, k))).r;
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

    n = getNoise(pos);

    float dist = min(1.0f, length(vp / radius));
    float voxelDist = min(1.0f, 1 - (v / 16.0f));
    dist = max(dist, voxelDist);

    dist = smoothstep(_DensityFalloff, 1.0f, dist);

    falloff = min(1.0f, dist + n);
    
    return clamp(clamp(v, 0.0,1.0) * (1 - falloff), 0.0, 1.0);
}

vec3 ComputeWorldSpacePosition(vec2 positionNDC, float deviceDepth) {
    vec4 positionCS = vec4(positionNDC * 2.0 - 1.0, deviceDepth, 1.0);
    vec4 hpositionWS = _CameraInvViewProjection * positionCS;
    return hpositionWS.xyz / hpositionWS.w;
}
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

vec4 calcFogColor()
{
    vec3 col = u_SmokeColor;
    float alpha = 1.0f;
    vec3 rayDir = vec3(normalize(worldPos));
    //vec3 rayDir = _CameraInvViewProjection * vec4(uvCoord * 2 - 1, 0.0f, 1.0f).xyz;
    //rayDir = _CameraToWorld * float4(rayDir, 0.0f).xyz;

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
    
    if (vv <= 0) // no smoke found debug
    {
        return vec4(vec3(1.0,0.0,0.0), 0.2);
    }

    distanceTraveled -= 0.4f;
    worldPointToCheck = vec3(u_CameraWorldPos) + (distanceTraveled * rayDir);

    for (int i = 0; i * u_StepSize < maxDistance; i++)
    {
        worldPointToCheck = vec3(u_CameraWorldPos) + (distanceTraveled * rayDir) + (rayDir * i * u_StepSize);
        //worldPointToCheck = worldPointToCheck + vec3(0.25,0.25, 0.25);
        float depth = texture(_DepthMap, uvCoord).r;
        depth = LinearizeDepth(depth);
        vec3 sceneObstacle = ComputeWorldSpacePosition(uvCoord, depth);
        //sceneObstacle = vec3(projMatrix * vec4(sceneObstacle, 1.0));

        if (length(vec3(worldPointToCheck) - vec3(u_CameraWorldPos)) > length(sceneObstacle - vec3(u_CameraWorldPos)))
        {
            break;
        }


        //if (index1D != -1) // check if the index is valid
        //{
            int texelData = getVoxelValue(worldPointToCheck);
            //if (texelData == -1) // there is a scene object
            //{
            //    break;
           //}

            // check ellipsoid
            vec3 distanceVectorFromExplosion = vec3(worldPointToCheck - explosionPos);
            float distanceFromExplosion = length(distanceVectorFromExplosion / u_Ellipsoid);
            //if (distanceFromExplosion > 1.1) {
            //    continue;
            //}

            //if (texelData >= 0.99) // there is smoke
            //{
                //float cos_theta = dot(rayDir, float3(0, 1, 0));
                //float p = phase(_G, cos_theta);

                float sampledDensity = getDensity(worldPointToCheck);
                accumDensity += sampledDensity * volumeDensity;
                thickness += u_StepSize * sampledDensity;
                alpha = exp(-thickness * accumDensity * extinctionCoefficient);

                // To the light
                if (sampledDensity >= 0.0)
                {
                    float tau = 0.0f;
                    float accumDensityToLight = 0.0f;
                    for (int j = 0; j * u_LigthStepSize < toLightMaxDistance; j++)
                    {
                        vec3 worldPointToCheckToLight = vec3(worldPointToCheck) - (u_DirLight.direction * j * u_LigthStepSize);
                        vec3 distanceVectorFromExplosion = vec3(worldPointToCheckToLight - explosionPos);
                        float distanceFromExplosion = length(distanceVectorFromExplosion / u_Ellipsoid);
                        if (distanceFromExplosion > 1.1) {
                            break;
                        }
                        int index1D = getVoxelIndex(worldPointToCheckToLight);
                        if (index1D != -1) // check if the index is valid
                        {
                            float texelData = texelFetch(voxelBuffer, index1D).r;
                            if (texelData < -0.9) // wall
                            {
                                break;
                            }
                            //if (texelData >= 0.99) // there is smoke
                            //{
                                float sampledDensity = getDensity(worldPointToCheck);
                                tau += sampledDensity * shadowDensity;
                            //}
                        }
                    }
                    vec3 lightAttenuation = exp(-(tau / u_ExtinctionColor) * extinctionCoefficient * shadowDensity);
                    col += u_DirLight.color * lightAttenuation * alpha * u_ScatteringCoefficient * volumeDensity * sampledDensity; // TODO: reintroduce param p
                }
            //}
        //}
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