#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 PlayerPos;
uniform vec3 Test1;

void main()
{   

	vec3 GLOBAL_LIGHT_POSITION =  vec3(433300,433300,433300);
	vec3 GLOBAL_LIGHT_COLOR = vec3(1.0,1.0,1.0);
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(GLOBAL_LIGHT_POSITION - FragPos); 

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * GLOBAL_LIGHT_COLOR;
	float distance    = length(GLOBAL_LIGHT_POSITION - FragPos);
	//float attenuation = 1.0 / (0.32 + 0.032 * distance +  0.007 * (distance * distance));    
    //diffuse *= attenuation;
	vec3 diffuseColor = vec3(0.02*FragPos.y +.68 - (FragPos.y - 5) * 0.12, 0.03*FragPos.y, 0.05*FragPos.y);

vec3 ambient = vec3(0.25*diffuseColor);
	vec3 result = ambient + diffuse * diffuseColor;

if (FragPos == Test1) result = vec3(1.0,0,0);
	FragColor = vec4(result,1.0); 
};

