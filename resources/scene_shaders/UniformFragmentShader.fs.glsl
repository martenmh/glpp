#version 410 core
out vec4 o_Color;

in vec4 v_Color;
in vec2 v_TexCoord;
in vec4 v_Position;

uniform mat4 mvp;

void main(){
    o_Color = v_Color;
}