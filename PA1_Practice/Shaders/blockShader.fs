#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;


void main()
{   

	vec3 GLOBAL_LIGHT_POSITION = vec3(300,300,-300);
	vec3 GLOBAL_LIGHT_COLOR = vec3(1.0,1.0,1.0);
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(GLOBAL_LIGHT_POSITION - FragPos); 

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * GLOBAL_LIGHT_COLOR;
	
	vec3 result = diffuse * vec3(0.4-0.015*FragPos.y,0.4+0.013*FragPos.y,0.26 + 0.01*FragPos.y);
	FragColor = vec4(result,1.0); 
};

