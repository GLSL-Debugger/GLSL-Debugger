#version 140
#extension GL_ARB_geometry_shader4 : enable
// Test geometry shader features
// Now EmitVertex & EndPrimitive only

void main() {
  int i;
  vec4 vertex;
  for (i = 0; i < gl_VerticesIn; i++) {
    gl_Position = gl_PositionIn[i];
    EmitVertex();
  }
  EndPrimitive();
}