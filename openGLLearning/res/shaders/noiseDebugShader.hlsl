#shader vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}



#shader fragment
#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler3D _NoiseTex;

void main()
{
    vec3 texCol = texture(_NoiseTex, vec3(TexCoords,0)).rgb;
    float result = texture(_NoiseTex, vec3(0.8)).r;
    if (result < 0.5f)
    {
        FragColor = vec4(vec3(1.0,0,0.2), 1.0);
    }
    else
    {
        FragColor = vec4(vec3(0.0,1.0,0), 1.0);
    }
    FragColor = vec4(texCol, 1.0);
    
}