#version 130

uniform sampler2D texture;
uniform float seed;

float rand(vec2 co){
	return fract( sin(dot(co.xy ,vec2(12.9898,78.233))) * seed);
}

void main() {
	float grain = rand( gl_TexCoord[0].xy) * 0.15 + 0.85;

	gl_FragColor = texture2D(texture, gl_TexCoord[0].xy) * grain;
	
}
