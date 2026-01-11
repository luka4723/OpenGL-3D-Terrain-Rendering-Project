#version 410 core
struct Material {
	sampler2D texture_diffuse1;
//sampler2D texture_specular1;
	float shininess;
};

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define NR_POINT_LIGHTS 4

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform sampler2D lampa;
uniform vec2 screen;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

in vec3 FragPos;

out vec4 final_colour;

uniform sampler2D normal_map;
uniform float uni_phase;
uniform vec3 uni_camera_pos;
uniform vec3 normal;

void main()
{
	vec2 uv_coord = FragPos.xz;

	vec2 uv_coord1 = vec2(uv_coord.x + uni_phase * 1.53, uv_coord.y + uni_phase * 1.15);
	vec2 uv_coord2 = vec2(uv_coord.x + uni_phase * 0.97, uv_coord.y + uni_phase * 1.87);
	vec2 uv_coord3 = vec2(uv_coord.x + uni_phase * 2.03, uv_coord.y + uni_phase * 1.11);

	vec3 accum1 = texture(normal_map, uv_coord1).rgb;
	vec3 accum2 = texture(normal_map, uv_coord2).rgb;
	vec3 accum3 = texture(normal_map, uv_coord3).rgb;
	vec3 total = normalize((accum1 + accum2 + accum3));

	float sky_colour_factor = total.x;
	sky_colour_factor = clamp(sky_colour_factor, 0.0, 1.0);

	vec3 to_camera_vec = normalize(FragPos - uni_camera_pos);

	float specular_factor = dot(reflect(normalize(vec3(1.0, 1.0, 1.0)), total.xyz), to_camera_vec);
	specular_factor = clamp(specular_factor, 0.0, 1.0);
	specular_factor = pow(specular_factor, 256.0);

	vec3 diffuse_color = mix(vec3(0.02, 0.02, 0.5), vec3(0.4, 0.6, 0.8), sky_colour_factor);

	final_colour = vec4(diffuse_color,0.0);

	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = CalcDirLight(dirLight, normal, viewDir);

	for(int i = 0; i < NR_POINT_LIGHTS; i++)
	result += CalcPointLight(pointLights[i], normal, FragPos, viewDir);

	result += CalcSpotLight(spotLight,normal, FragPos, viewDir);

	final_colour = vec4(final_colour.xyz*result+ (vec3(1.0, 0.9, 0.85) * specular_factor)*0.3,0.6);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse * diff ;
	vec3 specular = light.specular * spec ;
	return (ambient + diffuse + specular);
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse * diff ;
	vec3 specular = light.specular * spec ;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse * diff ;
	vec3 specular = light.specular * spec ;
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	vec2 torchFactor = gl_FragCoord.xy/screen;
	torchFactor.x -= 0.2f;
	return (ambient + diffuse + specular) * vec3(texture(lampa, torchFactor));
}