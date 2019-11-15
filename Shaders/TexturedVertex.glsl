#version 150 core

uniform mat4 modelMatrix ;
uniform mat4 viewMatrix ;
uniform mat4 projMatrix ;
uniform mat4 textureMatrix ;

uniform float time;

in vec3 position ;
in vec2 texCoord ;

out Vertex {
	vec2 texCoord;
} OUT;

void main (void) {

vec3 temp = position;

temp.y *= time;

mat4 mvp = projMatrix * viewMatrix * modelMatrix ;
gl_Position = mvp * vec4(temp, 1.0);


OUT.texCoord = (textureMatrix * vec4 (texCoord, 0.0, 1.0)).xy;
}