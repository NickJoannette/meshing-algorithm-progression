#version 330 core
out vec4 FragColor;
  
uniform vec3 ourColor; // we set this variable in the OpenGL code.

void main()
{
    FragColor = vec4(ourColor.xyz,1.0);
}   
