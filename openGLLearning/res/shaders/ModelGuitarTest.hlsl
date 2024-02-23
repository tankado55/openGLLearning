#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 7) in float aScaleUV;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 u_MVP;
uniform float u_Interpolation;

uniform mat4 u_AvgRotation;

void main()
{
    TexCoords = aTexCoords;
    //gl_Position = u_MVP * vec4(aPos, 1.0);
    //gl_Position = u_MVP * vec4(aTexCoords.x, aTexCoords.y, 0.0, 1.0);

    mat4 mvp = projection * view * model;

    vec3 pos2d = vec3(aTexCoords, 0.0) * aScaleUV;
    vec3 pos3d = aPos;

    gl_Position = mvp * vec4( mix(pos3d, pos2d, u_Interpolation), 1.0);

}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform float u_TextureGridMode;
uniform float u_TextureColorMode;

const vec3 plainColor = vec3(1.0);

vec3 gridColor(vec2 p, vec3 defaultColor)
{
    const float nCells = 30;
    vec2 q = fract(p * nCells);
    if (q.x < 0.1) return vec3(0.8, 0.0, 0.0);
    if (q.y < 0.1) return vec3(0.0, 0.5, 0.0);
    return defaultColor;
}

void main()
{


    vec3 colTexture = texture(texture_diffuse1, TexCoords).rgb;
    colTexture = mix(plainColor, colTexture, u_TextureColorMode);
    vec3 gridTexture = gridColor(TexCoords, colTexture);

    FragColor.rgb = mix(
        colTexture,
        gridTexture,
        u_TextureGridMode
    );
    FragColor.a = 1.0;
}