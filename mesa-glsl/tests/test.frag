#version 120
varying vec3 f_color;
int deref_array[1];

struct test_struct {
  int val;
};

int test_func(int param) {
  return param * param;
}

void main(void) {
  test_struct deref_record;
  vec3 f_color_adv = f_color;
  if (f_color_adv.x < 0.3)
    discard;
  int count = int(f_color_adv.x * 10);
  deref_record.val = test_func(count);
  deref_array[0] = deref_record.val;
  for (int i=0; i < count; ++i) {
    f_color_adv.x += 0.6;
  }
  gl_FragColor = vec4(f_color_adv.x, f_color.y * deref_array[0], f_color.z * deref_record.val, 1.0);
}
