#version 330

layout(location = 0) in vec3 apos;
layout (location = 1) in vec2 atexcoord;

uniform mat4 Projection;
uniform mat4 ModelView;

out vec2 texcoord; //to fragment shader

void main() {
	gl_Position = Projection * ModelView * vec4(apos, 1.0);
	texcoord = atexcoord;
}