#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform sampler2D rockTex;
uniform sampler2DShadow shadowTex;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex{
vec3 colour;
vec2 texCoord;
vec3 normal;
vec3 worldPos;
vec4 shadowProj;
}IN;

out vec4 fragColour;


float blend(float y) {
	if (y < 180)
		return 0;
	if (y > 220)
		return 1;
	float m = (y - 180) / 40;
	return smoothstep(0.0, 1.0, m);
}

void main (void) {
vec4 diffuse =texture(diffuseTex, IN.texCoord);
vec4 rock = texture(rockTex, IN.texCoord);

vec3 incident = normalize(lightPos-IN.worldPos);
float lambert = max(0.0, dot(incident, IN.normal));

float dist = length(lightPos-IN.worldPos);
float atten = 1.0-clamp(dist/lightRadius, 0.0, 1.0);

vec3 viewDir = normalize(cameraPos-IN.worldPos);
vec3 halfDir = normalize(incident + viewDir);

float rFactor = max(0.0, dot(halfDir, IN.normal));
float sFactor = pow(rFactor, 50.0);

float shadow = 1.0;
if (IN.shadowProj.w > 0.0) {
	shadow = textureProj(shadowTex, IN.shadowProj);
}
lambert *= shadow;

vec4 texColour = mix(diffuse, rock, blend(IN.worldPos.y));

//if(IN.worldPos.y>200){
	vec3 colour = (texColour.rgb * lightColour.rgb);
	colour += (lightColour.rgb * sFactor) * 0.33;
	fragColour = vec4(colour * atten *lambert, texColour.a);
	fragColour.rgb +=(texColour.rgb* lightColour.rgb) * 0.1;
//}

/*
else{
	vec3 colour = (diffuse.rgb * lightColour.rgb);
	colour	+= (lightColour.rgb * sFactor) * 0.33;
	fragColour = vec4(colour * atten *lambert, diffuse.a);
	fragColour.rgb +=(diffuse.rgb* lightColour.rgb) * 0.1;
}
*/

}