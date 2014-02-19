#version 140
// variable definition
varying vec3 f_color;
// array definition & initializer
int deref_array[1] = {1};

// struct definition
struct test_struct {
  int val;
};

// function declaration & parameter declarator
int test_func(int param) {
  // return & binary expression
  return param * param;
}

void main(void) {
  // record declaration
  test_struct deref_record;
  // assignment & deref value
  vec3 f_color_adv = f_color;
  // selection (expression swizle to constant)
  if (f_color_adv.x < 0.3)
    discard; // discard
  // constructor call
  int count = int(f_color_adv.x * 10);
  // assign to record & call with parameter
  deref_record.val = test_func(count);
  // call with parameter
  test_func(count);
  // array + index & record field
  deref_array[0] = deref_record.val;

  // loop && unary increment
  for (int i=0; i < count; ++i) {
    // increment assignment
    f_color_adv.x += 0.6;
    // loop break
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

  // built-in variable & assign & constructor with different stuff
  gl_FragColor = vec4(f_color_adv.x, f_color.y * deref_array[0], f_color.z * deref_record.val, 1.0);
}
