// ------------------------------------------------------------------------- 
// High Level Design Compiler for Intel(R) FPGAs Version 2025.0 (Release Build #4f8f97ee91)
// 
// Legal Notice: Copyright 2024 Intel Corporation.  All rights reserved.
// Your use of  Intel Corporation's design tools,  logic functions and other
// software and  tools, and its AMPP partner logic functions, and any output
// files any  of the foregoing (including  device programming  or simulation
// files), and  any associated  documentation  or information  are expressly
// subject  to the terms and  conditions of the  Intel FPGA Software License
// Agreement, Intel MegaCore Function License Agreement, or other applicable
// license agreement,  including,  without limitation,  that your use is for
// the  sole  purpose of  programming  logic devices  manufactured by  Intel
// and  sold by Intel  or its authorized  distributors. Please refer  to the
// applicable agreement for further details.
// ---------------------------------------------------------------------------

// SystemVerilog created from i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_0gr
// Created for function/kernel HybridCPU_SignExtend_Kernel
// SystemVerilog created on Fri Nov  6 06:17:16 2026


(* altera_attribute = "-name AUTO_SHIFT_REGISTER_RECOGNITION OFF; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 10037; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 15400; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 12020; -name MESSAGE_DISABLE 12030; -name MESSAGE_DISABLE 12010; -name MESSAGE_DISABLE 12110; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 13410; -name MESSAGE_DISABLE 113007; -name MESSAGE_DISABLE 10958" *)
module HybridCPU_SignExtend_Kernel_i_llvm_fpga_mem_memdep000000r (
    input wire [0:0] in_flush,
    input wire [63:0] in_memdep_HybridCPU_SignExtend_Kernel_avm_readdata,
    input wire [0:0] in_memdep_HybridCPU_SignExtend_Kernel_avm_readdatavalid,
    input wire [0:0] in_memdep_HybridCPU_SignExtend_Kernel_avm_waitrequest,
    input wire [0:0] in_memdep_HybridCPU_SignExtend_Kernel_avm_writeack,
    output wire [0:0] out_lsu_memdep_o_active,
    input wire [0:0] in_i_stall,
    output wire [0:0] out_o_stall,
    output wire [0:0] out_o_valid,
    output wire [0:0] out_o_writeack,
    input wire [63:0] in_i_address,
    input wire [0:0] in_i_predicate,
    input wire [0:0] in_i_valid,
    input wire [63:0] in_i_writedata,
    output wire [34:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_address,
    output wire [0:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount,
    output wire [7:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable,
    output wire [0:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_enable,
    output wire [0:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_read,
    output wire [0:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_write,
    output wire [63:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata,
    input wire clock,
    input wire resetn
    );

    wire [0:0] GND_q;
    wire [34:0] addr_trunc_in;
    wire [34:0] addr_trunc_q;
    wire [31:0] c_i32_0_395_7gr_q;
    wire [34:0] c_i35_0_395_3gr_q;
    wire [2:0] c_i3_0_395_8gr_q;
    wire [63:0] c_i64_0_395_9gr_q;
    wire [7:0] c_i8_1_395_4gr_q;
    wire [63:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_readdata;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_readdatavalid;
    wire i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_readdatavalid_bitsignaltemp;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_waitrequest;
    wire i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_waitrequest_bitsignaltemp;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_writeack;
    wire i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_writeack_bitsignaltemp;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_flush;
    wire i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_flush_bitsignaltemp;
    wire [34:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_address;
    wire [2:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_atomic_op;
    wire [34:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_bitwiseor;
    wire [7:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_byteenable;
    wire [63:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_cmpdata;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_predicate;
    wire i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_predicate_bitsignaltemp;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_stall;
    wire i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_stall_bitsignaltemp;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_valid;
    wire i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_valid_bitsignaltemp;
    wire [63:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_writedata;
    wire [34:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_stream_base_addr;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_stream_reset;
    wire i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_stream_reset_bitsignaltemp;
    wire [31:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_stream_size;
    wire [34:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_address;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_burstcount;
    wire i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_burstcount_bitsignaltemp;
    wire [7:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_byteenable;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_enable;
    wire i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_enable_bitsignaltemp;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_read;
    wire i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_read_bitsignaltemp;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_write;
    wire i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_write_bitsignaltemp;
    wire [63:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_writedata;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_active;
    wire i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_active_bitsignaltemp;
    wire [4:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_input_fifo_depth;
    wire [63:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_readdata;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_stall;
    wire i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_stall_bitsignaltemp;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_valid;
    wire i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_valid_bitsignaltemp;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_writeack;
    wire i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_writeack_bitsignaltemp;
    wire [31:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_profile_avm_burstcount_total_incr;
    wire [31:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_profile_bw_incr;
    reg [0:0] rst_sync_rst_sclrn;


    // c_i32_0_395_7gr(CONSTANT,5)
    assign c_i32_0_395_7gr_q = 32'b00000000000000000000000000000000;

    // GND(CONSTANT,0)
    assign GND_q = 1'b0;

    // c_i64_0_395_9gr(CONSTANT,9)
    assign c_i64_0_395_9gr_q = 64'b0000000000000000000000000000000000000000000000000000000000000000;

    // c_i8_1_395_4gr(CONSTANT,10)
    assign c_i8_1_395_4gr_q = 8'b11111111;

    // c_i35_0_395_3gr(CONSTANT,6)
    assign c_i35_0_395_3gr_q = 35'b00000000000000000000000000000000000;

    // c_i3_0_395_8gr(CONSTANT,8)
    assign c_i3_0_395_8gr_q = 3'b000;

    // addr_trunc(ROUND,2)
    assign addr_trunc_in = in_i_address[34:0];
    assign addr_trunc_q = addr_trunc_in[34:0];

    // i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr(EXTIFACE,11)
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_readdata = in_memdep_HybridCPU_SignExtend_Kernel_avm_readdata;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_readdatavalid = in_memdep_HybridCPU_SignExtend_Kernel_avm_readdatavalid;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_waitrequest = in_memdep_HybridCPU_SignExtend_Kernel_avm_waitrequest;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_writeack = in_memdep_HybridCPU_SignExtend_Kernel_avm_writeack;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_flush = in_flush;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_address = addr_trunc_q;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_atomic_op = c_i3_0_395_8gr_q;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_bitwiseor = c_i35_0_395_3gr_q;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_byteenable = c_i8_1_395_4gr_q;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_cmpdata = c_i64_0_395_9gr_q;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_predicate = GND_q;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_stall = in_i_stall;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_valid = in_i_valid;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_writedata = in_i_writedata;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_stream_base_addr = c_i35_0_395_3gr_q;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_stream_reset = GND_q;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_stream_size = c_i32_0_395_7gr_q;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_readdatavalid_bitsignaltemp = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_readdatavalid[0];
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_waitrequest_bitsignaltemp = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_waitrequest[0];
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_writeack_bitsignaltemp = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_writeack[0];
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_flush_bitsignaltemp = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_flush[0];
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_predicate_bitsignaltemp = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_predicate[0];
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_stall_bitsignaltemp = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_stall[0];
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_valid_bitsignaltemp = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_valid[0];
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_stream_reset_bitsignaltemp = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_stream_reset[0];
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_burstcount[0] = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_burstcount_bitsignaltemp;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_enable[0] = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_enable_bitsignaltemp;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_read[0] = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_read_bitsignaltemp;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_write[0] = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_write_bitsignaltemp;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_active[0] = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_active_bitsignaltemp;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_stall[0] = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_stall_bitsignaltemp;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_valid[0] = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_valid_bitsignaltemp;
    assign i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_writeack[0] = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_writeack_bitsignaltemp;
    lsu_top #(
        .ABITS_PER_LMEM_BANK(0),
        .ADDRSPACE(1024),
        .ALIGNMENT_BYTES(8),
        .ALLOW_HIGH_SPEED_FIFO_USAGE(0),
        .ASYNC_RESET(0),
        .ATOMIC(0),
        .ATOMIC_SIGNED(0),
        .ATOMIC_WIDTH(3),
        .AVM_READ_DATA_LATENESS(0),
        .AVM_WRITE_DATA_LATENESS(0),
        .AWIDTH(35),
        .BURSTCOUNT_WIDTH(1),
        .ENABLE_BANKED_MEMORY(0),
        .FORCE_NOP_SUPPORT(0),
        .HIGH_FMAX(1),
        .INPUTFIFO_USEDW_MAXBITS(5),
        .KERNEL_SIDE_MEM_LATENCY(9),
        .LMEM_ADDR_PERMUTATION_STYLE(0),
        .MEMORY_SIDE_MEM_LATENCY(1),
        .MWIDTH_BYTES(8),
        .NUMBER_BANKS(1),
        .PROFILE_ADDR_TOGGLE(0),
        .READ(0),
        .STALLFREE(0),
        .STYLE("BURST-COALESCED"),
        .SYNCHRONIZE_RESET(0),
        .USECACHING(0),
        .USEINPUTFIFO(0),
        .USEOUTPUTFIFO(1),
        .USE_BYTE_EN(0),
        .USE_STALL_LATENCY(0),
        .USE_WRITE_ACK(1),
        .WIDE_DATA_SLICING(0),
        .WIDTH_BYTES(8),
        .WRITEDATAWIDTH_BYTES(8)
    ) thei_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr (
        .avm_readdata(in_memdep_HybridCPU_SignExtend_Kernel_avm_readdata),
        .avm_readdatavalid(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_readdatavalid_bitsignaltemp),
        .avm_waitrequest(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_waitrequest_bitsignaltemp),
        .avm_writeack(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_writeack_bitsignaltemp),
        .flush(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_flush_bitsignaltemp),
        .i_address(addr_trunc_q),
        .i_atomic_op(c_i3_0_395_8gr_q),
        .i_bitwiseor(c_i35_0_395_3gr_q),
        .i_byteenable(c_i8_1_395_4gr_q),
        .i_cmpdata(c_i64_0_395_9gr_q),
        .i_predicate(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_predicate_bitsignaltemp),
        .i_stall(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_stall_bitsignaltemp),
        .i_valid(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_i_valid_bitsignaltemp),
        .i_writedata(in_i_writedata),
        .stream_base_addr(c_i35_0_395_3gr_q),
        .stream_reset(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_stream_reset_bitsignaltemp),
        .stream_size(c_i32_0_395_7gr_q),
        .avm_address(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_address),
        .avm_burstcount(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_burstcount_bitsignaltemp),
        .avm_byteenable(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_byteenable),
        .avm_enable(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_enable_bitsignaltemp),
        .avm_read(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_read_bitsignaltemp),
        .avm_write(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_write_bitsignaltemp),
        .avm_writedata(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_writedata),
        .o_active(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_active_bitsignaltemp),
        .o_input_fifo_depth(),
        .o_readdata(),
        .o_stall(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_stall_bitsignaltemp),
        .o_valid(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_valid_bitsignaltemp),
        .o_writeack(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_writeack_bitsignaltemp),
        .profile_avm_burstcount_total_incr(),
        .profile_bw_incr(),
        .clock(clock),
        .resetn(rst_sync_rst_sclrn[0])
    );

    // regfree_osync(GPOUT,17)
    assign out_lsu_memdep_o_active = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_active;

    // sync_out_59(GPOUT,19)@20000000
    assign out_o_stall = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_stall;

    // sync_out_60(GPOUT,20)@91
    assign out_o_valid = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_valid;
    assign out_o_writeack = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_o_writeack;

    // dupName_0_regfree_osync_x(GPOUT,22)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_address = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_address;

    // dupName_1_regfree_osync_x(GPOUT,23)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_burstcount;

    // dupName_2_regfree_osync_x(GPOUT,24)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_byteenable;

    // dupName_3_regfree_osync_x(GPOUT,25)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_enable = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_enable;

    // dupName_4_regfree_osync_x(GPOUT,26)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_read = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_read;

    // dupName_5_regfree_osync_x(GPOUT,27)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_write = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_write;

    // dupName_6_regfree_osync_x(GPOUT,28)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_395_1gr_avm_writedata;

    // rst_sync(RESETSYNC,29)
    acl_reset_handler #(
        .ASYNC_RESET(0),
        .USE_SYNCHRONIZER(1),
        .PULSE_EXTENSION(0),
        .PIPE_DEPTH(3),
        .DUPLICATE(1)
    ) therst_sync (
        .clk(clock),
        .i_resetn(resetn),
        .o_sclrn(rst_sync_rst_sclrn)
    );

endmodule
