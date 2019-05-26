#version 130

uniform sampler2D texture;
uniform vec2 direction;
uniform float blurRadius;
void main() {
	float offset[5] = float[](0.0,1.0,2.0,3.0,4.0);
	float weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216,0.0540540541, 0.0162162162);

	vec4 outputColor = texture2D(texture, gl_TexCoord[0].xy) * weight[0];

	for(int i=1; i<5; i++) {
		outputColor += texture2D(texture, vec2( gl_TexCoord[0].x - offset[i]*blurRadius*direction.x,  gl_TexCoord[0].y - offset[i]*blurRadius*direction.y)) * weight[i];
		outputColor += texture2D(texture, vec2( gl_TexCoord[0].x + offset[i]*blurRadius*direction.x,  gl_TexCoord[0].y + offset[i]*blurRadius*direction.y)) * weight[i];
	}

	outputColor.w = 1.0;
	gl_FragColor = outputColor;
}
