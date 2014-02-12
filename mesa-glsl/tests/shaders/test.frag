#version 140
// ir_variable definition
varying vec3 f_color;
int deref_array[1];

// struct definition
struct test_struct {
  int val;
};

// ir_function ir_function_signature
int test_func(int param) {
  // ir_return
  return param * param;
}

void main(void) {
  // ir_deref_record
  test_struct deref_record;
  // ir_assignment ir_deref_value
  vec3 f_color_adv = f_color;
  // ir_if (ir_deref_record ir_expression ir_constant)
  if (f_color_adv.x < 0.3)
    discard; // ir_discard
  int count = int(f_color_adv.x * 10);
  // ir_assign ir_call
  deref_record.val = test_func(count);
  // ir_call
  test_func(count);
  // ir_deref_array
  deref_array[0] = deref_record.val;

  // ir_loop
  for (int i=0; i < count; ++i) {
    f_color_adv.x += 0.6;
    // ir_loop_break
  }

  // switch
  switch (count % 10) {
      // case
      case 1:
        break; // switch break
      case 10:
        count += 1;
      default:
        break;
  }

  gl_FragColor = vec4(f_color_adv.x, f_color.y * deref_array[0], f_color.z * deref_record.val, 1.0);
}
