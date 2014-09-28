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
/*************************************************************************/
/*                                                                       */
/*   SNU-RT Benchmark Suite for Worst Case Timing Analysis               */
/*   =====================================================               */
/*                              Collected and Modified by S.-S. Lim      */
/*                                           sslim@archi.snu.ac.kr       */
/*                                         Real-Time Research Group      */
/*                                        Seoul National University      */
/*                                                                       */
/*                                                                       */
/*        < Features > - restrictions for our experimental environment   */
/*                                                                       */
/*          1. Completely structured.                                    */
/*               - There are no unconditional jumps.                     */
/*               - There are no exit from loop bodies.                   */
/*                 (There are no 'break' or 'return' in loop bodies)     */
/*          2. No 'switch' statements.                                   */
/*          3. No 'do..while' statements.                                */
/*          4. Expressions are restricted.                               */
/*               - There are no multiple expressions joined by 'or',     */
/*                'and' operations.                                      */
/*          5. No library calls.                                         */
/*               - All the functions needed are implemented in the       */
/*                 source file.                                          */
/*                                                                       */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*  FILE: adpcm.c                                                        */
/*  SOURCE : C Algorithms for Real-Time DSP by P. M. Embree              */
/*                                                                       */
/*  DESCRIPTION :                                                        */
/*                                                                       */
/*     CCITT G.722 ADPCM (Adaptive Differential Pulse Code Modulation)   */
/*     algorithm.                                                        */
/*     16khz sample rate data is stored in the array test_data[SIZE].    */
/*     Results are stored in the array compressed[SIZE] and result[SIZE].*/
/*     Execution time is determined by the constant SIZE (default value  */
/*     is 2000).                                                         */
/*                                                                       */
/*  REMARK :                                                             */
/*                                                                       */
/*  EXECUTION TIME :                                                     */
/*                                                                       */
/*                                                                       */
/*************************************************************************/


/* G722 encode function two ints in, one 8 bit output */

/* put input samples in xin1 = first value, xin2 = second value */
/* returns il and ih stored together */

/*int
abs (int n)
{
  int m;

  if (n >= 0)
    m = n;
  else
    m = -n;
  return m;
}*/



