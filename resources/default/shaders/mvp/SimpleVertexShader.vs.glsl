#version 410 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in vec3 a_Normals;

uniform mat4 mvp;
out vec4 v_Position;
out vec4 v_Color;

void main(){
  gl_Position = mvp * vec4(a_Position, 1.0);
  v_Color = a_Color;
  v_Position = gl_Position;
}