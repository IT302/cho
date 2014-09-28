#define N 16
#define HLEN (N+N/2)


#define SEED 5381
#define HASHC(v, h) ((((h) << 5) + (h)) ^ (v))