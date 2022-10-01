#version 430

uniform sampler2D texture;
uniform vec3 WorldCamPos;
uniform vec3 WorldLightPos;
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 La;
uniform vec3 Ld;
uniform vec3 Ls;
uniform int  gloss;
uniform bool EdgeFlag;

in vec2 uv;
in vec3 normal;
in vec4 worldPos;

out vec4 color;
void main()
{
	// TODO : Calculate the Blinn-Phong Shading
	// Hint :
	//		  1. See the formula of Phong-Blinn lighting model / reflection model (you can refer to "ICG_4Shading" slides)
	//		  2. For specular , gloss is the shininess coefficient and it's recommemded that 
	//				- using the quarter of gloss if you calculate sepcular in original Phong lighting model.
	//				- using the full of gloss if you calculate sepcular by halfway angle(Blinn-Phong).
	//		  3. The "albedo" variable is the color(texture) of a pixel
	//		  4. using EdgeFlag to determine whether or not to add the edge color to "every pixel"
	//		  5. using the angle between view and pixel to determine the edge intensity (be careful of that the pixel of edge should have higher intensity of edge color)
	//				- you can using the color you like to draw the edge

	vec4 albedo = texture2D(texture, uv);
	float edge_intensity = 0;

	vec3 Light = normalize(WorldLightPos - worldPos.xyz);
	vec3 Viewer = normalize(WorldCamPos - worldPos.xyz);
	vec3 Halfway = normalize(Light + Viewer);

	vec3 ambient = La * Ka * albedo.xyz;
	vec3 diffuse = Ld * Kd * albedo.xyz * dot(Light, normal); // must > 0
	vec3 specular = Ls * Ks * pow(dot(normal,Halfway), gloss);

	float cos = dot(Viewer, normal);
	if(cos < 0.2)
	{
		edge_intensity = 1;
	}
	vec4 edge_color = vec4(1, 1, 1, 1)  * pow(edge_intensity,5);

	// it's the color which only contain texture , you need to change the output to phong shading result 
	if(EdgeFlag)
	{
		color = vec4(ambient + diffuse + specular + edge_color.xyz, 1.0);
	}
	else
	{
		color = vec4(ambient + diffuse + specular, 1.0);
	}
} 