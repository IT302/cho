
#include "global.h"
#include "adpcm.c"


#define SIZE 100
#define IN_END 100

__kernel
#ifdef FPGA
//__attribute__((num_compute_units(1)))
__attribute__((task))
#endif
//__attribute__((reqd_work_group_size(1,1,1)))
void adpcm_main(__global int* restrict  input_data,  __global int* restrict  output_dataA,__global int* restrict  output_dataB)
{

  int tmp[SIZE] = {0};

    //equivalent  to  reset()
    evars e = {     .tqmf = {0},
                    .nbl =  0,
                    .al1 =  0,
                    .al2 =  0,
                    .plt1 = 0,
                    .plt2 = 0,
                    .rlt1 = 0,
                    .rlt2 = 0,
                    .nbh =  0,
                    .ah1 =  0,
                    .ah2 =  0,
                    .ph1 =  0,
                    .ph2 =  0,
                    .rh1 =  0,
                    .rh2 =  0,
                    .detl = 32,
                    .deth = 8,
                    .delay_dltx = {0},
                    .delay_dhx = {0},
                    .delay_bpl = {0},
                    .delay_bph = {0},

                   };


    int xin1;
    int xin2;


    for (int i = 0; i < IN_END; i += 2)
    {
      xin1 = input_data[i];
      xin2 = input_data[i + 1];
      tmp[i / 2] = encode (xin1, xin2, &e);
      output_dataA[i / 2] = tmp[i / 2];
    }

    /*    dvars d = {.dec_del_dltx = {0},
                    .dec_del_dhx = {0},
                    .dec_del_bpl = {0},
                    .dec_del_bph = {0},
                    .accumc = {0},
                    .accumd = {0},
                    .dec_detl = 32,
                    .dec_deth = 8,
                    //nbl = al1 = al2 = plt1 = plt2 = rlt1 = rlt2 = 0;
                    //nbh = ah1 = ah2 = ph1 = ph2 = rh1 = rh2 = 0;
                   .dec_nbl =  0,
                   .dec_al1 = 0,
                   .dec_al2 = 0,
                   .dec_plt1 = 0,
                   .dec_plt2 = 0,
                   .dec_rlt1 = 0,
                   .dec_rlt2 = 0,
                   .dec_nbh = 0,
                   .dec_ah1 = 0,
                   .dec_ah2 = 0,
                   .dec_ph1 = 0,
                   .dec_ph2 = 0,
                   .dec_rh1 = 0,
                   .dec_rh2 = 0,
                    };

          d.il = e.il;



    for (int i = 0; i < IN_END; i += 2)
    {

      decode (tmp[i/2], &d);
      //output_dataB[i] = d.xout1;
      //output_dataB[i + 1] = d.xout2;*/


      
    }

   

    


}

