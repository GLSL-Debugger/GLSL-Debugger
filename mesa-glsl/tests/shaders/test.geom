#version 330
// Test geometry shader features
// Layout, method, EmitVertex, EndPrimitive only
layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

void main() {
  int i;
  vec4 vertex;
  for (i = 0; i < gl_in.length(); i++) {
    gl_Position = gl_in[i].gl_Position;
    EmitVertex();
  }
  EndPrimitive();
}