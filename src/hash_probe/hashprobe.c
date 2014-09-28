#include <stdio.h>

#define N 16
#define HLEN (N+N/2)

typedef struct {
	char valid;
	unsigned int key;
	unsigned int value;
} hash_entry;
volatile hash_entry ht[HLEN];
volatile unsigned int t1rids[N][N];
volatile int table1[N] = {
	10784, 13526, 32000, 22506, 26498, 14320, 5164, 28073, 6621,
	11413, 2759, 12222, 28137, 32000, 4709, 16120};
volatile int table2[N] = {
	6103, 26498, 6147, 930, 11339, 8966, 5499, 18370,
	15296, 27942, 28073, 23254, 26498, 28455, 32663, 32000};
volatile int outData[N];

#define SEED 5381
#define HASHC(v, h) ((((h) << 5) + (h)) ^ (v))
inline unsigned int hash(v) {
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

// hash probe
int main() {
	int i;
	int j;
	
	for (i = 0; i < HLEN; i++) {
		ht[i].valid = 0;
		ht[i].key = 0;
		ht[i].value = 0;
	}
	
	// Constructing the hash table
	for (i = 0; i < N; i++) {
		unsigned int index = hash(table1[i]) % HLEN;
		while (ht[index].valid && table1[i] != ht[index].key) {
			// Hash collision, next hash entry
			index = (index + 1) % HLEN;
		}
		// Insert T1 row 'i' in entry 'index'
		ht[index].valid = 1;
		ht[index].key = table1[i];
		t1rids[index][ht[index].value++] = i;
	}
	
	printf("Hash table:\n\n");
	for (i = 0; i < HLEN; i++) {
		if (!ht[i].valid)
			printf("Entry %u: Empty\n", i);
		else {
			printf("Entry %u: {valid:%u, key:%u, # T1 rids:%u}\n", i, ht[i].valid, ht[i].key, ht[i].value);
			for (j = 0; j < ht[i].value; j++)
				printf("  T1 rid %u\n", t1rids[i][j]);
		}
	}
	
	printf("\n\nStart\n");
	
	for (i = 0; i < N; i++) {
		unsigned int key = table2[i];
		unsigned int index = hash(key) % HLEN;
		while (ht[index].valid) {
			if (ht[index].key == key) {
				printf("Match: (key:%u, T1 vec sz:%u, T2 rid:%u)\n", key, ht[index].value, i);
				break;
			}
			index = (index + 1) % HLEN;
		}
	}
	
	printf("Finish\n");
	
	return 0;
}

