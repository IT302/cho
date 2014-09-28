
//int encode (int, int);
//void decode (int);
int filtez (int *bpl, int *dlt);
void upzero (int dlt, int *dlti, int *bli);
int filtep (int rlt1, int al1, int rlt2, int al2);
int quantl (int el, int detl);
int logscl (int il, int nbl);
int scalel (int nbl, int shift_constant);
int uppol2 (int al1, int al2, int plt, int plt1, int plt2);
int uppol1 (int al1, int apl2, int plt, int plt1);
int logsch (int ih, int nbh);
//void reset ();

/* G722 C code */

/* variables for transimit quadrature mirror filter here */
//


struct EncoderVars
{
  int tqmf[24];

  /* variables for encoder (hi and lo) here */
  int il, szl, spl, sl, el;

  int delay_bpl[6];
  int delay_dltx[6];

  int nbl;      /* delay line */
  int al1, al2;
  int plt, plt1, plt2;
  int dlt;
  int rlt, rlt1, rlt2;

  int detl;

  int deth;
  int sh;       /* this comes from adaptive predictor */
  int eh;


  int dh, ih;
  int nbh, szh;
  int sph, ph, yh, rh;

  int delay_dhx[6];

  int delay_bph[6];

  int ah1, ah2;
  int ph1, ph2;
  int rh1, rh2;

  int xl, xh;
  


};

/* QMF filter coefficients:
scaled by a factor of 4 compared to G722 CCITT recomendation */
__constant int h[24] = {
  12, -44, -44, 212, 48, -624, 128, 1448,
  -840, -3220, 3804, 15504, 15504, 3804, -3220, -840,
  1448, 128, -624, 48, 212, -44, -44, 12
};

//

/* variables for receive quadrature mirror filter here */
//int accumc[11], accumd[11];

/* outputs of decode() */
//int xout1, xout2;

//int xs, xd;




__constant int qq4_code4_table[16] = {
  0, -20456, -12896, -8968, -6288, -4240, -2584, -1200,
  20456, 12896, 8968, 6288, 4240, 2584, 1200, 0
};


__constant int qq6_code6_table[64] = {
  -136, -136, -136, -136, -24808, -21904, -19008, -16704,
  -14984, -13512, -12280, -11192, -10232, -9360, -8576, -7856,
  -7192, -6576, -6000, -5456, -4944, -4464, -4008, -3576,
  -3168, -2776, -2400, -2032, -1688, -1360, -1040, -728,
  24808, 21904, 19008, 16704, 14984, 13512, 12280, 11192,
  10232, 9360, 8576, 7856, 7192, 6576, 6000, 5456,
  4944, 4464, 4008, 3576, 3168, 2776, 2400, 2032,
  1688, 1360, 1040, 728, 432, 136, -432, -136
};


__constant int wl_code_table[16] = {
  -60, 3042, 1198, 538, 334, 172, 58, -30,
  3042, 1198, 538, 334, 172, 58, -30, -60
};

__constant int ilb_table[32] = {
  2048, 2093, 2139, 2186, 2233, 2282, 2332, 2383,
  2435, 2489, 2543, 2599, 2656, 2714, 2774, 2834,
  2896, 2960, 3025, 3091, 3158, 3228, 3298, 3371,
  3444, 3520, 3597, 3676, 3756, 3838, 3922, 4008
};



/* decision levels - pre-multiplied by 8, 0 to indicate end */
__constant int decis_levl[30] = {
  280, 576, 880, 1200, 1520, 1864, 2208, 2584,
  2960, 3376, 3784, 4240, 4696, 5200, 5712, 6288,
  6864, 7520, 8184, 8968, 9752, 10712, 11664, 12896,
  14120, 15840, 17560, 20456, 23352, 32767
};



/* quantization table 31 long to make quantl look-up easier,
last entry is for mil=30 case when wd is max */
__constant int quant26bt_pos[31] = {
  61, 60, 59, 58, 57, 56, 55, 54,
  53, 52, 51, 50, 49, 48, 47, 46,
  45, 44, 43, 42, 41, 40, 39, 38,
  37, 36, 35, 34, 33, 32, 32
};

/* quantization table 31 long to make quantl look-up easier,
last entry is for mil=30 case when wd is max */
__constant int quant26bt_neg[31] = {
  63, 62, 31, 30, 29, 28, 27, 26,
  25, 24, 23, 22, 21, 20, 19, 18,
  17, 16, 15, 14, 13, 12, 11, 10,
  9, 8, 7, 6, 5, 4, 4
};




__constant int qq2_code2_table[4] = {
  -7408, -1616, 7408, 1616
};

__constant int wh_code_table[4] = {
  798, -214, 798, -214
};


struct DecoderVars{



/* variables for decoder here */
int ilr, rl;
int dec_deth, dec_detl, dec_dlt;

int dec_del_bpl[6];

int dec_del_dltx[6];

int dec_plt, dec_plt1, dec_plt2;
int dec_szl, dec_spl, dec_sl;
int dec_rlt1, dec_rlt2, dec_rlt;
int dec_al1, dec_al2;
int dl;
int dec_nbl, dec_dh, dec_nbh;

/* variables used in filtez*/ 
int dec_del_bph[6];

int dec_del_dhx[6];

int dec_szh;
/* variables used in filtep */
int dec_rh1, dec_rh2;
int dec_ah1, dec_ah2;
int dec_ph, dec_sph;

int dec_sh;
int xs, xd;

int dec_ph1, dec_ph2;
int dh, ih, il,rh;

/* variables for receive quadrature mirror filter here */
int accumc[11], accumd[11];

/* outputs of decode() */
int xout1, xout2;
};

typedef struct EncoderVars evars;
typedef struct DecoderVars dvars;
