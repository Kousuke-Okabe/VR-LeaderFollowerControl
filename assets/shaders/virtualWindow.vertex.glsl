
#version 430

uniform mat4 projection;
uniform mat4 lookAt;
uniform mat4 modelTransform;

in vec4 position;
in vec4 normal;
in vec2 texCoord0;

out vec2 genxy;
out vec2 tx0;

void main(){

	gl_Position=projection*lookAt*modelTransform*position;
	genxy=vec2(position.x,position.y);
	tx0=texCoord0;

}