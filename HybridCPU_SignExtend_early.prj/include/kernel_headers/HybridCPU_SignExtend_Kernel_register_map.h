
/* This header file describes the Register Map for the HybridCPU_SignExtend_Kernel kernel */

/* Note that this header file should NOT be included directly! */
/* Please include the top-level header file register_map_offsets.h instead! */

#ifndef __HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_REGS_H__
#define __HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_REGS_H__



/* Status register contains all the control bits to control kernel execution */
/******************************************************************************/
/* Memory Map Summary                                                         */
/******************************************************************************/

/*
 Address | Access | Register     | Argument                            | Description 
---------|--------|--------------|-------------------------------------|-------------------------------
     0x0 |      R |   reg0[63:0] |                        Status[63:0] |         * Read the status bits
         |        |              |                                     |       that are described below
---------|--------|--------------|-------------------------------------|-------------------------------
     0x8 |      W |   reg1[31:0] |                         Start[31:0] |        * Write 1 to initiate a
         |        |              |                                     |                   kernel start
---------|--------|--------------|-------------------------------------|-------------------------------
    0x30 |      R |   reg6[31:0] |                 FinishCounter[31:0] | * Read to get number of kernel
         |        |  reg6[63:32] |                 FinishCounter[31:0] |       finishes, note that this
         |        |              |                                     |    register will clear on read
---------|--------|--------------|-------------------------------------|-------------------------------
    0x80 |      W |  reg16[63:0] |                       arg_out[63:0] |                              
---------|--------|--------------|-------------------------------------|-------------------------------
    0x88 |      W |  reg17[63:0] |                      arg_out1[63:0] |                              
---------|--------|--------------|-------------------------------------|-------------------------------
    0x90 |      W |  reg18[63:0] |                      arg_out2[63:0] |                              
---------|--------|--------------|-------------------------------------|-------------------------------
    0x98 |      W |  reg19[63:0] |                      arg_out4[63:0] |                              
---------|--------|--------------|-------------------------------------|-------------------------------
    0xa0 |      W |  reg20[63:0] |                        arg_in[63:0] |                              
---------|--------|--------------|-------------------------------------|-------------------------------
    0xa8 |      W |  reg21[63:0] |                       arg_in6[63:0] |                              
---------|--------|--------------|-------------------------------------|-------------------------------
    0xb0 |      W |  reg22[63:0] |                       arg_in7[63:0] |                              
---------|--------|--------------|-------------------------------------|-------------------------------
    0xb8 |      W |  reg23[63:0] |                       arg_in9[63:0] |                              

The status register (0x00) has the following layout:

  Byte         |  7  |  6  |  5  |  4  |        3        |        2        |      1      |     0     |
---------------|-----------------------|-----------------------------------|-------------------------|
  Description  |        Reserved       |      CSR Address Map Version      |    kernel_status Bits   |

If the structure of the generated CSR changes in future versions of the Intel® oneAPI DPC++/C++ Compiler,
the CSR address map version will be updated. The current CSR address map version is 5.
If the CSR address map version changes, review the generated CSR structure and any code that depends
on the CSR structure.
The kernel_status bits have the following layout:

  Bit          |    15     |    14:3    |    2   |    1   |     0      |
---------------|-----------|------------|--------|--------|------------|
  Description  |  running  |  reserved  |  busy  |  done  |  reserved  |

*/

#define CSR_VERSION_NUMBER (5)

/******************************************************************************/
/* Register Address Macros                                                    */
/******************************************************************************/

/* Status Register Bit Offsets (Bits) */
/* Note: Bits In Status Registers Are Marked As Read-Only or Read-Write
   Please Do Not Write To Read-Only Bits */
#ifndef __REGISTER_BITOFFSET_MACROS__
#define __REGISTER_BITOFFSET_MACROS__
#define KERNEL_REGISTER_MAP_DONE_OFFSET (1) // Read-only
#define KERNEL_REGISTER_MAP_BUSY_OFFSET (2) // Read-only
#define KERNEL_REGISTER_MAP_RUNNING_OFFSET (15) // Read-only
#endif

/* Status Register Bit Masks (Bits) */
#ifndef __REGISTER_BITMASK_MACROS__
#define __REGISTER_BITMASK_MACROS__
#define KERNEL_REGISTER_MAP_DONE_MASK (0x2)
#define KERNEL_REGISTER_MAP_BUSY_MASK (0x4)
#define KERNEL_REGISTER_MAP_RUNNING_MASK (0x8000)
#endif

/* Byte Addresses */
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_STATUS_REG (0x0 + HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_OFFSET)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_START_REG (0x8 + HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_OFFSET)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_FINISHCOUNTER_REG (0x30 + HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_OFFSET)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_FINISHCOUNTER_REG (0x34 + HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_OFFSET)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_OUT_REG (0x80 + HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_OFFSET)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_OUT1_REG (0x88 + HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_OFFSET)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_OUT2_REG (0x90 + HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_OFFSET)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_OUT4_REG (0x98 + HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_OFFSET)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_IN_REG (0xa0 + HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_OFFSET)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_IN6_REG (0xa8 + HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_OFFSET)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_IN7_REG (0xb0 + HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_OFFSET)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_IN9_REG (0xb8 + HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_OFFSET)

/* Argument Sizes (bytes) */
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_OUT_SIZE (8)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_OUT1_SIZE (8)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_OUT2_SIZE (8)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_OUT4_SIZE (8)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_IN_SIZE (8)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_IN6_SIZE (8)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_IN7_SIZE (8)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_IN9_SIZE (8)

/* Argument Masks */
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_OUT_MASK (0xffffffffffffffffULL)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_OUT1_MASK (0xffffffffffffffffULL)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_OUT2_MASK (0xffffffffffffffffULL)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_OUT4_MASK (0xffffffffffffffffULL)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_IN_MASK (0xffffffffffffffffULL)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_IN6_MASK (0xffffffffffffffffULL)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_IN7_MASK (0xffffffffffffffffULL)
#define HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_ARG_ARG_IN9_MASK (0xffffffffffffffffULL)

#endif /* __HYBRIDCPU_SIGNEXTEND_KERNEL_REGISTER_MAP_REGS_H__ */
