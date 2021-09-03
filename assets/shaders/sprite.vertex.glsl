
#version 430

in vec4 position;
in vec4 normal;

out vec2 tx0;

void main(){
	gl_Position=position;
	tx0=position.xy;

}