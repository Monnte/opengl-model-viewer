#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;


in vec3 Normal;
in vec2 texCoord;
in vec3 crntPos;
in vec3 lightPositions[4];
in vec3 camPos;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D normal0;
uniform sampler2D metallic0;

uniform bool hasTexture;
uniform bool hasNormalMap;
uniform bool hasSpecularMap;
uniform bool hasMetalicRoughnessMap;


uniform vec4 lightColors[4];

uniform vec3 materialAmbientColor;
uniform vec3 materialDiffuseColor;
uniform vec3 materialSpecularColor;
uniform float materialMetallic;
uniform float materialRoughness;
uniform float materialShininess;
uniform float opacity;


const float PI = 3.14159265359;

float distributionGGX(float NdotH, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float denom = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
	denom = PI * denom * denom;
	return a2 / max(denom, 0.0000001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}

float geometrySchlickGGX(float NdotV, float NdotL, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float ggx1 = NdotV / (NdotV * (1.0 - k) + k);
	float ggx2 = NdotL / (NdotL * (1.0 - k) + k);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float HdotV, vec3 baseReflectivity)
{
	return baseReflectivity + (1.0 - baseReflectivity) * pow((1.0 - HdotV), 5.0);
}


void main()
{
	vec3 albedo = materialDiffuseColor;
	if (hasTexture)
	{
		albedo = texture(diffuse0, texCoord).rgb;
	}

	vec3 N = normalize(Normal);
	if (hasNormalMap)
	{
		N = normalize(texture(normal0, texCoord).rgb * 2.0 - 1.0);
	}
	vec3 V = normalize(camPos - crntPos);

	vec3 baseReflectivity = mix(vec3(0.04), albedo, materialMetallic);

	float metalic = materialMetallic;
	if (hasMetalicRoughnessMap)
	{
		metalic = texture(metallic0, texCoord).b;
	}

	float roughness = materialRoughness;
	if (hasMetalicRoughnessMap)
	{
		roughness = texture(metallic0, texCoord).g;
	}


	vec3 Lo = vec3(0.0);
	for (int i = 0; i < 4; i++)
	{
		vec3 L = normalize(lightPositions[i] - crntPos);
		vec3 H = normalize(V + L);
		float distance = length(lightPositions[i] - crntPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightColors[i].rgb * attenuation;

		float NdotV = max(dot(N, V), 0.0000001);
		float NdotL = max(dot(N, L), 0.0000001);
		float HdotV = max(dot(H, V), 0.0);
		float NdotH = max(dot(N, H), 0.0);

		float D = distributionGGX(NdotH, roughness);
		float G = geometrySchlickGGX(NdotV, NdotL, roughness);
		vec3 F = fresnelSchlick(HdotV, baseReflectivity);

		vec3 specular = (D * G * F) / (4.0 * NdotV * NdotL);

		vec3 kD = vec3(1.0) - F;

		kD*= (1.0 - metalic);

		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}
	// //loop trought lighs
	// vec3 L = normalize(lightPos - crntPos);
	// vec3 H = normalize(V + L);
	// float distance = length(lightPos - crntPos);
	// float attenuation = 1.0 / (distance * distance);
	// vec3 radiance = lightColor.rgb * attenuation;

	// float NdotV = max(dot(N, V), 0.0000001);
	// float NdotL = max(dot(N, L), 0.0000001);
	// float HdotV = max(dot(H, V), 0.0);
	// float NdotH = max(dot(N, H), 0.0);

	// float D = distributionGGX(NdotH, roughness);
	// float G = geometrySchlickGGX(NdotV, NdotL, roughness);
	// vec3 F = fresnelSchlick(HdotV, baseReflectivity);

	// vec3 specular = (D * G * F) / (4.0 * NdotV * NdotL);

	// vec3 kD = vec3(1.0) - F;

	// kD*= (1.0 - metalic);

	// Lo += (kD * albedo / PI + specular) * radiance * NdotL;

	// // end

	vec3 ambient = vec3(0.03) * albedo;

	vec3 color = ambient + Lo;

	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0 / 2.2));

	FragColor = vec4(color, opacity);
}