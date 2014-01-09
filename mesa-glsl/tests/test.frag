#version 120
varying vec3 f_color;

void main(void) {
  vec3 f_color_adv = f_color;
  int count = int(f_color_adv.x * 10);
  for(int i=0; i < count; ++i){
    f_color_adv.x += 0.6;
  }
  gl_FragColor = vec4(f_color_adv.x, f_color.y, f_color.z, 1.0);
}
