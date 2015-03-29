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
/*
 * Copyright (C) 2008
 * Y. Hara, H. Tomiyama, S. Honda, H. Takada and K. Ishii
 * Nagoya University, Japan
 * All rights reserved.
 *
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any license fee or
 * royalty on an "as is" basis. The authors disclaims any and all warranties, 
 * whether express, implied, or statuary, including any implied warranties or 
 * merchantability or of fitness for a particular purpose. In no event shall the
 * copyright-holder be liable for any incidental, punitive, or consequential damages
 * of any kind whatsoever arising from the use of these programs. This disclaimer
 * of warranty extends to the user of these programs and user's customers, employees,
 * agents, transferees, successors, and assigns.
 *
 */
/*
 * Read the header information from buffer in JFIF format and begin decoding
 *
 *  @(#) $Id: jfif_read.c,v 1.2 2003/07/18 10:19:21 honda Exp $ 
 */

/*
 * Initialize after reading markers
 */
void
jpeg_init_decompress (bvars *b, dvars *d, hvars * h, mvars *m)
{
  int tmp;
  /*
   * Get MCU number
   */
  d->p_jinfo_MCUHeight = (d->p_jinfo_image_height - 1) / 8 + 1;
  d->p_jinfo_MCUWidth = (d->p_jinfo_image_width - 1) / 8 + 1;
  d->p_jinfo_NumMCU = d->p_jinfo_MCUHeight * d->p_jinfo_MCUWidth;

  /*
   * Create Huffman Table for decoding
   */
  tmp = huff_make_dhuff_tb (&d->p_jinfo_dc_xhuff_tbl_bits[0][0],
		  d->p_jinfo_dc_dhuff_tbl_ml[0],
			    &d->p_jinfo_dc_dhuff_tbl_maxcode[0][0],
			    &d->p_jinfo_dc_dhuff_tbl_mincode[0][0],
			    &d->p_jinfo_dc_dhuff_tbl_valptr[0][0]);
  d->p_jinfo_dc_dhuff_tbl_ml[0] = tmp;
  tmp = huff_make_dhuff_tb (&d->p_jinfo_dc_xhuff_tbl_bits[1][0],
		  d->p_jinfo_dc_dhuff_tbl_ml[1],
			    &d->p_jinfo_dc_dhuff_tbl_maxcode[1][0],
			    &d->p_jinfo_dc_dhuff_tbl_mincode[1][0],
			    &d->p_jinfo_dc_dhuff_tbl_valptr[1][0]);
  d->p_jinfo_dc_dhuff_tbl_ml[1] = tmp;
  tmp = huff_make_dhuff_tb (&d->p_jinfo_ac_xhuff_tbl_bits[0][0],
		  d->p_jinfo_ac_dhuff_tbl_ml[0],
			    &d->p_jinfo_ac_dhuff_tbl_maxcode[0][0],
			    &d->p_jinfo_ac_dhuff_tbl_mincode[0][0],
			    &d->p_jinfo_ac_dhuff_tbl_valptr[0][0]);
  d->p_jinfo_ac_dhuff_tbl_ml[0] = tmp;
  tmp = huff_make_dhuff_tb (&d->p_jinfo_ac_xhuff_tbl_bits[1][0],
		  d->p_jinfo_ac_dhuff_tbl_ml[1],
			    &d->p_jinfo_ac_dhuff_tbl_maxcode[1][0],
			    &d->p_jinfo_ac_dhuff_tbl_mincode[1][0],
			    &d->p_jinfo_ac_dhuff_tbl_valptr[1][0]);
  d->p_jinfo_ac_dhuff_tbl_ml[1] = tmp;
}



void
jpeg_read (unsigned char *read_buf, bvars *b, dvars *d, hvars * h, mvars *m)
{


	unsigned char *ReadBuf;

  /*
   * Read markers
   */




  read_markers (m, d, ReadBuf, read_buf);
//
//
//  /*
//   * Initialize the information used for decoding
//   */
  jpeg_init_decompress (b,d,h,m);
//
//  /*
//   * Start decoding
//   */
  decode_start (&b->OutData_image_width, &b->OutData_image_height,
		  &b->OutData_comp_vpos[0], &b->OutData_comp_hpos[0],		b,d,h,m);

}
