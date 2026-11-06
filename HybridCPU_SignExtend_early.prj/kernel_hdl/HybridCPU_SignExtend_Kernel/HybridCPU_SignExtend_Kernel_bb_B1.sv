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

// SystemVerilog created from bb_HybridCPU_SignExtend_Kernel_B1
// Created for function/kernel HybridCPU_SignExtend_Kernel
// SystemVerilog created on Fri Nov  6 06:17:16 2026


(* altera_attribute = "-name AUTO_SHIFT_REGISTER_RECOGNITION OFF; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 10037; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 15400; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 12020; -name MESSAGE_DISABLE 12030; -name MESSAGE_DISABLE 12010; -name MESSAGE_DISABLE 12110; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 13410; -name MESSAGE_DISABLE 113007; -name MESSAGE_DISABLE 10958" *)
module HybridCPU_SignExtend_Kernel_bb_B1 (
    input wire [0:0] in_pipeline_stall_in,
    output wire [0:0] out_pipeline_valid_out,
    output wire [0:0] out_exiting_stall_out,
    output wire [0:0] out_exiting_valid_out,
    output wire [0:0] out_lsu_memdep_o_active,
    output wire [34:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_address,
    output wire [0:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount,
    output wire [7:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable,
    output wire [0:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_enable,
    output wire [0:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_read,
    output wire [0:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_write,
    output wire [63:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata,
    output wire [0:0] out_stall_in_0,
    output wire [0:0] out_stall_out_0,
    output wire [0:0] out_stall_out_1,
    output wire [34:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address,
    output wire [0:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount,
    output wire [7:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable,
    output wire [0:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable,
    output wire [0:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read,
    output wire [0:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write,
    output wire [63:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata,
    output wire [0:0] out_valid_in_0,
    output wire [0:0] out_valid_in_1,
    output wire [0:0] out_valid_out_0,
    output wire [0:0] out_valid_out_1,
    input wire [63:0] in_arg_in,
    input wire [63:0] in_arg_out,
    input wire [0:0] in_flush,
    input wire [0:0] in_forked_0,
    input wire [0:0] in_forked_1,
    input wire [63:0] in_memdep_HybridCPU_SignExtend_Kernel_avm_readdata,
    input wire [0:0] in_memdep_HybridCPU_SignExtend_Kernel_avm_readdatavalid,
    input wire [0:0] in_memdep_HybridCPU_SignExtend_Kernel_avm_waitrequest,
    input wire [0:0] in_memdep_HybridCPU_SignExtend_Kernel_avm_writeack,
    input wire [0:0] in_stall_in_0,
    input wire [0:0] in_stall_in_1,
    input wire [63:0] in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdata,
    input wire [0:0] in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdatavalid,
    input wire [0:0] in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_waitrequest,
    input wire [0:0] in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writeack,
    input wire [0:0] in_valid_in_0,
    input wire [0:0] in_valid_in_1,
    input wire [63:0] in_arg_in9_0_tpl,
    input wire [63:0] in_arg_out4_0_tpl,
    input wire clock,
    input wire resetn
    );

    wire [0:0] HybridCPU_SignExtend_Kernel_B1_branch_out_stall_out;
    wire [0:0] HybridCPU_SignExtend_Kernel_B1_branch_out_valid_out_0;
    wire [0:0] HybridCPU_SignExtend_Kernel_B1_branch_out_valid_out_1;
    wire [0:0] HybridCPU_SignExtend_Kernel_B1_merge_out_forked;
    wire [0:0] HybridCPU_SignExtend_Kernel_B1_merge_out_stall_out_0;
    wire [0:0] HybridCPU_SignExtend_Kernel_B1_merge_out_stall_out_1;
    wire [0:0] HybridCPU_SignExtend_Kernel_B1_merge_out_valid_out;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_aclp_to_limiter_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_exiting_stall_out;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_aclp_to_limiter_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_exiting_valid_out;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_fpga_indvars_iv_pop2_struct_0_shuffle_bit3;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_lsu_memdep_o_active;
    wire [34:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_address;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount;
    wire [7:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_enable;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_read;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_write;
    wire [63:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_pipeline_valid_out;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_stall_out;
    wire [34:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount;
    wire [7:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write;
    wire [63:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_valid_out;
    reg [0:0] rst_sync_rst_sclrn;


    // HybridCPU_SignExtend_Kernel_B1_branch(BLACKBOX,2)
    HybridCPU_SignExtend_Kernel_B1_branch theHybridCPU_SignExtend_Kernel_B1_branch (
        .in_fpga_indvars_iv_pop2_struct_0_shuffle_bit3(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_fpga_indvars_iv_pop2_struct_0_shuffle_bit3),
        .in_stall_in_0(in_stall_in_0),
        .in_stall_in_1(in_stall_in_1),
        .in_valid_in(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_valid_out),
        .out_stall_out(HybridCPU_SignExtend_Kernel_B1_branch_out_stall_out),
        .out_valid_out_0(HybridCPU_SignExtend_Kernel_B1_branch_out_valid_out_0),
        .out_valid_out_1(HybridCPU_SignExtend_Kernel_B1_branch_out_valid_out_1),
        .clock(clock),
        .resetn(resetn)
    );

    // HybridCPU_SignExtend_Kernel_B1_merge(BLACKBOX,3)
    HybridCPU_SignExtend_Kernel_B1_merge theHybridCPU_SignExtend_Kernel_B1_merge (
        .in_forked_0(in_forked_0),
        .in_forked_1(in_forked_1),
        .in_stall_in(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_stall_out),
        .in_valid_in_0(in_valid_in_0),
        .in_valid_in_1(in_valid_in_1),
        .out_forked(HybridCPU_SignExtend_Kernel_B1_merge_out_forked),
        .out_stall_out_0(HybridCPU_SignExtend_Kernel_B1_merge_out_stall_out_0),
        .out_stall_out_1(HybridCPU_SignExtend_Kernel_B1_merge_out_stall_out_1),
        .out_valid_out(HybridCPU_SignExtend_Kernel_B1_merge_out_valid_out),
        .clock(clock),
        .resetn(resetn)
    );

    // bb_HybridCPU_SignExtend_Kernel_B1_stall_region(BLACKBOX,4)
    HybridCPU_SignExtend_Kernel_bb_B1_stall_region thebb_HybridCPU_SignExtend_Kernel_B1_stall_region (
        .in_arg_in(in_arg_in),
        .in_arg_out(in_arg_out),
        .in_flush(in_flush),
        .in_forked(HybridCPU_SignExtend_Kernel_B1_merge_out_forked),
        .in_memdep_HybridCPU_SignExtend_Kernel_avm_readdata(in_memdep_HybridCPU_SignExtend_Kernel_avm_readdata),
        .in_memdep_HybridCPU_SignExtend_Kernel_avm_readdatavalid(in_memdep_HybridCPU_SignExtend_Kernel_avm_readdatavalid),
        .in_memdep_HybridCPU_SignExtend_Kernel_avm_waitrequest(in_memdep_HybridCPU_SignExtend_Kernel_avm_waitrequest),
        .in_memdep_HybridCPU_SignExtend_Kernel_avm_writeack(in_memdep_HybridCPU_SignExtend_Kernel_avm_writeack),
        .in_pipeline_stall_in(in_pipeline_stall_in),
        .in_stall_in(HybridCPU_SignExtend_Kernel_B1_branch_out_stall_out),
        .in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdata(in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdata),
        .in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdatavalid(in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdatavalid),
        .in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_waitrequest(in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_waitrequest),
        .in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writeack(in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writeack),
        .in_valid_in(HybridCPU_SignExtend_Kernel_B1_merge_out_valid_out),
        .in_arg_in9_0_tpl(in_arg_in9_0_tpl),
        .in_arg_out4_0_tpl(in_arg_out4_0_tpl),
        .out_aclp_to_limiter_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_exiting_stall_out(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_aclp_to_limiter_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_exiting_stall_out),
        .out_aclp_to_limiter_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_exiting_valid_out(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_aclp_to_limiter_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_exiting_valid_out),
        .out_fpga_indvars_iv_pop2_struct_0_shuffle_bit3(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_fpga_indvars_iv_pop2_struct_0_shuffle_bit3),
        .out_lsu_memdep_o_active(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_lsu_memdep_o_active),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_address(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_address),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_enable(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_enable),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_read(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_read),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_write(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_write),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata),
        .out_pipeline_valid_out(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_pipeline_valid_out),
        .out_stall_out(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_stall_out),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata),
        .out_valid_out(bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_valid_out),
        .clock(clock),
        .resetn(resetn)
    );

    // pipeline_valid_out_sync(GPOUT,6)
    assign out_pipeline_valid_out = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_pipeline_valid_out;

    // out_exiting_stall_out(GPOUT,7)
    assign out_exiting_stall_out = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_aclp_to_limiter_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_exiting_stall_out;

    // out_exiting_valid_out(GPOUT,8)
    assign out_exiting_valid_out = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_aclp_to_limiter_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_exiting_valid_out;

    // out_lsu_memdep_o_active(GPOUT,9)
    assign out_lsu_memdep_o_active = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_lsu_memdep_o_active;

    // out_memdep_HybridCPU_SignExtend_Kernel_avm_address(GPOUT,10)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_address = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_address;

    // out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount(GPOUT,11)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount;

    // out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable(GPOUT,12)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable;

    // out_memdep_HybridCPU_SignExtend_Kernel_avm_enable(GPOUT,13)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_enable = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_enable;

    // out_memdep_HybridCPU_SignExtend_Kernel_avm_read(GPOUT,14)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_read = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_read;

    // out_memdep_HybridCPU_SignExtend_Kernel_avm_write(GPOUT,15)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_write = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_write;

    // out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata(GPOUT,16)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata;

    // out_stall_in_0(GPOUT,17)
    assign out_stall_in_0 = in_stall_in_0;

    // out_stall_out_0(GPOUT,18)
    assign out_stall_out_0 = HybridCPU_SignExtend_Kernel_B1_merge_out_stall_out_0;

    // out_stall_out_1(GPOUT,19)
    assign out_stall_out_1 = HybridCPU_SignExtend_Kernel_B1_merge_out_stall_out_1;

    // out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address(GPOUT,20)
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address;

    // out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount(GPOUT,21)
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount;

    // out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable(GPOUT,22)
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable;

    // out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable(GPOUT,23)
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable;

    // out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read(GPOUT,24)
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read;

    // out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write(GPOUT,25)
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write;

    // out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata(GPOUT,26)
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata = bb_HybridCPU_SignExtend_Kernel_B1_stall_region_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata;

    // out_valid_in_0(GPOUT,27)
    assign out_valid_in_0 = in_valid_in_0;

    // out_valid_in_1(GPOUT,28)
    assign out_valid_in_1 = in_valid_in_1;

    // out_valid_out_0(GPOUT,29)
    assign out_valid_out_0 = HybridCPU_SignExtend_Kernel_B1_branch_out_valid_out_0;

    // out_valid_out_1(GPOUT,30)
    assign out_valid_out_1 = HybridCPU_SignExtend_Kernel_B1_branch_out_valid_out_1;

    // rst_sync(RESETSYNC,50)
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
