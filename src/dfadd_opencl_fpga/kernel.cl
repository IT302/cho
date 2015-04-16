
#include "SPARC-GCC.h"
#include "softfloat.h"
#include "softfloat-macros"
#include "softfloat-specialize"
#include "softfloat.c"

double
ullong_to_double (unsigned  long x)
{
  union
  {
    double d;
    unsigned long ll;
  } t;

  t.ll = x;
  return t.d;
}

#define N 46
__kernel
__attribute__((num_compute_units(1)))
__attribute__((reqd_work_group_size(1,1,1)))
void dfadd_main(__global unsigned long* restrict  input_dataA,
                __global unsigned long* restrict  input_dataB,  
	            __global unsigned long* restrict  output_data)
{
	int i;
	float64a x1, x2, x3;
	int8a float_rounding_mode = float_round_nearest_even;
	int8a float_exception_flags = 0;


    #pragma unroll 23
	for (i = 0; i < N; i++)
	{
	  x1 = input_dataA[i];
	  x2 = input_dataB[i];
	  x3 = float64_add (x1, 
	  	                            x2,
	  	                            &float_rounding_mode,
	  	                            &float_exception_flags);
	  output_data[i] = x3;

	}






}
