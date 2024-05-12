#version 430 core

layout (local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

layout(rgba32f, binding = 0) uniform image3D imgOutput;

layout(location = 0) uniform int _Octaves;
layout(location = 1) uniform int _CellSize;
layout(location = 2) uniform int _AxisCellCount;
layout(location = 3) uniform float _Amplitude;
layout(location = 4) uniform float _Warp;
layout(location = 5) uniform float _Add;
layout(location = 6) uniform int _Seed;

float hash(uint n) {
    // integer hash copied from Hugo Elias
    n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 0x789221U) + 4294967295;
    return float(n & uint(0x7fffffffU)) / float(0x7fffffff);
}

float worley(vec3 coord, int axisCellCount) {
    ivec3 cell = ivec3(floor(coord / _CellSize));

    vec3 localSamplePos = vec3(coord / _CellSize - cell);

    float dist = 1.0f;

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            for (int z = -1; z <= 1; ++z) {
                ivec3 cellCoordinate = cell + ivec3(x, y, z);
                

                if (cellCoordinate.x == -1 || cellCoordinate.x == axisCellCount || cellCoordinate.y == -1 || cellCoordinate.y == axisCellCount || cellCoordinate.z == -1 || cellCoordinate.z == axisCellCount) {
                    ivec3 wrappedCellCoordinate = (cellCoordinate + axisCellCount) % axisCellCount;
                    int wrappedCellIndex = wrappedCellCoordinate.x + axisCellCount * (wrappedCellCoordinate.y + wrappedCellCoordinate.z * axisCellCount);
                    vec3 featurePointOffset = vec3(cellCoordinate) + vec3(hash(_Seed + wrappedCellIndex), hash(_Seed + wrappedCellIndex * 2), hash(_Seed + wrappedCellIndex * 3));
                    dist = min(dist, distance(cell + localSamplePos, featurePointOffset));
                }
                else {
                    int cellIndex = cellCoordinate.x + axisCellCount * (cellCoordinate.y + cellCoordinate.z * axisCellCount);
                    vec3 featurePointOffset = cellCoordinate + vec3(hash(_Seed + cellIndex), hash(_Seed + cellIndex * 2), hash(_Seed + cellIndex * 3));
                    dist = min(dist, distance(cell + localSamplePos, featurePointOffset));
                }
            }
        }
    }

    dist = sqrt(1.0f - dist * dist);
    dist *= dist * dist * dist * dist * dist;
    return dist;
}

void main()
{
	//Convert to UV
    uvec3 pos = gl_GlobalInvocationID.xyz;
    
    float n = 0.0f;

    float G = exp2(-1);
    float f = 1.0f;
    float a = 1.0f;

    for (int i = 0; i < _Octaves; ++i) {
        float noise = worley(pos.xyz * f + i * _Warp, int(_AxisCellCount * f)) * a;
        n += noise;
        f *= 2.0f;
        a *= G;
    }

    n += _Add;
    n = clamp(n, 0.0, 1.0) * _Amplitude;
    
    imageStore(imgOutput, ivec3(pos), vec4(_Amplitude - n));
}