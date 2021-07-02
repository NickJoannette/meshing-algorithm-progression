#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aState;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 world;
uniform mat4 projection;

out float state;


void main()
{
   state = aState;
   gl_Position = projection * view * world * model * transform * vec4(aPos, 1.0);
};