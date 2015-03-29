/*
+--------------------------------------------------------------------------+
| CHStone : a suite of benchmark programs for C-based High-Level Synthesis |
| ======================================================================== |
|                                                                          |
| * Collected and Modified : Y. Hara, H. Tomiyama, S. Honda,               |
|                            H. Takada and K. Ishii                        |
|                            Nagoya University, Japan                      |
|                                                                          |
| * Remark :                                                               |
|    1. This source code is modified to unify the formats of the benchmark |
|       programs in CHStone.                                               |
|    2. Test vectors are added for CHStone.                                |
|    3. If "main_result" is 0 at the end of the program, the program is    |
|       correctly executed.                                                |
|    4. Please follow the copyright of each benchmark program.             |
+--------------------------------------------------------------------------+
*/
/* getbits.c, bit level routines                                            */

/*
 * All modifications (mpeg2decode -> mpeg2play) are
 * Copyright (C) 1996, Stefan Eckart. All Rights Reserved.
 */

/* Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved. */

/*
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any license fee or
 * royalty on an "as is" basis.  The MPEG Software Simulation Group disclaims
 * any and all warranties, whether express, implied, or statuary, including any
 * implied warranties or merchantability or of fitness for a particular
 * purpose.  In no event shall the copyright-holder be liable for any
 * incidental, punitive, or consequential damages of any kind whatsoever
 * arising from the use of these programs.
 *
 * This disclaimer of warranty extends to the user of these programs and user's
 * customers, employees, agents, transferees, successors, and assigns.
 *
 * The MPEG Software Simulation Group does not represent or warrant that the
 * programs furnished hereunder are free of infringement of any third-party
 * patents.
 *
 * Commercial implementations of MPEG-1 and MPEG-2 video, including shareware,
 * are subject to royalty fees to patent holders.  Many of these patents are
 * general enough such that they are unavoidable regardless of implementation
 * design.
 *
 */


/* initialize buffer, call once before first getbits or showbits */
/*int
read (unsigned char *s1, const unsigned char *s2, int n)
{
  unsigned char *p1;
  const unsigned char *p2;
  int n_tmp;
  
p1 = s1;
  p2 = s2;
  n_tmp = n;
  
while (n_tmp-- > 0)
    {
      *p1 = *p2;
      
p1++;
      
p2++;
    
}
  
return n;
}*/


int read (unsigned char *s1, const unsigned char *s2, int n)
{

  for (int i = 0; i < n ; ++i)
  {
    s1[i] = inRdbfr[i];
  } 
  return n;
}

void
Fill_Buffer (globvars *g)
{
  int Buffer_Level;
  //unsigned char *p;
  //p = g->ld_Rdbfr;


  Buffer_Level = read (g->ld_Rdbfr, g->inRdbfr, 2048);
  g->ld_Rdptr = g->ld_Rdbfr;

  if (g->System_Stream_Flag)
    g->ld_Rdmax -= 2048;


  /* end of the bitstream file */
  if (Buffer_Level < 2048)
    {
      /* just to be safe */
      if (Buffer_Level < 0)
	Buffer_Level = 0;

      /* pad until the next to the next 32-bit word boundary */
      while (Buffer_Level & 3)
	g->ld_Rdbfr[Buffer_Level++] = 0;

      /* pad the buffer with sequence end codes */
      while (Buffer_Level < 2048)
	{
	  g->ld_Rdbfr[Buffer_Level++] = SEQUENCE_END_CODE >> 24;
	  g->ld_Rdbfr[Buffer_Level++] = SEQUENCE_END_CODE >> 16;
	  g->ld_Rdbfr[Buffer_Level++] = SEQUENCE_END_CODE >> 8;
	  g->ld_Rdbfr[Buffer_Level++] = SEQUENCE_END_CODE & 0xff;
	}
    }
}

unsigned int
Show_Bits (N, g)
     int N;
     globvars *g;
{
  return g->ld_Bfr >> (unsigned)(32-N)%32;
}

/* advance by n bits */

void
Flush_Buffer (N, g)
     int N;
     globvars *g;
{
  int Incnt;

  g->ld_Bfr <<= N;

  Incnt = g->ld_Incnt -= N;

  if (Incnt <= 24)
    {
      if (g->ld_Rdptr < g->ld_Rdbfr + 2044)
  {
    do
      {
        g->ld_Bfr |= (*(g->ld_Rdptr++)) << (24 - Incnt);
        Incnt += 8;
      }
    while (Incnt <= 24);
  }
      else
  {
    do
      {
        if (g->ld_Rdptr >= g->ld_Rdbfr + 2048)
    Fill_Buffer (g);
        g->ld_Bfr |= (*(g->ld_Rdptr++)) << (24 - Incnt);
        Incnt += 8;
      }
    while (Incnt <= 24);
  }
      g->ld_Incnt = Incnt;
    }
}


/* return next n bits (right adjusted) */

unsigned int
Get_Bits (N, g)
     int N;
     globvars *g;
{
  unsigned int Val;

  Val = Show_Bits (N, g);
  Flush_Buffer (N, g);

  return Val;
}

/* return next bit (could be made faster than Get_Bits(1)) */

unsigned int
Get_Bits1 (g)
     globvars *g;
{
  return Get_Bits (1, g);
}






