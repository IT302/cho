#define R 0

#define ADDU 33
#define SUBU 35

#define MULT 24
#define MULTU 25

#define MFHI 16
#define MFLO 18

#define AND 36
#define OR 37
#define XOR 38
#define SLL 0
#define SRL 2
#define SLLV 4
#define SRLV 6

#define SLT 42
#define SLTU 43

#define JR 8

#define J 2
#define JAL 3

#define ADDIU 9
#define ANDI 12
#define ORI 13
#define XORI 14

#define LW 35
#define SW 43
#define LUI 15

#define BEQ 4
#define BNE 5
#define BGEZ 1

#define SLTI 10
#define SLTIU 11

#include "imem.h"
__kernel
#ifdef FPGA
//__attribute__((num_compute_units(1)))
__attribute__((task))
#endif
__attribute__((reqd_work_group_size(1,1,1)))
void mips_main(__global unsigned int* restrict  input_data,  __global int* restrict  output_data)
{


  long hilo;
  int reg[32] = {0};
  int Hi;
  int Lo;
  int pc = 0;
  int dmem[64];
  int j;

  unsigned int ins;
  int op;
  int rs;
  int rt;
  int rd;
  int shamt;
  int funct;
  short address;
  int tgtadr;

  int i;
  //int n_inst;

    //n_inst = 0;
    reg[29] = 0x7fffeffc;

  for (i = 0; i < 8; i++)
  {
    dmem[i] = input_data[i];
  }

  pc = 0x00400000;

  do
  {
    ins = imem[IADDR (pc)];
    pc = pc + 4;

    op = ins >> 26;

    switch (op)
    {
      case R:
      funct = ins & 0x3f;
      shamt = (ins >> 6) & 0x1f;
      rd = (ins >> 11) & 0x1f;
      rt = (ins >> 16) & 0x1f;
      rs = (ins >> 21) & 0x1f;

      switch (funct)
      {

        case ADDU:
        reg[rd] = reg[rs] + reg[rt];
        break;
        case SUBU:
        reg[rd] = reg[rs] - reg[rt];
        break;

        case MULT:
        hilo = (long) reg[rs] * (long) reg[rt];
        Lo = hilo & 0x00000000ffffffffUL;
        Hi = ((int) (hilo >> 32)) & 0xffffffffUL;
        break;
        case MULTU:
        hilo =
        (unsigned long) ((unsigned int) (reg[rs])) *
        (unsigned long) ((unsigned int) (reg[rt]));
        Lo = hilo & 0x00000000ffffffffUL;
        Hi = ((int) (hilo >> 32)) & 0xffffffffUL;
        break;

        case MFHI:
        reg[rd] = Hi;
        break;
        case MFLO:
        reg[rd] = Lo;
        break;

        case AND:
        reg[rd] = reg[rs] & reg[rt];
        break;
        case OR:
        reg[rd] = reg[rs] | reg[rt];
        break;
        case XOR:
        reg[rd] = reg[rs] ^ reg[rt];
        break;
        case SLL:
        reg[rd] = reg[rt] << shamt;
        break;
        case SRL:
        reg[rd] = reg[rt] >> shamt;
        break;
        case SLLV:
        reg[rd] = reg[rt] << reg[rs];
        break;
        case SRLV:
        reg[rd] = reg[rt] >> reg[rs];
        break;

        case SLT:
        reg[rd] = reg[rs] < reg[rt];
        break;
        case SLTU:
        reg[rd] = (unsigned int) reg[rs] < (unsigned int) reg[rt];
        break;

        case JR:
        pc = reg[rs];
        break;
        default:
        pc = 0; // error
        break;
    }
    break;

    case J:
    tgtadr = ins & 0x3ffffff;
    pc = tgtadr << 2;
    break;
    case JAL:
    tgtadr = ins & 0x3ffffff;
    reg[31] = pc;
    pc = tgtadr << 2;
    break;

    default:

    address = ins & 0xffff;
    rt = (ins >> 16) & 0x1f;
    rs = (ins >> 21) & 0x1f;
    switch (op)
    {
      case ADDIU:
      reg[rt] = reg[rs] + address;
      break;

      case ANDI:
      reg[rt] = reg[rs] & (unsigned short) address;
      break;
      case ORI:
      reg[rt] = reg[rs] | (unsigned short) address;
      break;
      case XORI:
      reg[rt] = reg[rs] ^ (unsigned short) address;
      break;

      case LW:
      reg[rt] = dmem[DADDR (reg[rs] + address)];
      break;
      case SW:
      dmem[DADDR (reg[rs] + address)] = reg[rt];
      break;

      case LUI:
      reg[rt] = address << 16;
      break;

      case BEQ:
      if (reg[rs] == reg[rt])
      pc = pc - 4 + (address << 2);
      break;
      case BNE:
      if (reg[rs] != reg[rt])
      pc = pc - 4 + (address << 2);
      break;
      case BGEZ:
      if (reg[rs] >= 0)
      pc = pc - 4 + (address << 2);
      break;

      case SLTI:
      reg[rt] = reg[rs] < address;
      break;

      case SLTIU:
      reg[rt] = (unsigned int) reg[rs] < (unsigned short) address;
      break;

      default:
      pc = 0; /* error */
      break;
    }
        break;
      }
    reg[0] = 0;
    //n_inst = n_inst + 1;
  }
  while (pc != 0);
  for (j = 0; j < 8; j++)
  {
    //main_result += (dmem[j] != outData[j]);
    //printf("dmen[%d] = %d\n", j, dmem[j]);
    output_data[j] = dmem[j];
  }






}
