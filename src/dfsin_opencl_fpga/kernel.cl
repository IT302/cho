#include "SPARC-GCC.h"
#include "softfloat.h"
#include "softfloat-macros"
#include "softfloat-specialize"
#include "softfloat.c"
#define N 36


float64a
float64_abs (float64a x)
{
  return (x & 0x7fffffffffffffffULL);
}

float64a
local_sin (float64a rad, int8a* float_rounding_mode, int8a* float_exception_flags)
{
  float64a app;
  float64a diff;
  float64a m_rad2;
  int inc;

  app = diff = rad;
  inc = 1;
  m_rad2 = float64_neg (float64_mul (rad, rad,  float_rounding_mode, float_exception_flags));
  //printf("m_rad2  =  %016llx\n", m_rad2);
  #pragma unroll
  do
    {
      diff = float64_div (float64_mul (diff, m_rad2, float_rounding_mode, float_exception_flags),
                          int32_to_float64 ((2 * inc) * (2 * inc + 1)),
                          float_rounding_mode,
                          float_exception_flags);
      //printf("x =  %016llx\n", x);
      app = float64_add (app, diff,  float_rounding_mode,float_exception_flags);
      inc++;
    }
  while (float64_ge (float64_abs (diff), 0x3ee4f8b588e368f1UL, float_exception_flags));	/* 0.00001 */
  return app;
}



__kernel
__attribute__((num_compute_units(1)))
__attribute__((reqd_work_group_size(1,1,1)))
void dfsin_main(__global const unsigned long* restrict  input_data,  __global unsigned long* restrict  output_data)
{
    int i;
    float64a result, in;
    int8a float_rounding_mode = float_round_nearest_even;
    int8a float_exception_flags = 0;
    #pragma unroll 3
    for (i = 0; i < N; i++)
    {


      
      in  =  input_data[i];
      //printf("input =  %016llx\n", input_data[i]);
      output_data[i] = local_sin (in, &float_rounding_mode, &float_exception_flags);
      //printf("result =  %016llx\n", result);
     
    }

}
