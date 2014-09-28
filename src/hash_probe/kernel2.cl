#include "private.h"

struct hash_entry
{
  unsigned int key;
	unsigned int value;
	char valid;
	
};

__constant int table1[N] = {
	10784, 13526, 32000, 22506, 26498, 14320, 5164, 28073, 6621,
	11413, 2759, 12222, 28137, 32000, 4709, 16120};
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
__attribute__((num_compute_units(1)))
__attribute__((max_unroll_loops(4)))
__attribute__((num_compute_units(1)))
__attribute__((num_share_resources(1)))
__attribute__((max_share_resources(8)))
__attribute__((task))
#endif
//__attribute__((reqd_work_group_size(1,1,1)))
void hash_main(__global  struct hash_entry  * restrict ht,  
	           __global   int    *  restrict output_data)
{
	


    unsigned int t1rids[N][N];

    // Constructing the hash table
	for (int i = 0; i < N; i++) {
		unsigned int index = hash(table1[i]) % HLEN;
		while (ht[index].valid && table1[i] != ht[index].key) {
			// Hash collision, next hash entry
			index = (index + 1) % HLEN;
		}
		// Insert T1 row 'i' in entry 'index'
		ht[index].valid = 1;
		ht[index].key = table1[i];
	}
	

	int ptr = 0;
	
	for (int i = 0; i < N; i++) {
		unsigned int key = table2[i];
		unsigned int index = hash(key) % HLEN;
		while (ht[index].valid) {
			if (ht[index].key == key) {
				//printf("Match: (key:%u, T1 vec sz:%u, T2 rid:%u)\n", key, ht[index].value, i);
                output_data[ptr++] = key;
				break;
			}
			index = (index + 1) % HLEN;
		}
	}
	
	//printf("Finish\n");



}
