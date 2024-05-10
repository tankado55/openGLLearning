#version 430 core

layout (local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

layout(rgba32f, binding = 0) uniform image3D imgOutput;

void main()
{
	//Convert to UV
    uvec3 pos = gl_GlobalInvocationID.xyz;
    
    float n = 0.0f;

    float G = exp2(-1);
    float f = 1.0f;
    float a = 1.0f;

    for (int i = 0; i < _Octaves; ++i) {
        float noise = worley(pos.xyz * f + i * _Warp, _AxisCellCount * f) * a;
        n += noise;
        f *= 2.0f;
        a *= G;
    }

    n += _Add;
    n = saturate(n) * _Amplitude;
    
    _RWNoiseTex[id.xyz] = _InvertNoise ? _Amplitude - n : n;
}