#version 410 core
out vec4 o_Color;

in vec4 v_Color;
in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform sampler2D texture2;

void main(){
    o_Color = mix(texture(texture2, v_TexCoord),texture(u_Texture, v_TexCoord), 0.2);
}