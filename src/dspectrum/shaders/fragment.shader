#version 330

in vec2 TexCoord;

uniform sampler2D currentTexture;

out vec4 color;

void main (void) {  
   color = texture(currentTexture, TexCoord);
}    
