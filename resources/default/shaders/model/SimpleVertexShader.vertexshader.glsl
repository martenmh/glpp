#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in vec3 a_Normals;

out vec4 v_Color;
out vec2 v_TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
  v_TexCoord = a_TexCoord;
  v_Color = a_Color;
  gl_Position = projection * view * model * vec4(a_Position, 1.0);
}