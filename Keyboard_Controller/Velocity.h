
byte velocity_map[128];

/*
A quadrativ function 
  f(x) = ax^2 + bx + c 
with the 2 conditions
  f(0) = 7 
  f(127) = 127
is the function with the 1 parameter a
  f_a(x) = ax^2 + (120/127 - 127a)x + 7
parametrized by a.
a = 0 gives the linear extreme.
a = 120 / 127^2 is a quadratic function with minimum in 0.
We allow the range [0, 127/127^2] for a.

We define alpha by a = alpha / 127^2.
So our parametrized function f_alpha(x) is 
  f_alpha(x) = alpha/127^2 x^2 + (120 - alpha)/127 x + 7
*/

static int f_quadratic(long x, int alpha) {
  static const int square_127 = 127 * 127;
  return x*x*alpha/square_127 + (120-alpha)*x/127 + 7;
}


/**
 * Inits the velocity map in the range x element of [0,127].
 */
void init_velocity_map(int alpha) {
  for (long i = 0; i < 128; i++) {
    velocity_map[i] = f_quadratic(i, alpha);
    //Serial.print(i);Serial.print(" -> ");Serial.println(velocity_map[i]);
  }
}

