#version 410 core
layout(location = 0) in vec3 a_Position;
//layout(location = 1) in vec4 a_Color;
//layout(location = 2) in vec2 a_TexCoord;
//layout(location = 3) in float a_TexIndex;
//layout(location = 4) in vec3 a_Normals;

out vec4 v_Color;
//out vec2 v_TexCoord;

void main(){
  gl_Position = vec4(a_Position, 1.0);
  //v_Color = a_Color;

}