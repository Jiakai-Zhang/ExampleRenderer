#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float scale;

void main()
{
    gl_PointSize = scale;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Color = aColor/255;

}



