//#include "private.h"

#define T_SIZE 69905066
#define H_SIZE 128*1024


typedef struct 
{
    unsigned short valid;
    unsigned short key;
    unsigned int ridptr;
} bucket_t;


typedef struct 
{
    unsigned short key;
    unsigned int rid;
} row_t;

typedef struct 
{
    unsigned int ridptr;
    unsigned int rid;
} match_t;


inline unsigned int hashfunc(unsigned short k) {
	unsigned short h = k;
	for (unsigned int i = 0; i < 16; i++) {
		char v11 = (h >> 10) & 0x1;
		char v13 = (h >> 12) & 0x1;
		char v14 = (h >> 13) & 0x1;
		char v16 = (h >> 15) & 0x1;
		h = (h << 1) | ((((v16 ^ v14) ^ v13) ^ v11) & 0x1);
	}
	return h;
}

__kernel
#ifdef FPGA
__attribute__((task))
#endif

void hash_main(__global  bucket_t  *restrict hashtable,
               __global  row_t  * restrict table,  
	           __global  match_t    *  restrict matches,
	           __global   int    *  restrict num_elems)
{
	int num_matches = 0;
	__local bucket_t hashtable_local[H_SIZE];
	for (int i  = 0; i < H_SIZE; i++ )
	{
		hashtable_local[i] = hashtable[i]; 
	}

	for (int i = 0; i < T_SIZE; i++) 
	{
		
		int hash = hashfunc(table[i].key);
		while (hashtable_local[hash].valid) {
			if (hashtable_local[hash].key == table[i].key) 
			{
				//printf("Match: (key:%u, T1 vec sz:%u, T2 rid:%u)\n", hashtable[hash].ridptr, table[i].rid);
                //output_data[ptr++] = key;
                matches[num_matches].ridptr = hashtable_local[hash].ridptr;
                matches[num_matches].rid =   table[i].rid;
                ++num_matches;
				break;
			}
			hash = (hash + 1) % H_SIZE;
		}


		//table400[i] = table[i]






	}



	//printf("Number of matches: %d\n", num_matches);

	*num_elems = num_matches;

	//row_t a  = table400[9000];



}
