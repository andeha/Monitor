//
//  Freeform.h
//  The MIPS/PIC32 Monitor
//

#ifndef __FREEFORM_H
#define __FREEFORM_H

#pragma mark MIPS Instruction Opcodes

#define MIPS32_OP_ADDIU 0x21
#define MIPS32_OP_ANDI  0x0C
#define MIPS32_OP_BGTZ  0x07
#define MIPS32_OP_J     0x02
#define MIPS32_OP_RDHWR 0x3B
#define MIPS32_OP_XORI  0x0E
#define MIPS32_OP_SLTU  0x2B
#define MIPS32_OP_SYNCI 0x1F
#define MIPS_OP_BEQ     0x04
#define MIPS_OP_BNE     0x05
#define MIPS_OP_ADDI    0x08
#define MIPS_OP_AND     0x24
#define MIPS_OP_COP0    0x10
#define MIPS_OP_JR      0x08
#define MIPS_OP_LUI     0x0F
#define MIPS_OP_LW      0x23
#define MIPS_OP_LBU     0x24
#define MIPS_OP_LHU     0x25
#define MIPS_OP_MFHI    0x10
#define MIPS_OP_MTHI    0x11
#define MIPS_OP_MFLO    0x12
#define MIPS_OP_MTLO    0x13
#define MIPS_OP_SB      0x28
#define MIPS_OP_SH      0x29
#define MIPS_OP_SW      0x2B
#define MIPS_OP_ORI     0x0D
#define MIPS_OP_XOR     0x26
#define MIPS_OP_SRL     0x03
#define MIPS_OP_CACHE   0x2f

#define MIPS_COP0_MF    0x00
#define MIPS_COP0_MT    0x04

#define MIPS_R_INST(opcode, rs, rt, rd, shamt, funct)   \
  (((opcode) << 26) | ((rs) << 21) | ((rt) << 16) |     \
  ((rd) << 11)| ((shamt) << 6) | (funct))

#define MIPS_I_INST(opcode, rs, rt, immd) \
  (((opcode) << 26) | ((rs) << 21) | ((rt) << 16) | (immd))

#define MIPS_J_INST(opcode, addr) (((opcode) << 26) | (addr))

#define MIPS_NOP                    0
#define MIPS_ADDI(tar, src, val)    MIPS_I_INST(MIPS_OP_ADDI, src, tar, val)
#define MIPS32_ADDU(dst, src, tar)  MIPS32_R_INST(MIPS32_OP_SPECIAL, src, tar, dst, 0, MIPS32_OP_ADDIU)
#define MIPS_AND(reg, off, val)     MIPS_R_INST(0, off, val, reg, 0, MIPS_OP_AND)
#define MIPS32_ANDI(tar, src, val)  MIPS32_I_INST(MIPS32_OP_ANDI, src, tar, val)
#define MIPS_B(off)                 MIPS_BEQ(0, 0, off)
#define MIPS_BEQ(src,tar,off)       MIPS_I_INST(MIPS_OP_BEQ, src, tar, off)
#define MIPS32_BGTZ(reg, off)       MIPS32_I_INST(MIPS32_OP_BGTZ, reg, 0, off)
#define MIPS_BNE(src,tar,off)       MIPS_I_INST(MIPS_OP_BNE, src, tar, off)
#define MIPS32_CACHE(op, off, base) MIPS32_I_INST(MIPS32_OP_CACHE, base, op, off)
#define MIPS32_J(tar)               MIPS32_J_INST(MIPS32_OP_J, tar)
#define MIPS_JR(reg)                MIPS_R_INST(0, reg, 0, 0, 0, MIPS_OP_JR)
#define MIPS_MFC0(gpr, cpr, sel)    MIPS_R_INST(MIPS_OP_COP0, MIPS_COP0_MF, gpr, cpr, 0, sel)
#define MIPS_MTC0(gpr,cpr, sel)     MIPS_R_INST(MIPS_OP_COP0, MIPS_COP0_MT, gpr, cpr, 0, sel)
#define MIPS_LBU(reg, off, base)    MIPS_I_INST(MIPS_OP_LBU, base, reg, off)
#define MIPS_LHU(reg, off, base)    MIPS_I_INST(MIPS_OP_LHU, base, reg, off)
#define MIPS_LUI(reg, val)          MIPS_I_INST(MIPS_OP_LUI, 0, reg, val)
#define MIPS_LW(reg, off, base)     MIPS_I_INST(MIPS_OP_LW, base, reg, off)
#define MIPS_MFLO(reg)              MIPS_R_INST(0, 0, 0, reg, 0, MIPS_OP_MFLO)
#define MIPS_MFHI(reg)              MIPS_R_INST(0, 0, 0, reg, 0, MIPS_OP_MFHI)
#define MIPS_MTLO(reg)              MIPS_R_INST(0, reg, 0, 0, 0, MIPS_OP_MTLO)
#define MIPS_MTHI(reg)              MIPS_R_INST(0, reg, 0, 0, 0, MIPS_OP_MTHI)
#define MIPS_ORI(tar, src, val)     MIPS_I_INST(MIPS_OP_ORI, src, tar, val)
#define MIPS32_XORI(tar, src, val)  MIPS32_I_INST(MIPS32_OP_XORI, src, tar, val)
#define MIPS32_RDHWR(tar, dst)      MIPS32_R_INST(MIPS32_OP_SPECIAL3, 0, tar, dst, 0, MIPS32_OP_RDHWR)
#define MIPS_SB(reg, off, base)     MIPS_I_INST(MIPS_OP_SB, base, reg, off)
#define MIPS_SH(reg, off, base)     MIPS_I_INST(MIPS_OP_SH, base, reg, off)
#define MIPS_SW(reg, off, base)     MIPS_I_INST(MIPS_OP_SW, base, reg, off)
#define MIPS_XOR(reg, val1, val2)   MIPS_R_INST(0, val1, val2, reg, 0, MIPS_OP_XOR)
#define MIPS_SRL(reg, src, off)     MIPS_R_INST(0, 0, src, reg, off, MIPS_OP_SRL)
#define MIPS32_SLTU(dst, src, tar)  MIPS32_R_INST(MIPS32_OP_SPECIAL, src, tar, dst, 0, MIPS32_OP_SLTU)
#define MIPS32_SYNCI(off, base)     MIPS32_I_INST(MIPS32_OP_REGIMM, base, MIPS32_OP_SYNCI, off)
#define MIPS_CACHE(op, off, base)   MIPS_I_INST(MIPS_OP_CACHE, base, op, off)

