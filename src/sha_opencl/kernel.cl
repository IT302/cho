/* NIST Secure Hash Algorithm */
/* heavily modified by Uwe Hollerbach uh@alumni.caltech edu */
/* from Peter C. Gutmann's implementation as found in */
/* Applied Cryptography by Bruce Schneier */

/* NIST's proposed modification to SHA of 7/11/94 may be */
/* activated by defining USE_MODIFIED_SHA */

/* SHA f()-functions */

typedef unsigned char BYTE;
typedef unsigned int INT32;

#define SHA_BLOCKSIZE       64

#define f1(x,y,z)   ((x & y) | (~x & z))
#define f2(x,y,z)   (x ^ y ^ z)
#define f3(x,y,z)   ((x & y) | (x & z) | (y & z))
#define f4(x,y,z)   (x ^ y ^ z)

/* SHA constants */

#define CONST1      0x5a827999L
#define CONST2      0x6ed9eba1L
#define CONST3      0x8f1bbcdcL
#define CONST4      0xca62c1d6L

/* 32-bit rotate */

#define ROT32(x,n)  ((x << n) | (x >> (32 - n)))

#define FUNC(n,i)                       \
    temp = ROT32(A,5) + f##n(B,C,D) + E + W[i] + CONST##n;  \
    E = D; D = C; C = ROT32(B,30); B = A; A = temp

#define BLOCK_SIZE 8192
#define VSIZE 2

//used to reconstruct flattened array
#define NUMCOLS 2


void
local_memset (INT32 * s, int c, int n, int e)
{
  INT32 uc;
  INT32 *p;
  int m;

  m = n / 4;
  uc = c;
  p = (INT32 *) s;
  while (e-- > 0)
    {
      p++;
    }
  while (m-- > 0)
    {
      *p++ = uc;
    }
}

void
local_memcpy (INT32 * s1, const BYTE * s2, int n)
{
  INT32 *p1;
  BYTE *p2;
  INT32 tmp;
  int m;
  m = n / 4;
  p1 = (INT32 *) s1;
  p2 = (BYTE *) s2;

  while (m-- > 0)
    {
      tmp = 0;
      tmp |= 0xFF & *p2++;
      tmp |= (0xFF & *p2++) << 8;
      tmp |= (0xFF & *p2++) << 16;
      tmp |= (0xFF & *p2++) << 24;
      *p1 = tmp;
      p1++;
    }
}

/* do SHA transformation */

void
sha_transform (INT32 sha_info_data[], INT32 sha_info_digest[])
{
  int i;
  INT32 temp, A, B, C, D, E, W[80];

  for (i = 0; i < 16; ++i)
    {
      W[i] = sha_info_data[i];
    }
  for (i = 16; i < 80; ++i)
    {
      W[i] = W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16];
    }
  A = sha_info_digest[0];
  B = sha_info_digest[1];
  C = sha_info_digest[2];
  D = sha_info_digest[3];
  E = sha_info_digest[4];

  for (i = 0; i < 20; ++i)
    {
      FUNC (1, i);
    }
  for (i = 20; i < 40; ++i)
    {
      FUNC (2, i);
    }
  for (i = 40; i < 60; ++i)
    {
      FUNC (3, i);
    }
  for (i = 60; i < 80; ++i)
    {
      FUNC (4, i);
    }

  sha_info_digest[0] += A;
  sha_info_digest[1] += B;
  sha_info_digest[2] += C;
  sha_info_digest[3] += D;
  sha_info_digest[4] += E;
}

/* initialize the SHA digest */

void
sha_init (INT32 sha_info_digest[], INT32 *sha_info_count_lo,
          INT32 *sha_info_count_hi)
{
  sha_info_digest[0] = 0x67452301L;
  sha_info_digest[1] = 0xefcdab89L;
  sha_info_digest[2] = 0x98badcfeL;
  sha_info_digest[3] = 0x10325476L;
  sha_info_digest[4] = 0xc3d2e1f0L;
  *sha_info_count_lo = 0L;
  *sha_info_count_hi = 0L;
}


