
#include "globvars.h"
#include "config.h"
#include "global.h"
#include "getbits.c"
#include "getvlc.h"
#include "getvlc.c"
#include "motion.c"


void
Initialize_Buffer (globvars *g)
{
  g->ld_Incnt = 0;
  g->ld_Rdptr = g->ld_Rdbfr + 2048;
  g->ld_Rdmax = g->ld_Rdptr;
  g->ld_Bfr = 68157440;
  Flush_Buffer (0, g);   /* fills valid data into bfr */
}





__constant int inPMV[2][2][2] = { {{45, 207}, {70, 41}}, {{4, 180}, {120, 216}} };
__constant int inmvfs[2][2] = { {232, 200}, {32, 240} };
__constant int outPMV[2][2][2] =
  { {{1566, 206}, {70, 41}}, {{1566, 206}, {120, 216}} };
__constant int outmvfs[2][2] = { {0, 200}, {0, 240} };

__kernel
#ifdef FPGA
//__attribute__((num_compute_units(1)))
__attribute__((task))
#endif
__attribute__((reqd_work_group_size(1,1,1)))
void motion_main(__global int* restrict  input_dataA, __global int* restrict  input_dataB,
                  __global int* restrict  output_dataA, __global int* restrict  output_dataB)
{

 int i, j, k;
 int main_result;
 int PMV[2][2][2];
 int dmvector[2] = {0};
 int motion_vertical_field_select[2][2];
 int s, motion_vector_count, mv_format, h_r_size, v_r_size, dmv, mvscale;

 globvars g = {.System_Stream_Flag = 0,
  .evalue = 0,};

  main_result = 0;
  //evalue = 0; in struct
  //System_Stream_Flag = 0; in struct
  s = 0;
  motion_vector_count = 1;
  mv_format = 0;
  h_r_size = 200;
  v_r_size = 200;
  dmv = 0;
  mvscale = 1;


  for (i = 0; i < 2; i++)
  {
    dmvector[i] = 0;
    for (j = 0; j < 2; j++)
    {
      motion_vertical_field_select[i][j] = input_dataB[i*2 +j];
      //printf ("%d\n", motion_vertical_field_select[i][j]);
      for (k = 0; k < 2; k++)
      {
        PMV[i][j][k] = input_dataA[i + 2 * (j + 2 * k)];
        //printf ("%d\n", PMV[i][j][k]);
      }
    }
  }

  Initialize_Buffer (&g);

  motion_vectors (PMV, dmvector, motion_vertical_field_select, s,
    motion_vector_count, mv_format, h_r_size, v_r_size, dmv,
    mvscale, &g);

  for (i = 0; i < 2; i++)
  {
    for (j = 0; j < 2; j++)

    {
       //main_result +=
        //(motion_vertical_field_select[i][j] != outmvfs[i][j]);
        output_dataB[i*2 +j] =  motion_vertical_field_select[i][j];
      //printf("[%d][%d] = %d\n", i,j,motion_vertical_field_select[i][j]);

      for (k = 0; k < 2; k++)
      {
        //main_result += (PMV[i][j][k] != outPMV[i][j][k]);
        //printf ("%d\n", PMV[i][j][k]);
        output_dataA[i + 2 * (j + 2 * k)] = PMV[i][j][k];
      }

    }

  }
  //printf ("%d\n", main_result);


}
