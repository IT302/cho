#include "private.h"

struct hash_entry
{
  unsigned int key;
	unsigned int value;
	char valid;
	
};

__constant int table2[N] = {
	6103, 26498, 6147, 930, 11339, 8966, 5499, 18370,
	15296, 27942, 28073, 23254, 26498, 28455, 32663, 32000};


unsigned int hash(unsigned int v) {
	unsigned int _v = v;
	unsigned int _h = HASHC(_v & 0xff, SEED);
	_v = _v >> 8;
	_h = HASHC(_v & 0xff, _h);
	_v = _v >> 8;
	_h = HASHC(_v & 0xff, _h);
	_v = _v >> 8;
	_h = HASHC(_v & 0xff, _h);
	return _h;
}

__kernel
	#ifdef FPGA
//__attribute__((num_compute_units(1)))
__attribute__((task))
#endif
//__attribute__((reqd_work_group_size(1,1,1)))
void hash_main(__global  struct hash_entry  * restrict input_data,  
	           __global   int    *  restrict output_data)
{
	for (int i = 0; i < HLEN; ++i)
	{
		//printf("Entry %u: {valid:%u, key:%u, # T1 rids:%u}\n", i, input_data[i].valid, 
			    //input_data[i].key, input_data[i].value);

	}

	//printf("\n\nStart\n");

	int ptr = 0;
	
	for (int i = 0; i < N; i++) {
		unsigned int key = table2[i];
		unsigned int index = hash(key) % HLEN;
		while (input_data[index].valid) {
			if (input_data[index].key == key) {
				//printf("Match: (key:%u, T1 vec sz:%u, T2 rid:%u)\n", key, input_data[index].value, i);
                output_data[ptr++] = key;
				break;
			}
			index = (index + 1) % HLEN;
		}
	}
	
	//printf("Finish\n");



}
