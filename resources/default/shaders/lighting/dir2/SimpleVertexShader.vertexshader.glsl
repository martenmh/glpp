#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in vec3 a_Normals;

out vec3 FragPos;
out vec3 Normal;
//out vec4 objectColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
  FragPos = vec3(model * vec4(a_Position, 1.0));
  Normal = mat3(transpose(inverse(model))) * a_Normals;
  //objectColor = a_Color;
  gl_Position = projection * view * vec4(FragPos, 1.0);
}