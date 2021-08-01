#version 430

in vec2 genxy;
out vec4 oColor;

uniform vec4 lightway;
uniform vec4 lookway;
uniform sampler2D tex0;

void main(){
	//oColor=vec4(vec3(max(0,dot(attribNormal.xyz,lightway.xyz))+max(0,dot(attribNormal.xyz,lookway.xyz))+0.2),1.0);
	//oColor=vec4((genxy.x+1.)/2.,(genxy.y+1.)/2.,0,1);
	vec2 junkgen=(genxy+vec2(1,1))/2.;
	oColor=texture2D(tex0,vec2(junkgen.x,1-junkgen.y));
}