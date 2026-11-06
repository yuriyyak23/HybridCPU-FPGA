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

// SystemVerilog created from HybridCPU_SignExtend_Kernel_function
// Created for function/kernel HybridCPU_SignExtend_Kernel
// SystemVerilog created on Fri Nov  6 06:17:16 2026


(* altera_attribute = "-name AUTO_SHIFT_REGISTER_RECOGNITION OFF; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 10037; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 15400; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 12020; -name MESSAGE_DISABLE 12030; -name MESSAGE_DISABLE 12010; -name MESSAGE_DISABLE 12110; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 13410; -name MESSAGE_DISABLE 113007; -name MESSAGE_DISABLE 10958" *)
module HybridCPU_SignExtend_Kernel_function (
    output wire [34:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_address,
    output wire [0:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount,
    output wire [7:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable,
    output wire [0:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_enable,
    output wire [0:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_read,
    output wire [0:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_write,
    output wire [63:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata,
    output wire [0:0] out_o_active_memdep,
    output wire [0:0] out_stall_out,
    output wire [34:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address,
    output wire [0:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount,
    output wire [7:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable,
    output wire [0:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable,
    output wire [0:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read,
    output wire [0:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write,
    output wire [63:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata,
    output wire [0:0] out_valid_out,
    input wire [63:0] in_arg_arg_in,
    input wire [63:0] in_arg_arg_out,
    input wire [63:0] in_memdep_HybridCPU_SignExtend_Kernel_avm_readdata,
    input wire [0:0] in_memdep_HybridCPU_SignExtend_Kernel_avm_readdatavalid,
    input wire [0:0] in_memdep_HybridCPU_SignExtend_Kernel_avm_waitrequest,
    input wire [0:0] in_memdep_HybridCPU_SignExtend_Kernel_avm_writeack,
    input wire [0:0] in_stall_in,
    input wire [0:0] in_start,
    input wire [63:0] in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdata,
    input wire [0:0] in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdatavalid,
    input wire [0:0] in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_waitrequest,
    input wire [0:0] in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writeack,
    input wire [0:0] in_valid_in,
    input wire [63:0] in_arg_arg_in6_0_tpl,
    input wire [63:0] in_arg_arg_in7_0_tpl,
    input wire [63:0] in_arg_arg_in9_0_tpl,
    input wire [63:0] in_arg_arg_out1_0_tpl,
    input wire [63:0] in_arg_arg_out2_0_tpl,
    input wire [63:0] in_arg_arg_out4_0_tpl,
    input wire clock,
    input wire resetn
    );

    wire [0:0] GND_q;
    wire [0:0] VCC_q;
    wire [0:0] HybridCPU_SignExtend_Kernel_function_i_enable;
    wire HybridCPU_SignExtend_Kernel_function_i_enable_bitsignaltemp;
    wire [0:0] HybridCPU_SignExtend_Kernel_function_i_end;
    wire HybridCPU_SignExtend_Kernel_function_i_end_bitsignaltemp;
    wire [0:0] HybridCPU_SignExtend_Kernel_function_i_start;
    wire HybridCPU_SignExtend_Kernel_function_i_start_bitsignaltemp;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B2_out_stall_out_0;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B2_out_valid_out_0;
    wire [0:0] i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_sr_out_o_stall;
    wire [0:0] i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_sr_out_o_valid;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B0_aunroll_x_out_stall_out_0;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B0_aunroll_x_out_valid_out_0;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_lsu_memdep_o_active;
    wire [34:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_address;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount;
    wire [7:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_enable;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_read;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_write;
    wire [63:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_pipeline_valid_out;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_stall_out_0;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_stall_out_1;
    wire [34:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount;
    wire [7:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write;
    wire [63:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_valid_out_0;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_sr_1_aunroll_x_out_o_stall;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_sr_1_aunroll_x_out_o_valid;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B1_sr_1_aunroll_x_out_o_data_0_tpl;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B2_sr_0_aunroll_x_out_o_stall;
    wire [0:0] bb_HybridCPU_SignExtend_Kernel_B2_sr_0_aunroll_x_out_o_valid;
    reg [0:0] rst_sync_rst_sclrn;


    // GND(CONSTANT,0)
    assign GND_q = 1'b0;

    // i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_sr(BLACKBOX,13)
    HybridCPU_SignExtend_Kernel_i_llvm_fpga_pipeline_k000001r thei_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_sr (
        .in_i_data(GND_q),
        .in_i_stall(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_stall_out_0),
        .in_i_valid(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_pipeline_valid_out),
        .out_o_data(),
        .out_o_stall(i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_sr_out_o_stall),
        .out_o_valid(i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_sr_out_o_valid),
        .clock(clock),
        .resetn(rst_sync_rst_sclrn[0])
    );

    // bb_HybridCPU_SignExtend_Kernel_B0_aunroll_x(BLACKBOX,14)
    HybridCPU_SignExtend_Kernel_bb_B0 thebb_HybridCPU_SignExtend_Kernel_B0_aunroll_x (
        .in_arg_in(in_arg_arg_in),
        .in_arg_out(in_arg_arg_out),
        .in_stall_in_0(bb_HybridCPU_SignExtend_Kernel_B1_sr_1_aunroll_x_out_o_stall),
        .in_valid_in_0(in_valid_in),
        .in_arg_in9_0_tpl(in_arg_arg_in9_0_tpl),
        .in_arg_out4_0_tpl(in_arg_arg_out4_0_tpl),
        .out_stall_out_0(bb_HybridCPU_SignExtend_Kernel_B0_aunroll_x_out_stall_out_0),
        .out_valid_out_0(bb_HybridCPU_SignExtend_Kernel_B0_aunroll_x_out_valid_out_0),
        .clock(clock),
        .resetn(resetn)
    );

    // bb_HybridCPU_SignExtend_Kernel_B1_sr_1_aunroll_x(BLACKBOX,16)
    HybridCPU_SignExtend_Kernel_bb_B1_sr_1 thebb_HybridCPU_SignExtend_Kernel_B1_sr_1_aunroll_x (
        .in_i_stall(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_stall_out_1),
        .in_i_valid(bb_HybridCPU_SignExtend_Kernel_B0_aunroll_x_out_valid_out_0),
        .in_i_data_0_tpl(VCC_q),
        .out_o_stall(bb_HybridCPU_SignExtend_Kernel_B1_sr_1_aunroll_x_out_o_stall),
        .out_o_valid(bb_HybridCPU_SignExtend_Kernel_B1_sr_1_aunroll_x_out_o_valid),
        .out_o_data_0_tpl(bb_HybridCPU_SignExtend_Kernel_B1_sr_1_aunroll_x_out_o_data_0_tpl),
        .clock(clock),
        .resetn(rst_sync_rst_sclrn[0])
    );

    // bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x(BLACKBOX,15)
    HybridCPU_SignExtend_Kernel_bb_B1 thebb_HybridCPU_SignExtend_Kernel_B1_aunroll_x (
        .in_arg_in(in_arg_arg_in),
        .in_arg_out(in_arg_arg_out),
        .in_flush(in_start),
        .in_forked_0(GND_q),
        .in_forked_1(bb_HybridCPU_SignExtend_Kernel_B1_sr_1_aunroll_x_out_o_data_0_tpl),
        .in_memdep_HybridCPU_SignExtend_Kernel_avm_readdata(in_memdep_HybridCPU_SignExtend_Kernel_avm_readdata),
        .in_memdep_HybridCPU_SignExtend_Kernel_avm_readdatavalid(in_memdep_HybridCPU_SignExtend_Kernel_avm_readdatavalid),
        .in_memdep_HybridCPU_SignExtend_Kernel_avm_waitrequest(in_memdep_HybridCPU_SignExtend_Kernel_avm_waitrequest),
        .in_memdep_HybridCPU_SignExtend_Kernel_avm_writeack(in_memdep_HybridCPU_SignExtend_Kernel_avm_writeack),
        .in_pipeline_stall_in(i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_sr_out_o_stall),
        .in_stall_in_0(bb_HybridCPU_SignExtend_Kernel_B2_sr_0_aunroll_x_out_o_stall),
        .in_stall_in_1(GND_q),
        .in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdata(in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdata),
        .in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdatavalid(in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdatavalid),
        .in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_waitrequest(in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_waitrequest),
        .in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writeack(in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writeack),
        .in_valid_in_0(i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_sr_out_o_valid),
        .in_valid_in_1(bb_HybridCPU_SignExtend_Kernel_B1_sr_1_aunroll_x_out_o_valid),
        .in_arg_in9_0_tpl(in_arg_arg_in9_0_tpl),
        .in_arg_out4_0_tpl(in_arg_arg_out4_0_tpl),
        .out_exiting_stall_out(),
        .out_exiting_valid_out(),
        .out_lsu_memdep_o_active(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_lsu_memdep_o_active),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_address(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_address),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_enable(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_enable),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_read(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_read),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_write(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_write),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata),
        .out_pipeline_valid_out(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_pipeline_valid_out),
        .out_stall_in_0(),
        .out_stall_out_0(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_stall_out_0),
        .out_stall_out_1(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_stall_out_1),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata),
        .out_valid_in_0(),
        .out_valid_in_1(),
        .out_valid_out_0(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_valid_out_0),
        .out_valid_out_1(),
        .clock(clock),
        .resetn(resetn)
    );

    // bb_HybridCPU_SignExtend_Kernel_B2_sr_0_aunroll_x(BLACKBOX,17)
    HybridCPU_SignExtend_Kernel_bb_B2_sr_0 thebb_HybridCPU_SignExtend_Kernel_B2_sr_0_aunroll_x (
        .in_i_stall(bb_HybridCPU_SignExtend_Kernel_B2_out_stall_out_0),
        .in_i_valid(bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_valid_out_0),
        .in_i_data_0_tpl(GND_q),
        .out_o_stall(bb_HybridCPU_SignExtend_Kernel_B2_sr_0_aunroll_x_out_o_stall),
        .out_o_valid(bb_HybridCPU_SignExtend_Kernel_B2_sr_0_aunroll_x_out_o_valid),
        .out_o_data_0_tpl(),
        .clock(clock),
        .resetn(rst_sync_rst_sclrn[0])
    );

    // bb_HybridCPU_SignExtend_Kernel_B2(BLACKBOX,4)
    HybridCPU_SignExtend_Kernel_bb_B2 thebb_HybridCPU_SignExtend_Kernel_B2 (
        .in_stall_in_0(in_stall_in),
        .in_valid_in_0(bb_HybridCPU_SignExtend_Kernel_B2_sr_0_aunroll_x_out_o_valid),
        .out_stall_out_0(bb_HybridCPU_SignExtend_Kernel_B2_out_stall_out_0),
        .out_valid_out_0(bb_HybridCPU_SignExtend_Kernel_B2_out_valid_out_0),
        .clock(clock),
        .resetn(resetn)
    );

    // VCC(CONSTANT,1)
    assign VCC_q = 1'b1;

    // HybridCPU_SignExtend_Kernel_function(EXTIFACE,3)
    assign HybridCPU_SignExtend_Kernel_function_i_enable = VCC_q;
    assign HybridCPU_SignExtend_Kernel_function_i_end = bb_HybridCPU_SignExtend_Kernel_B2_out_valid_out_0;
    assign HybridCPU_SignExtend_Kernel_function_i_start = in_start;
    assign HybridCPU_SignExtend_Kernel_function_i_enable_bitsignaltemp = HybridCPU_SignExtend_Kernel_function_i_enable[0];
    assign HybridCPU_SignExtend_Kernel_function_i_end_bitsignaltemp = HybridCPU_SignExtend_Kernel_function_i_end[0];
    assign HybridCPU_SignExtend_Kernel_function_i_start_bitsignaltemp = HybridCPU_SignExtend_Kernel_function_i_start[0];
    hld_sim_latency_tracker #(
        .ADDITIONAL_START_LATENCY(9),
        .CRA_CONTROL(1),
        .IS_COMPONENT(1),
        .NAME("HybridCPU_SignExtend_Kernel")
    ) theHybridCPU_SignExtend_Kernel_function (
        .i_enable(HybridCPU_SignExtend_Kernel_function_i_enable_bitsignaltemp),
        .i_end(HybridCPU_SignExtend_Kernel_function_i_end_bitsignaltemp),
        .i_start(HybridCPU_SignExtend_Kernel_function_i_start_bitsignaltemp),
        .clock(clock),
        .resetn(rst_sync_rst_sclrn[0])
    );

    // out_memdep_HybridCPU_SignExtend_Kernel_avm_address(GPOUT,18)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_address = bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_address;

    // out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount(GPOUT,19)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount = bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount;

    // out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable(GPOUT,20)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable = bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable;

    // out_memdep_HybridCPU_SignExtend_Kernel_avm_enable(GPOUT,21)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_enable = bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_enable;

    // out_memdep_HybridCPU_SignExtend_Kernel_avm_read(GPOUT,22)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_read = bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_read;

    // out_memdep_HybridCPU_SignExtend_Kernel_avm_write(GPOUT,23)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_write = bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_write;

    // out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata(GPOUT,24)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata = bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata;

    // out_o_active_memdep(GPOUT,25)
    assign out_o_active_memdep = bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_lsu_memdep_o_active;

    // out_stall_out(GPOUT,26)
    assign out_stall_out = bb_HybridCPU_SignExtend_Kernel_B0_aunroll_x_out_stall_out_0;

    // out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address(GPOUT,27)
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address = bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address;

    // out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount(GPOUT,28)
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount = bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount;

    // out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable(GPOUT,29)
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable = bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable;

    // out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable(GPOUT,30)
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable = bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable;

    // out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read(GPOUT,31)
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read = bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read;

    // out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write(GPOUT,32)
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write = bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write;

    // out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata(GPOUT,33)
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata = bb_HybridCPU_SignExtend_Kernel_B1_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata;

    // out_valid_out(GPOUT,34)
    assign out_valid_out = bb_HybridCPU_SignExtend_Kernel_B2_out_valid_out_0;

    // rst_sync(RESETSYNC,54)
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