int encode (int xin1, int xin2 ,evars* e)
{
  int i;
  int h_ptr = 0;
  int tqmf_ptr = 0;
  int tqmf_ptr1 = 0;
  long int xa, xb;
  int decis;


/* transmit quadrature mirror filters implemented here */
  //h_ptr = h;
  //tqmf_ptr = tqmf;
  xa = (long) e->tqmf[tqmf_ptr++] * h[h_ptr++];
  xb = (long) e->tqmf[tqmf_ptr++] * h[h_ptr++];
/* main multiply accumulate loop for samples and coefficients */
  for (i = 0; i < 10; i++)
    {
      xa += (long)  e->tqmf[tqmf_ptr++] * h[h_ptr++];
      xb += (long)  e->tqmf[tqmf_ptr++] * h[h_ptr++];
    }
/* final mult/accumulate */
  xa += (long) e->tqmf[tqmf_ptr++] * h[h_ptr++];
  xb += (long) e->tqmf[tqmf_ptr] * h[h_ptr++];

/* update delay line tqmf */
  tqmf_ptr1 = tqmf_ptr - 2;
  for (i = 0; i < 22; i++)
    e->tqmf[tqmf_ptr--] = e->tqmf[tqmf_ptr1--];
  e->tqmf[tqmf_ptr--] = xin1;
  e->tqmf[tqmf_ptr] = xin2;

/* scale outputs */
  e->xl = (xa + xb) >> 15;
  e->xh = (xa - xb) >> 15;

/* end of quadrature mirror filter code */

/* starting with lower sub band encoder */

/* filtez - compute predictor output section - zero section */
   e->szl = filtez (e->delay_bpl, e->delay_dltx);

/* filtep - compute predictor output signal (pole section) */
  e->spl = filtep (e->rlt1, e->al1, e->rlt2, e->al2);

/* compute the predictor output value in the lower sub_band encoder */
  e->sl = e->szl + e->spl;
  e->el = e->xl - e->sl;

/* quantl: quantize the difference signal */
  e->il = quantl (e->el, e->detl);

/* computes quantized difference signal */
/* for invqbl, truncate by 2 lsbs, so mode = 3 */
  e->dlt = ((long) e->detl * qq4_code4_table[e->il >> 2]) >> 15;

/* logscl: updates logarithmic quant. scale factor in low sub band */
  e->nbl = logscl (e->il, e->nbl);

/* scalel: compute the quantizer scale factor in the lower sub band */
/* calling parameters nbl and 8 (constant such that scalel can be scaleh) */
  e->detl = scalel (e->nbl, 8);

/* parrec - simple addition to compute recontructed signal for adaptive pred */
  e->plt = e->dlt + e->szl;

/* upzero: update zero section predictor coefficients (sixth order)*/
/* calling parameters: dlt, dlt1, dlt2, ..., dlt6 from dlt */
/*  bpli (linear_buffer in which all six values are delayed */
/* return params:      updated bpli, delayed dltx */
  upzero (e->dlt, e->delay_dltx, e->delay_bpl);

/* uppol2- update second predictor coefficient apl2 and delay it as al2 */
/* calling parameters: al1, al2, plt, plt1, plt2 */
  e->al2 = uppol2 (e->al1, e->al2, e->plt, e->plt1, e->plt2);

/* uppol1 :update first predictor coefficient apl1 and delay it as al1 */
/* calling parameters: al1, apl2, plt, plt1 */
  e->al1 = uppol1 (e->al1, e->al2, e->plt, e->plt1);

/* recons : compute recontructed signal for adaptive predictor */
  e->rlt = e->sl + e->dlt;

/* done with lower sub_band encoder; now implement delays for next time*/
  e->rlt2 = e->rlt1;
  e->rlt1 = e->rlt;
  e->plt2 = e->plt1;
  e->plt1 = e->plt;

/* high band encode */

  e->szh = filtez (e->delay_bph, e->delay_dhx);

  e->sph = filtep (e->rh1, e->ah1, e->rh2, e->ah2);

/* predic: sh = sph + szh */
  e->sh = e->sph + e->szh;
/* subtra: eh = xh - sh */
  e->eh = e->xh - e->sh;

/* quanth - quantization of difference signal for higher sub-band */
/* quanth: in-place for speed params: eh, deth (has init. value) */
  if (e->eh >= 0)
    {
      e->ih = 3;			/* 2,3 are pos codes */
    }
  else
    {
      e->ih = 1;			/* 0,1 are neg codes */
    }
  decis = (564L * (long) e->deth) >> 12L;
  if (abs (e->eh) > decis)
    e->ih--;			/* mih = 2 case */

/* compute the quantized difference signal, higher sub-band*/
  e->dh = ((long) e->deth * qq2_code2_table[e->ih]) >> 15L;

/* logsch: update logarithmic quantizer scale factor in hi sub-band*/
  e->nbh = logsch (e->ih, e->nbh);

/* note : scalel and scaleh use same code, different parameters */
  e->deth = scalel (e->nbh, 10);

/* parrec - add pole predictor output to quantized diff. signal */
  e->ph = e->dh + e->szh;

/* upzero: update zero section predictor coefficients (sixth order) */
/* calling parameters: dh, dhi, bphi */
/* return params: updated bphi, delayed dhx */
  upzero (e->dh, e->delay_dhx, e->delay_bph);

/* uppol2: update second predictor coef aph2 and delay as ah2 */
/* calling params: ah1, ah2, ph, ph1, ph2 */
  e->ah2 = uppol2 (e->ah1, e->ah2, e->ph, e->ph1, e->ph2);

/* uppol1:  update first predictor coef. aph2 and delay it as ah1 */
  e->ah1 = uppol1 (e->ah1, e->ah2, e->ph, e->ph1);

/* recons for higher sub-band */
  e->yh = e->sh + e->dh;

/* done with higher sub-band encoder, now Delay for next time */
  e->rh2 = e->rh1;
  e->rh1 = e->yh;
  e->ph2 = e->ph1;
  e->ph1 = e->ph;

/* multiplex ih and il to get signals together */
  return (e->il | (e->ih << 6));
}

/* decode function, result in xout1 and xout2 */

