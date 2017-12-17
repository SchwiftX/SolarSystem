#version 150 core

in vec3 Color;
in vec3 normal;
in vec3 pos;
in vec3 lightDir;
in vec2 texcoord;

uniform float maxLum;
uniform float minLum;

out vec4 outColor;

uniform sampler2D tex;

//"   vec3 diffuseC = Color*max(dot(lightDir,normal),0.0);"
 // "   vec3 ambC = Color*ambient;" 
  //"   vec3 reflectDir = reflect(-lightDir,normal);" 
//  "   vec3 viewDir = normalize(-pos);"  //We know the eye is at 0,0
//  "   float spec = max(dot(reflectDir,viewDir),0.0);"
//  "   if (dot(lightDir,normal) <= 0.0) spec = 0;"
//  "   vec3 specC = vec3(.8,.8,.8)*pow(spec,4);"
//  "   outColor = vec4(ambC+diffuseC+specC, 1.0);"

const float ambient = .3;
void main() {
   //vec3 Color = texture(tex, texcoord).rgb;
   //vec3 Color = vec3(.2f,.2f,.6f);
   //vec3 Color = vec3(1.f,.2f,.1f); //Difficult color for 3D
   vec3 diffuseC = Color*max(dot(-lightDir,normal),0.0);
   vec3 ambC = Color*ambient;
   vec3 viewDir = normalize(-pos); //We know the eye is at (0,0)!
   vec3 reflectDir = reflect(lightDir,normal);
   float spec = max(dot(reflectDir,viewDir),0.0);
   if (dot(-lightDir,normal) <= 0.0)spec = 0;
   vec3 specC = .8*vec3(1.0,1.0,1.0)*pow(spec,4);
   vec3 oColor = ambC+diffuseC+specC;
   float lum = .3*oColor.r + .6*oColor.g + .1*oColor.b;
   if (lum < minLum || lum > maxLum){
      outColor = vec4(0,0,0,1);
   }
   else{
      outColor = vec4(oColor,1);
   }
}