#include "global.h"
#include "decode.h"
#include "marker.c"
#include "chenidct.c"
#include "huffman.h"
#include "huffman.c"
#include "decode.c"
#include "jfif_read.c"

__kernel
#ifdef FPGA
//__attribute__((num_compute_units(1)))
__attribute__((task))
#endif
__attribute__((reqd_work_group_size(1,1,1)))
void jpeg_main(__global unsigned char  *input_data,  __global unsigned char *output_dataA,
		       __global int *output_dataB, __global int *output_dataC)
{
	unsigned char JpegFileBuf[JPEG_FILE_SIZE];
	int i, j;



	//printf ("[%d] = %d\n", 5206, JpegFileBuf[5206]);

	bvars b;
	dvars d ;
	hvars h = {.read_position = -1};
	mvars m =
	{
			.out_length_get_sof = 17,
			.out_data_precision_get_sof = 8,
			.out_p_jinfo_image_height_get_sof = 59,
			.out_p_jinfo_image_width_get_sof = 90,
			.out_p_jinfo_num_components_get_sof = 3,
			.main_result = 0,
			.i_get_dht = 0,
			.i_get_dqt = 0,
			.ReadBufPtr = 0,
	};

	/*
	 * Store input data in buffer
	 */
	for (i = 0; i < JPEGSIZE; i++)

	{
		m.ReadBuf[i] = input_data[i];
	}

	//printf ("ReadBuf[%d] = %d\n", 5206, m.ReadBuf[5206]);

	jpeg_read (JpegFileBuf, &b, &d, &h, &m);


	  for (i = 0; i < RGB_NUM; i++)
	  {
	      for (j = 0; j < BMP_OUT_SIZE; j++)
		{
	    	  output_dataA[i*BMP_OUT_SIZE + j] = b.OutData_comp_buf[i][j];
		}
	  }

	  *output_dataB = b.OutData_image_width;
	  *output_dataC = b.OutData_image_height;
	  //printf ("output_dataB = %d\n", *output_dataB);
	  //printf ("output_datac = %d\n", *output_dataC);








}
