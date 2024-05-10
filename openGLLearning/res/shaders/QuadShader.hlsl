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
uniform float iTime;

uniform vec3 u_Ellipsoid;
uniform vec3 u_Radius;
uniform vec3 explosionPos;

uniform DirectionalLight u_DirLight;
uniform float u_AbsorptionCoefficient;
uniform float u_ScatteringCoefficient;
uniform vec3 u_ExtinctionColor;
uniform float _DensityFalloff;
uniform vec3 u_SmokeColor;
uniform vec3 u_VoxelSpaceBounds;

in vec4 worldPos;
out vec4 color;

float stepSize = 0.05;
float maxDistance = 150;

float toLightMaxDistance = 5.0f;

float densityDefaultSample = 4.0;
float volumeDensity = densityDefaultSample * stepSize;

float shadowDensityDefault = 2.5;
float lightStepSize = 0.25;
float shadowDensity = shadowDensityDefault * lightStepSize;

float extinctionCoefficient = u_AbsorptionCoefficient + u_ScatteringCoefficient;

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

float getDensity(vec3 pos)
{
    float v = 0;
    float n = 0.0f;
    float falloff = 0.0f;
    float heightMod = 0.0f;

    vec3 vp = pos - explosionPos;
    vec3 radius = u_Radius - 0.1f;

    v = getTrilinearVoxel(pos);

    vec2 uv = vec2(pos.x, pos.z) / vec2(u_Ellipsoid.x, u_Ellipsoid.z); //my
    vec3 p = vec3(uv, iTime * 0.1); //my
    n = worley(p * 2.0 - 1.0, 4.0);

    float dist = min(1.0f, length(vp / radius));
    float voxelDist = min(1.0f, 1 - (v / 16.0f));
    dist = max(dist, voxelDist);

    dist = smoothstep(_DensityFalloff, 1.0f, dist);

    falloff = min(1.0f, dist);
    //falloff = min(1.0f, dist + n);
    
    return clamp(clamp(v, 0.0,1.0) * (1 - falloff), 0.0, 1.0);
}

vec4 calcFogColor()
{
    vec3 col = u_SmokeColor;
    float alpha = 1.0f;
    vec3 rayDir = vec3(normalize(worldPos));

    float accumDensity = 0.0f;
    float thickness = 0.0;
    for (int i = 0; i * stepSize < maxDistance; i++)
    {
        vec3 worldPointToCheck = vec3(u_CameraWorldPos) + (rayDir * i * stepSize);
        

        int index1D = getVoxelIndex(worldPointToCheck);

        if (index1D != -1) // check if the index is valid
        {
            float texelData = texelFetch(voxelBuffer, index1D).r;
            if (texelData < 0.00) // there is a scene object
            {
                break;
            }

            // check ellipsoid
            vec3 distanceVectorFromExplosion = vec3(worldPointToCheck - explosionPos);
            float distanceFromExplosion = length(distanceVectorFromExplosion / u_Ellipsoid);
            if (distanceFromExplosion > 1.0) {
                continue;
            }

            if (texelData >= 0.99) // there is smoke
            {
                //float cos_theta = dot(rayDir, float3(0, 1, 0));
                //float p = phase(_G, cos_theta);

                float sampledDensity = getDensity(worldPointToCheck);
                accumDensity += sampledDensity * volumeDensity;
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
                                float sampledDensity = getDensity(worldPointToCheck);
                                tau += sampledDensity * shadowDensity;
                            }
                        }
                    }
                    vec3 lightAttenuation = exp(-(tau / u_ExtinctionColor) * extinctionCoefficient * shadowDensity);
                    col += u_DirLight.color * lightAttenuation * alpha * u_ScatteringCoefficient * volumeDensity * sampledDensity; // TODO: reintroduce param p
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
    if (fogFactor.w <= 0)
    {
        discard;
    }
    else
    {
        color = fogFactor;
    }
};