void
decode (int input, dvars *d)
{
  int i;
  long int xa1, xa2;		/* qmf accumulators */
  int h_ptr;
  int ac_ptr, ac_ptr1, ad_ptr, ad_ptr1;

/* split transmitted word from input into ilr and ih */
  d->ilr = input & 0x3f;
  d->ih = input >> 6;

/* LOWER SUB_BAND DECODER */

/* filtez: compute predictor output for zero section */
  d->dec_szl = filtez (d->dec_del_bpl, d->dec_del_dltx);

/* filtep: compute predictor output signal for pole section */
  d->dec_spl = filtep (d->dec_rlt1, d->dec_al1, d->dec_rlt2, d->dec_al2);

  d->dec_sl = d->dec_spl + d->dec_szl;

/* compute quantized difference signal for adaptive predic */
  d->dec_dlt = ((long) d->dec_detl * qq4_code4_table[d->ilr >> 2]) >> 15;

/* compute quantized difference signal for decoder output */
  d->dl = ((long) d->dec_detl * qq6_code6_table[d->il]) >> 15;

  d->rl = d->dl + d->dec_sl;

/* logscl: quantizer scale factor adaptation in the lower sub-band */
  d->dec_nbl = logscl (d->ilr, d->dec_nbl);

/* scalel: computes quantizer scale factor in the lower sub band */
  d->dec_detl = scalel (d->dec_nbl, 8);

/* parrec - add pole predictor output to quantized diff. signal */
/* for partially reconstructed signal */
  d->dec_plt = d->dec_dlt + d->dec_szl;

/* upzero: update zero section predictor coefficients */
  upzero (d->dec_dlt, d->dec_del_dltx, d->dec_del_bpl);

/* uppol2: update second predictor coefficient apl2 and delay it as al2 */
  d->dec_al2 = uppol2 (d->dec_al1, d->dec_al2, d->dec_plt, d->dec_plt1, d->dec_plt2);

/* uppol1: update first predictor coef. (pole setion) */
  d->dec_al1 = uppol1 (d->dec_al1, d->dec_al2, d->dec_plt, d->dec_plt1);

/* recons : compute recontructed signal for adaptive predictor */
  d->dec_rlt = d->dec_sl + d->dec_dlt;

/* done with lower sub band decoder, implement delays for next time */
  d->dec_rlt2 = d->dec_rlt1;
  d->dec_rlt1 = d->dec_rlt;
  d->dec_plt2 = d->dec_plt1;
  d->dec_plt1 = d->dec_plt;

/* HIGH SUB-BAND DECODER */

/* filtez: compute predictor output for zero section */
  d->dec_szh = filtez (d->dec_del_bph, d->dec_del_dhx);

/* filtep: compute predictor output signal for pole section */
  d->dec_sph = filtep (d->dec_rh1, d->dec_ah1, d->dec_rh2, d->dec_ah2);

/* predic:compute the predictor output value in the higher sub_band decoder */
  d->dec_sh = d->dec_sph + d->dec_szh;

/* in-place compute the quantized difference signal */
  d->dec_dh = ((long) d->dec_deth * qq2_code2_table[d->ih]) >> 15L;

/* logsch: update logarithmic quantizer scale factor in hi sub band */
  d->dec_nbh = logsch (d->ih, d->dec_nbh);

 /*scalel: compute the quantizer scale factor in the higher sub band */
  d->dec_deth = scalel (d->dec_nbh, 10);

/* parrec: compute partially recontructed signal */
  d->dec_ph = d->dec_dh + d->dec_szh;

/* upzero: update zero section predictor coefficients */
  upzero (d->dec_dh, d->dec_del_dhx, d->dec_del_bph);

/* uppol2: update second predictor coefficient aph2 and delay it as ah2 */
  d->dec_ah2 = uppol2 (d->dec_ah1, d->dec_ah2, d->dec_ph, d->dec_ph1, d->dec_ph2);

/* uppol1: update first predictor coef. (pole setion) */
  d->dec_ah1 = uppol1 (d->dec_ah1, d->dec_ah2, d->dec_ph, d->dec_ph1);

/* recons : compute recontructed signal for adaptive predictor */
  d->rh = d->dec_sh + d->dec_dh;

/* done with high band decode, implementing delays for next time here */
  d->dec_rh2 = d->dec_rh1;
  d->dec_rh1 = d->rh;
  d->dec_ph2 = d->dec_ph1;
  d->dec_ph1 = d->dec_ph;

/* end of higher sub_band decoder */

/* end with receive quadrature mirror filters */
  d->xd = d->rl - d->rh;
  d->xs = d->rl + d->rh;

/* receive quadrature mirror filters implemented here */
  h_ptr = 0;
  //ac_ptr = accumc;
  ac_ptr = 0;
  //ad_ptr = accumd;
  ad_ptr = 0;
  xa1 = (long) d->xd * h[h_ptr++];
  xa2 = (long) d->xs * h[h_ptr++];
/* main multiply accumulate loop for samples and coefficients */
  for (i = 0; i < 10; i++)
    {
      xa1 += (long) d->accumc[ac_ptr++] * h[h_ptr++];
      xa2 += (long) d->accumd[ad_ptr++] *  h[h_ptr++];
    }
/* final mult/accumulate */
  xa1 += (long) d->accumc[ac_ptr] * h[h_ptr++];
  xa2 += (long) d->accumd[ad_ptr] * h[h_ptr++];

/* scale by 2^14 */
  d->xout1 = xa1 >> 14;
  d->xout2 = xa2 >> 14;

/* update delay lines */
  ac_ptr1 = ac_ptr - 1;
  ad_ptr1 = ad_ptr - 1;
  for (i = 0; i < 10; i++)
    {
      d->accumc[ac_ptr--] = d->accumc[ac_ptr1--];
      d->accumd[ad_ptr--] = d->accumd[ad_ptr1--];
    }
  d->accumc[ac_ptr] = d->xd;
  d->accumd[ad_ptr] = d->xs;
}

