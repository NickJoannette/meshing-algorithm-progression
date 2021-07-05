#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 PlayerPos;

void main()
{   

	vec3 GLOBAL_LIGHT_POSITION = vec3(433300,433300,433300);
	vec3 GLOBAL_LIGHT_COLOR = vec3(1.0,1.0,1.0);
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(GLOBAL_LIGHT_POSITION - FragPos); 

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * GLOBAL_LIGHT_COLOR;
	float distance    = length(GLOBAL_LIGHT_POSITION - FragPos);
	//float attenuation = 1.0 / (0.32 + 0.032 * distance +  0.007 * (distance * distance));    
    //diffuse *= attenuation;
	vec3 diffuseColor = vec3(0.14,0.22,0.56);
	vec3 result = diffuse * diffuseColor;
	FragColor = vec4(result,1.0); 
};

