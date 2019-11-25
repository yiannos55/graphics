#version 150 core

uniform sampler2D diffuseTex;
uniform vec2 pixelSize;
uniform int isVertical;


in Vertex{
	vec2 texCoord;
} IN;

out vec4 fragColour;
const float weights[5] = float[](0.2, 0.2, 0.2, 0.2, 0.2);

void main()
{
vec2 values[5];

	if(isVertical==1){
		values= vec2[](	vec2(0.0 , -pixelSize.y*2), 
		vec2(0.0 , -pixelSize.y*1),vec2(0.0 ,0.0),	
		vec2(0.0, pixelSize.y*1), vec2(0.0, pixelSize.y*2));
	}
	else {
		values = vec2 []( vec2(-pixelSize.x*2, 0.0),
		vec2(-pixelSize.x*1, 0.0), vec2(0 , 0.0),
		vec2(pixelSize.x*1 ,0.0), vec2(pixelSize.x*2 ,0.0));
	}

	for (int i=0; i<5; i ++ ) {
		
		if(distance(values[i],values[i+1])>1.0) {
		vec4 tmp = texture2D(diffuseTex, IN.texCoord.xy + values[i]);
		fragColour+= tmp*weights[i];
		}else{
			vec4 tmp = texture2D(diffuseTex, IN.texCoord.xy + values[i]);
			fragColour+= tmp;
		}
		
	}

} 