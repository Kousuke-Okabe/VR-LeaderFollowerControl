#version 430

in vec4 attribNormal;
out vec4 oColor;

uniform vec4 lightway;
uniform vec4 lookway;

void main(){
	//oColor=vec4(vec3(max(0,dot(attribNormal.xyz,lightway.xyz))+max(0,dot(attribNormal.xyz,lookway.xyz))+0.2),1.0);
	oColor=vec4(1,1,1,1);

}