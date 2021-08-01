
#version 430

uniform mat4 projection;
uniform mat4 lookAt;
uniform mat4 modelTransform;
in vec4 position;
in vec4 normal;

out vec4 attribNormal;

void main(){

//gl_Position=position*projection*lookAt*modelTransform;
//gl_Position=position*modelTransform*lookAt*projection;
gl_Position=vec4(position.xyz,1.0)*modelTransform*lookAt*projection;
attribNormal=normal*modelTransform;


}