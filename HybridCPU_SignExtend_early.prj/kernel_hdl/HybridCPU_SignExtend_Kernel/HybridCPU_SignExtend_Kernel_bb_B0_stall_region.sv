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

// SystemVerilog created from bb_HybridCPU_SignExtend_Kernel_B0_stall_region
// Created for function/kernel HybridCPU_SignExtend_Kernel
// SystemVerilog created on Fri Nov  6 06:17:16 2026


(* altera_attribute = "-name AUTO_SHIFT_REGISTER_RECOGNITION OFF; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 10037; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 15400; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 12020; -name MESSAGE_DISABLE 12030; -name MESSAGE_DISABLE 12010; -name MESSAGE_DISABLE 12110; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 13410; -name MESSAGE_DISABLE 113007; -name MESSAGE_DISABLE 10958" *)
module HybridCPU_SignExtend_Kernel_bb_B0_stall_region (
    input wire [0:0] in_stall_in,
    output wire [0:0] out_stall_out,
    output wire [0:0] out_valid_out,
    input wire [0:0] in_valid_in,
    input wire clock,
    input wire resetn
    );

    wire [0:0] GND_q;
    wire [0:0] HybridCPU_SignExtend_Kernel_B0_merge_reg_out_stall_out;
    wire [0:0] HybridCPU_SignExtend_Kernel_B0_merge_reg_out_valid_out;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B0_merge_reg_wireValid;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B0_merge_reg_backStall;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B0_merge_reg_V0;
    wire [0:0] SE_stall_entry_wireValid;
    wire [0:0] SE_stall_entry_backStall;
    wire [0:0] SE_stall_entry_V0;
    reg [0:0] rst_sync_rst_sclrn;


    // SE_out_HybridCPU_SignExtend_Kernel_B0_merge_reg(STALLENABLE,17)
    // Valid signal propagation
    assign SE_out_HybridCPU_SignExtend_Kernel_B0_merge_reg_V0 = SE_out_HybridCPU_SignExtend_Kernel_B0_merge_reg_wireValid;
    // Backward Stall generation
    assign SE_out_HybridCPU_SignExtend_Kernel_B0_merge_reg_backStall = in_stall_in | ~ (SE_out_HybridCPU_SignExtend_Kernel_B0_merge_reg_wireValid);
    // Computing multiple Valid(s)
    assign SE_out_HybridCPU_SignExtend_Kernel_B0_merge_reg_wireValid = HybridCPU_SignExtend_Kernel_B0_merge_reg_out_valid_out;

    // GND(CONSTANT,0)
    assign GND_q = 1'b0;

    // HybridCPU_SignExtend_Kernel_B0_merge_reg(BLACKBOX,2)@0
    // in in_stall_in@20000000
    // out out_data_out@1
    // out out_stall_out@20000000
    // out out_valid_out@1
    HybridCPU_SignExtend_Kernel_B0_merge_reg theHybridCPU_SignExtend_Kernel_B0_merge_reg (
        .in_data_in(GND_q),
        .in_stall_in(SE_out_HybridCPU_SignExtend_Kernel_B0_merge_reg_backStall),
        .in_valid_in(SE_stall_entry_V0),
        .out_data_out(),
        .out_stall_out(HybridCPU_SignExtend_Kernel_B0_merge_reg_out_stall_out),
        .out_valid_out(HybridCPU_SignExtend_Kernel_B0_merge_reg_out_valid_out),
        .clock(clock),
        .resetn(rst_sync_rst_sclrn[0])
    );

    // SE_stall_entry(STALLENABLE,18)
    // Valid signal propagation
    assign SE_stall_entry_V0 = SE_stall_entry_wireValid;
    // Backward Stall generation
    assign SE_stall_entry_backStall = HybridCPU_SignExtend_Kernel_B0_merge_reg_out_stall_out | ~ (SE_stall_entry_wireValid);
    // Computing multiple Valid(s)
    assign SE_stall_entry_wireValid = in_valid_in;

    // sync_out_5(GPOUT,11)@0
    assign out_stall_out = SE_stall_entry_backStall;

    // sync_out_6(GPOUT,12)@1
    assign out_valid_out = SE_out_HybridCPU_SignExtend_Kernel_B0_merge_reg_V0;

    // rst_sync(RESETSYNC,24)
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
