#version 130

uniform sampler2D texture;
uniform float seed;

void main() {
	float offset = seed;

	vec4 rValue = texture2D(texture, vec2(gl_TexCoord[0].x + offset, gl_TexCoord[0].y));  
	vec4 gValue = texture2D(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y ));
	vec4 bValue = texture2D(texture, vec2(gl_TexCoord[0].x - 2.0* offset, gl_TexCoord[0].y));
	vec4 aValue = texture2D(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y)); 

	gl_FragColor = vec4(rValue.r, gValue.g, bValue.b, aValue.a);
}