// /* clear all storage locations */

// void
// reset ()
// {
//   int i;

//   detl = dec_detl = 32;		/* reset to min scale factor */
//   deth = dec_deth = 8;
//   nbl = al1 = al2 = plt1 = plt2 = rlt1 = rlt2 = 0;
//   nbh = ah1 = ah2 = ph1 = ph2 = rh1 = rh2 = 0;
//   dec_nbl = dec_al1 = dec_al2 = dec_plt1 = dec_plt2 = dec_rlt1 = dec_rlt2 = 0;
//   dec_nbh = dec_ah1 = dec_ah2 = dec_ph1 = dec_ph2 = dec_rh1 = dec_rh2 = 0;

//   for (i = 0; i < 6; i++)
//     {
//       delay_dltx[i] = 0;
//       delay_dhx[i] = 0;
//       dec_del_dltx[i] = 0;
//       dec_del_dhx[i] = 0;
//     }

//   for (i = 0; i < 6; i++)
//     {
//       delay_bpl[i] = 0;
//       delay_bph[i] = 0;
//       dec_del_bpl[i] = 0;
//       dec_del_bph[i] = 0;
//     }

//   for (i = 0; i < 24; i++)
//     tqmf[i] = 0;		// i<23

//   for (i = 0; i < 11; i++)
//     {
//       accumc[i] = 0;
//       accumd[i] = 0;
//     }
// }

/* filtez - compute predictor output signal (zero section) */
/* input: bpl1-6 and dlt1-6, output: szl */

int
filtez (int *bpl, int *dlt)
{
  int i;
  long int zl;
  int bpl_ptr = 0;
  int dlt_ptr = 0;
  //zl = (long) (*bpl++) * (*dlt++);
  zl = (long) bpl[bpl_ptr++] * dlt[dlt_ptr++];
  for (i = 1; i < 6; i++)
    zl += (long) bpl[bpl_ptr++] * dlt[dlt_ptr++];

  return ((int) (zl >> 14));	/* x2 here */
}

/* filtep - compute predictor output signal (pole section) */
/* input rlt1-2 and al1-2, output spl */

int
filtep (int rlt1, int al1, int rlt2, int al2)
{
  long int pl, pl2;
  pl = 2 * rlt1;
  pl = (long) al1 *pl;
  pl2 = 2 * rlt2;
  pl += (long) al2 *pl2;
  return ((int) (pl >> 15));
}

/* quantl - quantize the difference signal in the lower sub-band */
int
quantl (int el, int detl)
{
  int ril, mil;
  long int wd, decis;

/* abs of difference signal */
  wd = abs (el);
/* determine mil based on decision levels and detl gain */
  for (mil = 0; mil < 30; mil++)
    {
      decis = (decis_levl[mil] * (long) detl) >> 15L;
      if (wd <= decis)
	break;
    }
/* if mil=30 then wd is less than all decision levels */
  if (el >= 0)
    ril = quant26bt_pos[mil];
  else
    ril = quant26bt_neg[mil];
  return (ril);
}

