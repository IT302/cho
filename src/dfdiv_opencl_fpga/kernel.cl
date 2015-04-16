#include "SPARC-GCC.h"
#include "softfloat.h"
#include "softfloat-macros"
#include "softfloat-specialize"
#include "softfloat.c"
#define N 22
__kernel
__attribute__((num_compute_units(1)))
__attribute__((reqd_work_group_size(1,1,1)))
void dfdiv_main(__global const unsigned long* restrict input_dataA,
                __global const unsigned long*  restrict input_dataB,
                __global unsigned long*  restrict output_data)
{
    int8a float_rounding_mode = float_round_nearest_even;
    int8a float_exception_flags = 0;


    #pragma unroll 40
    for (int i = 0; i < N; ++i)
    {
    	output_data[i] = float64_div (input_dataA[i], input_dataB[i], &float_rounding_mode,
        &float_exception_flags);
    }

    /*float64a x3[N];

    for (int i = 0; i < N; ++i)
    {
    	x3[i] = float64_div (input_dataA[i], input_dataB[i], &float_rounding_mode,
        &float_exception_flags);
    }*/


     
}
