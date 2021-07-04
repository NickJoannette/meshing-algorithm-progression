#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 PlayerPos;

void main()
{   

	vec3 GLOBAL_LIGHT_POSITION = PlayerPos;//vec3(300,300,-300);
	vec3 GLOBAL_LIGHT_COLOR = vec3(0.9,0.9,0.08);
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(GLOBAL_LIGHT_POSITION - FragPos); 

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * GLOBAL_LIGHT_COLOR;
	float distance    = length(GLOBAL_LIGHT_POSITION - FragPos);
	float attenuation = 1.0 / (0.3 + 0.13 * distance +  0.012 * (distance * distance));    
  diffuse *= attenuation;
	
	vec3 result = diffuse* vec3(abs(sin(0.01*FragPos.x + 0.002*FragPos.y)),abs(sin(0.001*FragPos.y)),0.06 + 0.0028*FragPos.y);
	FragColor = vec4(result,1.0); 
};

