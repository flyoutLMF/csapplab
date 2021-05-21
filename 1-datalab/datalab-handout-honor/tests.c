/* Testing Code */

#include <limits.h>
#include <math.h>

/* Routines used by floation point test code */

/* Convert from bit level representation to floating point number */
float u2f(unsigned u)
{
  union
  {
    unsigned u;
    float f;
  } a;
  a.u = u;
  return a.f;
}

/* Convert from floating point number to bit-level representation */
unsigned f2u(float f)
{
  union
  {
    unsigned u;
    float f;
  } a;
  a.f = f;
  return a.u;
}

int test_mod3(int x)
{
  return x % 3;
}

int test_bitReverse(int x)
{
  int res = 0;
  int i;
  for (i = 0; i < 32; i++)
  {
    int bit = ((unsigned)x >> i) & 1;
    int pos = 31 - i;
    res |= bit << pos;
  }
  return res;
}

int test_float_f2i(unsigned uf)
{
  float f = u2f(uf);
  return (int)f;
}