/* update the SHA digest */

void
sha_update (const BYTE * buffer, int count,
            INT32 sha_info_data[], INT32 sha_info_digest[],
            INT32* sha_info_count_lo, INT32* sha_info_count_hi)
{
  if (((*sha_info_count_lo) + ((INT32) count << 3)) < (*sha_info_count_lo))
    {
      ++(*sha_info_count_hi);
    }
  *sha_info_count_lo += (INT32) count << 3;
  *sha_info_count_hi += (INT32) count >> 29;
  while (count >= SHA_BLOCKSIZE)
    {
      local_memcpy (sha_info_data, buffer, SHA_BLOCKSIZE);
      sha_transform ( sha_info_data, sha_info_digest);
      buffer += SHA_BLOCKSIZE;
      count -= SHA_BLOCKSIZE;
    }
  local_memcpy (sha_info_data, buffer, count);
}


/* finish computing the SHA digest */

void
sha_final ( INT32 sha_info_data[], INT32 sha_info_digest[],
            INT32* sha_info_count_lo, INT32* sha_info_count_hi)
{
  int count;
  INT32 lo_bit_count;
  INT32 hi_bit_count;

  lo_bit_count = *sha_info_count_lo;
  hi_bit_count = *sha_info_count_hi;
  count = (int) ((lo_bit_count >> 3) & 0x3f);
  sha_info_data[count++] = 0x80;
  if (count > 56)
    {
      local_memset (sha_info_data, 0, 64 - count, count);
      sha_transform (sha_info_data , sha_info_digest);
      local_memset (sha_info_data, 0, 56, 0);
    }
  else
    {
      local_memset (sha_info_data, 0, 56 - count, count);
    }
  sha_info_data[14] = hi_bit_count;
  sha_info_data[15] = lo_bit_count;
  sha_transform (sha_info_data , sha_info_digest);
}

/* compute the SHA digest of a FILE stream */
void
sha_stream (BYTE indata[VSIZE][BLOCK_SIZE], int in_i[2], INT32 sha_info_digest[5])
{
  int i, j;
  const BYTE *p;
  INT32 sha_info_count_lo, sha_info_count_hi; /* 64-bit bit count */
  INT32 sha_info_data[16];

  sha_init (sha_info_digest, &sha_info_count_lo,&sha_info_count_hi);
  for (j = 0; j < VSIZE; j++)
    {
      i = in_i[j];
      p = &indata[j][0];
      sha_update (p, i,sha_info_data, sha_info_digest,
                  &sha_info_count_lo, &sha_info_count_hi);
    }
    sha_final ( sha_info_data, sha_info_digest,
                &sha_info_count_lo, &sha_info_count_hi);
}

__kernel
#ifdef FPGA
//__attribute__((num_compute_units(1)))
__attribute__((task))
#endif
__attribute__((reqd_work_group_size(1,1,1)))
void sha_main(__global BYTE* restrict  input_data,  __global int* restrict  output_data)
{
    //printf("hhhhhhhhh\n");
    //copy data into local private memory
    //remember we flatened the array on the host side
    BYTE indata[VSIZE][BLOCK_SIZE];
    for( int  row = 0; row < VSIZE; row++)
    {
        for (int col = 0; col < BLOCK_SIZE; col++)
        {
             indata[row][col] = input_data[row*BLOCK_SIZE + col];
             //std::cout <<  (int)Keys_in[row*VSIZE + col] << "\n";
              //printf("[%d][%d] = %d\n",row, col, indata[row][col] );
        }
    }
    int in_i[VSIZE] = { 8192, 8192 };
    INT32 sha_info_digest[5];
    sha_stream (indata, in_i, sha_info_digest);

    for (int i = 0; i < 5; i++)
    {
        //printf("%04x\n",sha_info_digest[i]);
        output_data[i] = sha_info_digest[i];

    }



}
