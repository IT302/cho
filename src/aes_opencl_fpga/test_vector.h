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
#ifndef AES_H
#define AES_H
/*+--------------------------------------------------------------------------+
| * Test Vector (added for CHStone)                                        |
|     out_enc_statemt : expected output data for "decrypt"                 |
+--------------------------------------------------------------------------+
*/
  const int out_dec_statemt[16] =
    { 0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2,
    0xe0, 0x37, 0x7, 0x34
  };

  /*
+--------------------------------------------------------------------------+
| * Test Vector (added for CHStone)                                        |
|     out_enc_statemt : expected output data for "encrypt"                 |
+--------------------------------------------------------------------------+
*/
  const int out_enc_statemt[16] =
    { 0x39, 0x25, 0x84, 0x1d, 0x2, 0xdc, 0x9, 0xfb, 0xdc, 0x11, 0x85, 0x97,
    0x19, 0x6a, 0xb, 0x32
  };

#endif /* AES_H */
