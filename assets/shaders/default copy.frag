#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;


// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the current position from the Vertex Shader
in vec3 crntPos;

// Gets the Texture Units from the main function
// uniform sampler2D diffuse0;
// uniform sampler2D specular0;
// uniform sampler2D normal0;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D normal0;

uniform bool hasTexture;
uniform bool hasNormalMap;
// Gets the color of the light from the main function
uniform vec4 lightColor;
// Gets the position of the light from the main function
uniform vec3 lightPos;
// Gets the position of the camera from the main function
uniform vec3 camPos;

uniform vec3 materialAmbientColor;
uniform vec3 materialDiffuseColor;
uniform vec3 materialSpecularColor;
uniform float opacity;



vec4 pointLight()
{	
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = lightPos - crntPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 1.00f;
	float b = 0.70f;
	float inten = 1.0f / (a + b * dist + dist * dist) + 0.5f;

	// ambient lighting
	float ambient = 0.05f;

	// diffuse lighting
	// Normals are mapped from the range [0, 1] to the range [-1, 1]
	vec3 normal = normalize(Normal);
	if (hasNormalMap)
	{
		normal = normalize(texture(normal0, texCoord).xyz * 2.0f - 1.0f);
	}
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specular = 0.0f;
	if (diffuse != 0.0f)
	{
		float specularLight = 0.50f;
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 halfwayVec = normalize(viewDirection + lightDirection);
		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * specularLight;
	};

	if (hasTexture)
	{
		vec3 retColor = (texture(diffuse0, texCoord).rgb * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * lightColor.rgb;
		return vec4(retColor, opacity);
	}
	else
	{
		vec3 retColor = (materialDiffuseColor * (diffuse * inten + ambient) + materialSpecularColor * specular * inten) * lightColor.rgb;
		return vec4(materialDiffuseColor, opacity);

	}
}

vec4 direcLight()
{
	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	if (hasNormalMap)
	{
		normal = normalize(texture(normal0, texCoord).xyz * 2.0f - 1.0f);
	}
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specular = 0.0f;
	if (diffuse != 0.0f)
	{
		float specularLight = 0.50f;
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 halfwayVec = normalize(viewDirection + lightDirection);
		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * specularLight;
	};

	
	if (hasTexture)
	{
		vec3 retColor = (texture(diffuse0, texCoord).rgb * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor.rgb;
		return vec4(retColor, opacity);
	}
	else
	{
		vec3 retColor = (materialDiffuseColor * (diffuse + ambient) + materialSpecularColor * specular) * lightColor.rgb;
		return vec4(retColor, opacity);

	}


}

void main()
{
	// outputs final color
	FragColor = direcLight();
}