#version 150 core

in vec3 Color;
in vec2 Texcoord;
flat in int Method;

out vec4 outColor;

uniform sampler2D texDim;
uniform sampler2D texBright;


void main() {
   //outColor = texture(texR, Texcoord)+texture(texG, Texcoord);
   vec4 l = texture(texDim, Texcoord);
   float d = .004;
   vec4 r = texture(texBright, Texcoord+vec2(0,0));
   vec4 r1 = texture(texBright, Texcoord+vec2(d,0));
   vec4 r2 = texture(texBright, Texcoord+vec2(-d,0));
   vec4 r3 = texture(texBright, Texcoord+vec2(0,d));
   vec4 r4 = texture(texBright, Texcoord+vec2(0,-d));

  r = .2*r + .2 * r1 + .2*r2 + .2*r3 + .2*r4;

   vec4 dimC = vec4(l.rgb,1);
   vec4 brightC = vec4(r.rgb,1);
   
   if (Method == 0){ //Simple Color Map
   	 outColor = dimC + brightC;
   }
   else if (Method == 1){ //Dim
     outColor = dimC;
   }
   else if (Method == 2){ //Bright
     outColor = brightC;
   }

}