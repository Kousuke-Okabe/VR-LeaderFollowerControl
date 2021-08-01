
#version 430

uniform mat4 projection;
uniform mat4 lookAt;
uniform mat4 modelTransform;

in vec4 position;
in vec4 normal;

out vec2 genxy;

void main(){

	gl_Position=projection*lookAt*modelTransform*position;
	genxy=vec2(position.x,position.y);

}