
#define N 40
#define KEYSIZE 5200


#include "blowfish.h"
#include "bf_locl.h"
#include "bf_pi.h"
#include "bf_enc.c"
#include "bf_skey.c"
#include "bf_cfb64.c"




__kernel
__attribute__((num_compute_units(1)))
__attribute__((reqd_work_group_size(1,1,1)))
void blowfish_main(__global const unsigned char* restrict  input_data,  __global unsigned char* restrict  output_data)
{
  unsigned char ukey[8] ={0};
  unsigned char indata[N];
  unsigned char outdata[N];
  unsigned char ivec[8] ={0};
  BF_LONG key_P[BF_ROUNDS + 2];
  BF_LONG key_S[4 * 256];
  int num;
  int i, j, k, l;
  int encordec;
  int check;

  num = 0;
  k = 0;
  l = 0;
  encordec = 1;
  check = 0;

  BF_set_key (8, ukey, key_P, key_S);
  i  = 0;
  #pragma unroll 2
  while ( k < KEYSIZE )
  {

    
    while (k < KEYSIZE && i < N)
   {
      //printf("input_data[%d] = %d\n", k , (int)input_data[k]);
      indata[i++] = input_data[k++];

    }
    BF_cfb64_encrypt (indata, outdata, i, ivec, &num, encordec, key_P, key_S);
    for (j = 0; j < i; j++)
    {
      //check += (outdata[j] != out_key[l++]);
      //printf("outdata[%d] = %d\n", j , (int)outdata[j]);

      output_data[l++] = outdata[j];


    }
  
    

    i = 0;
  }


}
