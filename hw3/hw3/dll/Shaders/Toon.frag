#version 430
uniform sampler2D texture;
uniform vec3 WorldLightPos;
uniform vec3 WorldCamPos;
uniform vec3 Kd;
uniform bool EdgeFlag;

in vec2 uv;
in vec3 normal;
in vec4 worldPos;

out vec4 color;

void main()
{
	// TODO : Calculate the Toon Shading with 5 level of threshold
	// Hint :
	//		  1. Calculate the color inteensity determined by the angles between the Light and normal vectors
	//		  2. The "albedo" variable is the color(texture) of a pixel
	//		  3. Using EdgeFlag to determine whether or not to add the edge color to "every pixel"
	//		  4. Using the angle between view and pixel to determine the edge intensity (be careful of that the pixel of edge should have higher intensity of edge color)
	//				- you can using the color you like to draw the edge

	vec4 albedo = texture2D(texture, uv);
	float edge_intensity = 0;
	vec4 diffuse;

	vec3 Light = normalize(WorldLightPos - worldPos.xyz);
	vec3 Viewer = normalize(WorldCamPos - worldPos.xyz);

	float intensity;
	float level = dot(Light, normal);
	if (level > 0.75) intensity = 0.8;
	else if (level > 0.30) intensity = 0.6;
	else intensity = 0.4;

	diffuse = vec4(Kd * albedo.xyz * intensity, 1.0); // must > 0

	float cos = dot(Viewer, normal);
	if(cos < 0.2)
	{
		edge_intensity = 1;
	}
	vec4 edge_color = vec4(1, 1, 1, 1)  * pow(edge_intensity,5);
	// it's the color which only contain darker texture to show different shader, you need to change the output to toon shading result
	if(EdgeFlag)
	{
		color = vec4(diffuse.xyz + edge_color.xyz, 1.0);
	}
	else
	{
		color = diffuse;
	}

}