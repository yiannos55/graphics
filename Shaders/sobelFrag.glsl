//https://computergraphics.stackexchange.com/questions/3646/opengl-glsl-sobel-edge-detection-filter

#version 330 core

uniform sampler2D diffuseTex;

in Vertex{
	vec2 texCoord;
} IN;


out vec4 fragColour;

mat3 sx = mat3( 
    1.0, 2.0, 1.0, 
    0.0, 0.0, 0.0, 
   -1.0, -2.0, -1.0 
);
mat3 sy = mat3( 
    1.0, 0.0, -1.0, 
    2.0, 0.0, -2.0, 
    1.0, 0.0, -1.0 
);

void main()
{
    vec3 diffuse = texture(diffuseTex, IN.texCoord.st).rgb;
    mat3 I;
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            vec3 sample  = texelFetch(diffuseTex, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).rgb;
            I[i][j] = length(sample); 
    }
}

float gx = dot(sx[0], I[0]) + dot(sx[1], I[1]) + dot(sx[2], I[2]); 
float gy = dot(sy[0], I[0]) + dot(sy[1], I[1]) + dot(sy[2], I[2]);

float g = sqrt(pow(gx, 2.0)+pow(gy, 2.0));
fragColour = vec4(diffuse - vec3(g), 1.0);
} 