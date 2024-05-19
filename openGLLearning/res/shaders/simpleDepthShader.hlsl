#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 u_Proj;
uniform mat4 u_View;
uniform mat4 u_Ortho;
uniform mat4 u_Model;

void main()
{
    gl_Position = u_Proj * u_View * u_Model * vec4(aPos, 1.0);
}


#shader fragment
#version 330 core

void main()
{
    // gl_FragDepth = gl_FragCoord.z; this is effectively what happens behind the scene anyways.
}