#define MIPS_DRET       0x4200001F
#define MIPS_BREAK      0x0000000D
#define MIPS_BREAK_MASK 0xFC00003F
#define MIPS_SDBBP      0x7000003F
#define MIPS16_SDBBP	0xE801

#define UPPER16(v)      ((v) >> 16)
#define LOWER16(v)      ((v) & 0xFFFF)
#define NEG16(v)        ((~(v) + 1) & 0xFFFF)

#pragma mark - Coprocessor 0 Defines

#define CP0_INDEX       0
#define CP0_RANDOM      1
#define CP0_ENTRYLO0    2
#define CP0_ENTRYLO1    3
#define CP0_CONTEXT     4
#define CP0_PAGEMASK    5
#define CP0_WIRED       6
#define CP0_7           7
#define CP0_BADVADDR    8
#define CP0_COUNT       9
#define CP0_ENTRYHI    10
#define CP0_COMPARE    11
#define CP0_STATUS     12
#define CP0_CAUSE      13
#define CP0_EPC        14
#define CP0_PRID       15
#define CP0_CONFIG     16
#define CP0_LLADDR     17
#define CP0_WATCHLO    18
#define CP0_WATCHHI    19
#define CP0_20         20
#define CP0_21         21
#define CP0_22         22
#define CP0_DEBUG      23
#define CP0_DEPC       24
#define CP0_PERFCNT    25
#define CP0_ERRCTL     26
#define CP0_CACHEERR   27
#define CP0_TAGLO      28
#define CP0_TAGHI      29
#define CP0_ERROREPC   30
#define CP0_DESAVE     31

#pragma mark - Status Register

#define ST_IE       0x00000001  /* Interrupt enable */
#define ST_EXL      0x00000002  /* Exception level */
#define ST_ERL      0x00000004  /* Error level */
#define ST_UM       0x00000010  /* User mode */
#define ST_IM_SW0   0x00000100  /* Software interrupt mask 0 */
#define ST_IM_SW1   0x00000200  /* Software interrupt mask 1 */
#define ST_IM_IRQ0  0x00000400  /* External interrupt mask 0 */
#define ST_IM_IRQ1  0x00000800  /* External interrupt mask 1 */
#define ST_IM_IRQ2  0x00001000  /* External interrupt mask 2 */
#define ST_IM_IRQ3  0x00002000  /* External interrupt mask 3 */
#define ST_IM_IRQ4  0x00004000  /* External interrupt mask 3 */
#define ST_IM_IRQ5  0x00008000  /* External interrupt mask 3 */
#define ST_NMI      0x00080000  /* Reset caused by NMI */
#define ST_TS       0x00200000  /* TLB shutdown */
#define ST_BEV      0x00400000  /* Boot exception vectors */

#define ST_CU0      0x10000000  /* Enable coprocessor 0 */
#define ST_CU1      0x20000000  /* Enable coprocessor 1 (FPU) */

#pragma mark - Coprocessor 1 (FPU) Registers

#define CP1_FIR      0  /* implementation and revision */
#define CP1_FCCR    25  /* condition codes */
#define CP1_FEXR    26  /* exceptions */
#define CP1_FENR    28  /* enables */
#define CP1_FCSR    31  /* control/status */

#endif
