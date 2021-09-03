#version 430

in vec4 attribNormal;
out vec4 oColor;


in vec2 tx0;
uniform sampler2D tex0;

void main(){
	oColor=texture2D(tex0,(tx0/2.+vec2(0.5,0.5)));
	//oColor=vec4(1,0,0,1);

}