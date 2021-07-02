#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;

void main()
{
	Normal = aNorm;
	FragPos = vec3(model * vec4(aPos,1.0));   
   
   gl_Position = projection * view * world * model * transform * vec4(aPos, 1.0);
};