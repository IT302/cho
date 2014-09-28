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
#ifndef MPEG2_H
#define MPEG2_H

/*
+--------------------------------------------------------------------------+
| * Test Vectors (added for CHStone)                                       |
|     A : input data                                                       |
|     outData : expected output data                                       |
+--------------------------------------------------------------------------+
*/
const int inPMV[2][2][2] = { {{45, 207}, {70, 41}}, {{4, 180}, {120, 216}} };
const int inmvfs[2][2] = { {232, 200}, {32, 240} };
const int outPMV[2][2][2] =
  { {{1566, 206}, {70, 41}}, {{1566, 206}, {120, 216}} };
const int outmvfs[2][2] = { {0, 200}, {0, 240} };


#endif /* MPEG2_H */
