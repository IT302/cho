#include "SPARC-GCC.h"
#include "softfloat.h"
#include "softfloat-macros"
#include "softfloat-specialize"
#include "softfloat.c"



#define N 20
__kernel

__attribute__((num_compute_units(1)))
__attribute__((reqd_work_group_size(1,1,1)))
void dfmul_main(__global unsigned long* restrict  input_dataA,
                __global unsigned long* restrict  input_dataB,
                __global unsigned long* restrict  output_data)
{
    float64a x1, x2, x3;
    int8a float_rounding_mode = float_round_nearest_even;
    int8a float_exception_flags = 0;

    #pragma unroll
    for (int i = 0; i < N; i++)
    {
        x1 = input_dataA[i];
        x2 = input_dataB[i];
        x3 = float64_mul (x1, x2, &float_rounding_mode,
        &float_exception_flags);
        //printf("result =  %016llx \n", result);a
        output_data[i] = x3;
    }




}
