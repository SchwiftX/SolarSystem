#version 150 core

in vec3 Color;
in vec3 normal;
in vec3 pos;
in vec3 lightDir;
in vec2 texcoord;

out vec4 outColor;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;
uniform sampler2D tex5;
uniform sampler2D tex6;
uniform sampler2D tex7;
uniform sampler2D tex8;

uniform int texID;
uniform int isCelShading;

uniform struct PointLight{
    vec3 position;
    vec3 intensity;
}pLight1,pLight2,pLight3,pLight4,pLight5,pLight6;

uniform vec3 aColor;
uniform vec3 dColor;
uniform vec3 sColor;
uniform int Ns;

uniform float u_numShades; // number of shades

const float ambient = .3;

// calculate diffuse component of lighting
float diffuseSimple(vec3 L, vec3 N){
    return clamp(dot(L,N),0.0,1.0);
}

// calculate specular component of lighting
float specularSimple(vec3 L,vec3 N,vec3 R){
    if(dot(N,L)>0){
        return pow(clamp(dot(L,R),0.0,1.0),4.0);
    }
    return 0.0;
}

void main() {
if(isCelShading==0)
{
   vec3 color;
   if (texID == -1)
   	 color = Color;
   else if (texID == 0)
     color = texture(tex0, texcoord).rgb;
   else if (texID == 1)
     color = texture(tex1, texcoord).rgb;
   else if (texID == 2)
       color = texture(tex2,texcoord).rgb;
   else if (texID == 3)
       color = texture(tex3,texcoord).rgb;
   else if (texID == 4)
       color = texture(tex4,texcoord).rgb;
   else if (texID == 5)
       color = texture(tex5,texcoord).rgb;
    else if (texID == 6)
       color = texture(tex6,texcoord).rgb;
    else if (texID == 7)
       color = texture(tex7,texcoord).rgb;
    else if (texID == 8)
        color = texture(tex8,texcoord).rgb;
   else{
   	 outColor = vec4(1,0,0,1);
   	 return; //This was an error, stop lighting!
   	}
    //add the diffuse/specular color with point light
    vec3 PLightDir = pLight1.position-pos;
    vec3 PLightDir2 = pLight2.position-pos;
    vec3 PLightDir3 = pLight3.position-pos;
    vec3 PLightDir4 = pLight4.position-pos;
    vec3 PLightDir5 = pLight5.position-pos;
    vec3 PLightDir6 = pLight6.position-pos;
    float fade_rate = pow(length(PLightDir),2);
    float fade_rate2 = pow(length(PLightDir2),2);
    float fade_rate3 = pow(length(PLightDir3),2);
    float fade_rate4 = pow(length(PLightDir4),2);
    float fade_rate5 = pow(length(PLightDir5),2);
    float fade_rate6 = pow(length(PLightDir6),2);
    PLightDir = normalize(PLightDir);
    PLightDir2 = normalize(PLightDir2);
    PLightDir3 = normalize(PLightDir3);
    PLightDir4 = normalize(PLightDir4);
    PLightDir5 = normalize(PLightDir5);
    PLightDir6 = normalize(PLightDir6);
    vec3 diffuseP1 = color*dColor*pLight1.intensity*(1/fade_rate)*max(dot(PLightDir,normal),0.0);
    vec3 diffuseP2 = color*dColor*pLight2.intensity*(1/fade_rate2)*max(dot(PLightDir2,normal),0.0);
    vec3 diffuseP3 = color*dColor*pLight3.intensity*(1/fade_rate3)*max(dot(PLightDir3,normal),0.0);
    vec3 diffuseP4 = color*dColor*pLight4.intensity*(1/fade_rate4)*max(dot(PLightDir4,normal),0.0);
    vec3 diffuseP5 = color*dColor*pLight5.intensity*(1/fade_rate5)*max(dot(PLightDir5,normal),0.0);
    vec3 diffuseP6 = color*dColor*pLight6.intensity*(1/fade_rate6)*max(dot(PLightDir6,normal),0.0);
    //printf("diffuseP: %f %f %f\n",diffuseP.x,diffuseP.y,diffuseP.z);
    
    vec3 diffuseC = dColor*color*max(dot(-lightDir,normal),0.0);
    vec3 ambC = aColor*ambient;
    vec3 viewDir = normalize(vec3(16,0,4)-pos); //We know the eye is at (0,0)!
    vec3 reflectDir = reflect(viewDir,normal);
    float spec = max(dot(reflectDir,lightDir),0.0);
    float specp1 = max(dot(reflectDir,PLightDir),0.0);
    float specp2 = max(dot(reflectDir,PLightDir2),0.0);
    if (dot(-lightDir,normal) <= 0.0)spec = 0;
    vec3 specC = .8*sColor*color*pow(spec,4);
    vec3 specP1 = .1*sColor*pow(specp1,4);
    vec3 specP2 = .1*sColor*pow(specp2,4);
    vec3 oColor;
    if(texID == 0)
    {
        oColor =vec3(1.2,1.2,1.2)*color;
    }
    else{
        oColor = ambC+diffuseC+specC+diffuseP1+diffuseP2+diffuseP3+diffuseP4+diffuseP5+diffuseP6;
    }
    outColor = vec4(oColor,1);
    }
    else if(isCelShading == 1){
        vec3 color;
        if (texID == -1)
            color = Color;
        else if (texID == 0)
            color = texture(tex0, texcoord).rgb;
        else if (texID == 1)
            color = texture(tex1, texcoord).rgb;
        else if (texID == 2)
            color = texture(tex2,texcoord).rgb;
        else if (texID == 3)
            color = texture(tex3,texcoord).rgb;
        else if (texID == 4)
            color = texture(tex4,texcoord).rgb;
        else if (texID == 5)
            color = texture(tex5,texcoord).rgb;
        else if (texID == 6)
            color = texture(tex6,texcoord).rgb;
        else if (texID == 7)
            color = texture(tex7,texcoord).rgb;
        else if (texID == 8)
            color = texture(tex8,texcoord).rgb;
        else{
            outColor = vec4(1,0,0,1);
            return; //This was an error, stop lighting!
        }
        vec3 viewDir = normalize(-pos); //We know the eye is at (0,0)!
        vec3 reflectOfViewDir = reflect(viewDir,normal);
        
        vec3 diffuseC = color*max(dot(-lightDir,normal),0.0);
        float idiff = diffuseSimple(-lightDir,normal);
        float ispec = specularSimple(-lightDir,normal,-reflectOfViewDir);
        //float intensity = ambient + idiff + ispec;
        float intensity = ambient + idiff + ispec;
        // quantize intensity for cel shading
        float shadeIntensity = ceil(intensity * u_numShades)/ u_numShades;
        
        //outColor = vec4(color*shadeIntensity,1);
        outColor = vec4(color,1);
    }
}
