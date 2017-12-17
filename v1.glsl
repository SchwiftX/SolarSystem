#version 150 core

in vec2 texcoord;
in vec2 position;
flat out int Method;

uniform int method;
out vec3 Color;
out vec2 Texcoord;

void main() {
   gl_Position = vec4(position,0,1.0);
   Texcoord = texcoord;
   Method =  method;
}