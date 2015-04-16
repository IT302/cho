
struct GlobalVars
{
  int type;
  int nb;
  int round;
  int key[32];
  int statemt[32];
  int word[4][120];

};

typedef struct GlobalVars globvars;


#include "aes_func.c"
#include "aes_key.c"
#include "aes_enc.c"
#include "aes_dec.c"


__kernel

__attribute__((num_compute_units(1)))
__attribute__((reqd_work_group_size(1,1,1)))
void aes_main( __global int* restrict  input_data1, 
              __global  int* restrict  input_data2,
              int  input3,
              int  input4,
              __global int* restrict  output_data)
{

  int type = input3;
  int nb = 0;
  int round = 0;
  int key[32];
  int statemt[32];
  int word[4][120];
 


  #pragma unroll 
  for (int i  = 0; i < 16; ++i )
  {
    statemt[i] = input_data1[i];
    
    key[i]     = input_data2[i];
  }

  //encrypt (statemt, key, 128128);
  if (input4 == 0)
  {
    encrypt (statemt, key, word, type, nb, round);

  }
  else
  {

    decrypt (statemt, key, word, type, nb, round);

  }
  


  #pragma unroll 
  for (int i  = 0; i < 16; ++i )
  {
    output_data[i] = statemt[i];
   
  }
  //decrypt (statemt, key, 128128);
  //decrypt (statemt, key, word, 128128, nb, round);


}