/* logscl - update log quantizer scale factor in lower sub-band */
/* note that nbl is passed and returned */

int
logscl (int il, int nbl)
{
  long int wd;
  wd = ((long) nbl * 127L) >> 7L;	/* leak factor 127/128 */
  nbl = (int) wd + wl_code_table[il >> 2];
  if (nbl < 0)
    nbl = 0;
  if (nbl > 18432)
    nbl = 18432;
  return (nbl);
}

/* scalel: compute quantizer scale factor in lower or upper sub-band*/

int
scalel (int nbl, int shift_constant)
{
  int wd1, wd2, wd3;
  wd1 = (nbl >> 6) & 31;
  wd2 = nbl >> 11;
  wd3 = ilb_table[wd1] >> (shift_constant + 1 - wd2);
  return (wd3 << 3);
}

/* upzero - inputs: dlt, dlti[0-5], bli[0-5], outputs: updated bli[0-5] */
/* also implements delay of bli and update of dlti from dlt */

void
upzero (int dlt, int *dlti, int *bli)
{
  int i, wd2, wd3;
/*if dlt is zero, then no sum into bli */
  if (dlt == 0)
    {
      for (i = 0; i < 6; i++)
	{
	  bli[i] = (int) ((255L * bli[i]) >> 8L);	/* leak factor of 255/256 */
	}
    }
  else
    {
      for (i = 0; i < 6; i++)
	{
	  if ((long) dlt * dlti[i] >= 0)
	    wd2 = 128;
	  else
	    wd2 = -128;
	  wd3 = (int) ((255L * bli[i]) >> 8L);	/* leak factor of 255/256 */
	  bli[i] = wd2 + wd3;
	}
    }
/* implement delay line for dlt */
  dlti[5] = dlti[4];
  dlti[4] = dlti[3];
  dlti[3] = dlti[2];
  dlti[1] = dlti[0];
  dlti[0] = dlt;
}

/* uppol2 - update second predictor coefficient (pole section) */
/* inputs: al1, al2, plt, plt1, plt2. outputs: apl2 */

int
uppol2 (int al1, int al2, int plt, int plt1, int plt2)
{
  long int wd2, wd4;
  int apl2;
  wd2 = 4L * (long) al1;
  if ((long) plt * plt1 >= 0L)
    wd2 = -wd2;			/* check same sign */
  wd2 = wd2 >> 7;		/* gain of 1/128 */
  if ((long) plt * plt2 >= 0L)
    {
      wd4 = wd2 + 128;		/* same sign case */
    }
  else
    {
      wd4 = wd2 - 128;
    }
  apl2 = wd4 + (127L * (long) al2 >> 7L);	/* leak factor of 127/128 */

/* apl2 is limited to +-.75 */
  if (apl2 > 12288)
    apl2 = 12288;
  if (apl2 < -12288)
    apl2 = -12288;
  return (apl2);
}

/* uppol1 - update first predictor coefficient (pole section) */
/* inputs: al1, apl2, plt, plt1. outputs: apl1 */

int
uppol1 (int al1, int apl2, int plt, int plt1)
{
  long int wd2;
  int wd3, apl1;
  wd2 = ((long) al1 * 255L) >> 8L;	/* leak factor of 255/256 */
  if ((long) plt * plt1 >= 0L)
    {
      apl1 = (int) wd2 + 192;	/* same sign case */
    }
  else
    {
      apl1 = (int) wd2 - 192;
    }
/* note: wd3= .9375-.75 is always positive */
  wd3 = 15360 - apl2;		/* limit value */
  if (apl1 > wd3)
    apl1 = wd3;
  if (apl1 < -wd3)
    apl1 = -wd3;
  return (apl1);
}

/* logsch - update log quantizer scale factor in higher sub-band */
/* note that nbh is passed and returned */

int
logsch (int ih, int nbh)
{
  int wd;
  wd = ((long) nbh * 127L) >> 7L;	/* leak factor 127/128 */
  nbh = wd + wh_code_table[ih];
  if (nbh < 0)
    nbh = 0;
  if (nbh > 22528)
    nbh = 22528;
  return (nbh);
}

