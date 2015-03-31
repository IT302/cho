/*
+--------------------------------------------------------------------------+
| CHStone : a suite of benchmark programs for C-based High-Level Synthesis |
| ======================================================================== |
|                                                                          |
| * Collected and Modified : Y. Hara, H. Tomiyama, S. Honda,               |
|                            H. Takada and K. Ishii                        |
|                            Nagoya University, Japan                      |
|                                                                          |
| * Remark :                                                               |
|    1. This source code is modified to unify the formats of the benchmark |
|       programs in CHStone.                                               |
|    2. Test vectors are added for CHStone.                                |
|    3. If "main_result" is 0 at the end of the program, the program is    |
|       correctly executed.                                                |
|    4. Please follow the copyright of each benchmark program.             |
+--------------------------------------------------------------------------+
*/
#ifndef DFDIV_H
#define DFDIV_H

/*
+--------------------------------------------------------------------------+
| * Test Vectors (added for CHStone)                                       |
|     a_input, b_input : input data                                        |
|     z_output : expected output data                                      |
+--------------------------------------------------------------------------+
*/
#define N 22

const cl_ulong a_input[N] = {
  0x7FFF000000000000ULL,	/* nan */
  0x7FF0000000000000ULL,	/* inf */
  0x7FF0000000000000ULL,	/* inf */
  0x7FF0000000000000ULL,	/* inf */
  0x3FF0000000000000ULL,	/* 1.0 */
  0x3FF0000000000000ULL,	/* 1.0 */
  0x0000000000000000ULL,	/* 0.0 */
  0x3FF0000000000000ULL,	/* 1.0 */
  0x0000000000000000ULL,	/* 0.0 */
  0x8000000000000000ULL,	/* -0.0 */
  0x4008000000000000ULL,	/* 3.0 */
  0xC008000000000000ULL,	/* -3.0 */
  0x4008000000000000ULL,	/* 3.0 */
  0xC008000000000000ULL,	/* -3.0 */
  0x4000000000000000ULL,	/* 2.0 */
  0xC000000000000000ULL,	/* -2.0 */
  0x4000000000000000ULL,	/* 2.0 */
  0xC000000000000000ULL,	/* -2.0 */
  0x3FF0000000000000ULL,	/* 1.0 */
  0xBFF0000000000000ULL,	/* -1.0 */
  0x3FF0000000000000ULL,	/* 1.0 */
  0xBFF0000000000000ULL		/* -1.0 */
};

const cl_ulong b_input[N] = {
  0x3FF0000000000000ULL,	/* 1.0 */
  0x7FF8000000000000ULL,	/* nan */
  0x7FF0000000000000ULL,	/* inf */
  0x3FF0000000000000ULL,	/* 1.0 */
  0x7FF8000000000000ULL,	/* nan */
  0x7FF0000000000000ULL,	/* inf */
  0x0000000000000000ULL,	/* 0.0 */
  0x0000000000000000ULL,	/* 0.0 */
  0x3FF0000000000000ULL,	/* 1.0 */
  0x3FF0000000000000ULL,	/* 1.0 */
  0x4000000000000000ULL,	/* 2.0 */
  0x4000000000000000ULL,	/* 2.0 */
  0xC000000000000000ULL,	/* 2.0 */
  0xC000000000000000ULL,	/* -2.0 */
  0x4010000000000000ULL,	/* 4.0 */
  0x4010000000000000ULL,	/* 4.0 */
  0xC010000000000000ULL,	/* -4.0 */
  0xC010000000000000ULL,	/* -4.0 */
  0x3FF8000000000000ULL,	/* 1.5 */
  0x3FF8000000000000ULL,	/* 1.5 */
  0xBFF8000000000000ULL,	/* -1.5 */
  0xBFF8000000000000ULL		/* -1.5 */
};

const cl_ulong z_output[N] = {
  0x7FFF000000000000ULL,	/* nan */
  0x7FF8000000000000ULL,	/* nan */
  0x7FFFFFFFFFFFFFFFULL,	/* nan */
  0x7FF0000000000000ULL,	/* inf */
  0x7FF8000000000000ULL,	/* nan */
  0x0000000000000000ULL,	/* 0.0 */
  0x7FFFFFFFFFFFFFFFULL,	/* nan */
  0x7FF0000000000000ULL,	/* inf */
  0x0000000000000000ULL,	/* 0.0 */
  0x8000000000000000ULL,	/* -0.0 */
  0x3FF8000000000000ULL,	/* 1.5 */
  0xBFF8000000000000ULL,	/* -1.5 */
  0xBFF8000000000000ULL,	/* 1.5 */
  0x3FF8000000000000ULL,	/* -1.5 */
  0x3FE0000000000000ULL,	/* 0.5 */
  0xBFE0000000000000ULL,	/* 5.0 */
  0xBFE0000000000000ULL,	/* -5.0 */
  0x3FE0000000000000ULL,	/* 0.5 */
  0x3FE5555555555555ULL,	/* 0.666667 */
  0xBFE5555555555555ULL,	/* -0.666667 */
  0xBFE5555555555555ULL,	/* -0.666667 */
  0x3FE5555555555555ULL		/* 0.666667 */
};

#endif /* DFDIV_H */
