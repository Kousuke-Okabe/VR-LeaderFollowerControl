#version 430

in vec4 attribNormal;
out vec4 oColor;

uniform vec4 lightway;
uniform vec4 lookway;

float powerAbs(float a){
	return (a<0.0)?-a:0.8*a;
}

void main(){
	//oColor=vec4(powerAbs(attribNormal.x),powerAbs(attribNormal.y),powerAbs(attribNormal.z),1.0);
	oColor=vec4(1,1,1,1);

}