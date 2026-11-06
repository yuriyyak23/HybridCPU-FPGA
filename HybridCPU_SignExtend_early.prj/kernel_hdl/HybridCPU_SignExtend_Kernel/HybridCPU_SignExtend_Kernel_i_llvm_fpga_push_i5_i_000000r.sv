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

// SystemVerilog created from i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_0gr
// Created for function/kernel HybridCPU_SignExtend_Kernel
// SystemVerilog created on Fri Nov  6 06:17:16 2026


(* altera_attribute = "-name AUTO_SHIFT_REGISTER_RECOGNITION OFF; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 10037; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 15400; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 12020; -name MESSAGE_DISABLE 12030; -name MESSAGE_DISABLE 12010; -name MESSAGE_DISABLE 12110; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 13410; -name MESSAGE_DISABLE 113007; -name MESSAGE_DISABLE 10958" *)
module HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i5_i_000000r (
    input wire [0:0] in_feedback_stall_in_3,
    output wire [7:0] out_feedback_out_3,
    output wire [0:0] out_feedback_valid_out_3,
    input wire [0:0] in_stall_in,
    output wire [0:0] out_stall_out,
    output wire [4:0] out_data_out,
    output wire [0:0] out_valid_out,
    input wire [0:0] in_c0_ene2,
    input wire [4:0] in_data_in,
    input wire [0:0] in_valid_in,
    input wire clock,
    input wire resetn
    );

    wire [0:0] GND_q;
    wire [2:0] c_i3_0_314_3gr_q;
    wire [7:0] element_extension_314_2_q;
    wire [7:0] i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_data_in;
    wire [0:0] i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_dir;
    wire i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_dir_bitsignaltemp;
    wire [0:0] i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_feedback_stall_in;
    wire i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_feedback_stall_in_bitsignaltemp;
    wire [0:0] i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_predicate;
    wire i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_predicate_bitsignaltemp;
    wire [0:0] i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_stall_in;
    wire i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_stall_in_bitsignaltemp;
    wire [0:0] i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_valid_in;
    wire i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_valid_in_bitsignaltemp;
    wire [7:0] i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_data_out;
    wire [7:0] i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_feedback_out;
    wire [0:0] i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_feedback_valid_out;
    wire i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_feedback_valid_out_bitsignaltemp;
    wire [0:0] i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_stall_out;
    wire i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_stall_out_bitsignaltemp;
    wire [0:0] i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_valid_out;
    wire i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_valid_out_bitsignaltemp;
    wire [4:0] adapt_scalar_trunc_314_4_sel_x_b;


    // GND(CONSTANT,0)
    assign GND_q = 1'b0;

    // c_i3_0_314_3gr(CONSTANT,4)
    assign c_i3_0_314_3gr_q = 3'b000;

    // element_extension_314_2(BITJOIN,5)@4
    assign element_extension_314_2_q = {c_i3_0_314_3gr_q, in_data_in};

    // i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr(EXTIFACE,8)@4
    assign i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_data_in = element_extension_314_2_q;
    assign i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_dir = in_c0_ene2;
    assign i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_feedback_stall_in = in_feedback_stall_in_3;
    assign i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_predicate = GND_q;
    assign i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_stall_in = in_stall_in;
    assign i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_valid_in = in_valid_in;
    assign i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_dir_bitsignaltemp = i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_dir[0];
    assign i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_feedback_stall_in_bitsignaltemp = i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_feedback_stall_in[0];
    assign i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_predicate_bitsignaltemp = i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_predicate[0];
    assign i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_stall_in_bitsignaltemp = i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_stall_in[0];
    assign i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_valid_in_bitsignaltemp = i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_valid_in[0];
    assign i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_feedback_valid_out[0] = i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_feedback_valid_out_bitsignaltemp;
    assign i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_stall_out[0] = i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_stall_out_bitsignaltemp;
    assign i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_valid_out[0] = i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_valid_out_bitsignaltemp;
    acl_push #(
        .FIFO_DEPTH(1),
        .INF_LOOP(0),
        .INF_LOOP_INITIALIZATION(8'b00000000),
        .MIN_FIFO_LATENCY(84),
        .STYLE("REGULAR"),
        .ASYNC_RESET(0),
        .DATA_WIDTH(8),
        .ENABLED(0),
        .RAM_FIFO_DEPTH_INC(0),
        .STALLFREE(1),
        .SYNCHRONIZE_RESET(0)
    ) thei_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr (
        .data_in(element_extension_314_2_q),
        .dir(i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_dir_bitsignaltemp),
        .feedback_stall_in(i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_feedback_stall_in_bitsignaltemp),
        .predicate(i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_predicate_bitsignaltemp),
        .stall_in(i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_stall_in_bitsignaltemp),
        .valid_in(i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_valid_in_bitsignaltemp),
        .data_out(i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_data_out),
        .feedback_out(i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_feedback_out),
        .feedback_valid_out(i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_feedback_valid_out_bitsignaltemp),
        .stall_out(i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_stall_out_bitsignaltemp),
        .valid_out(i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_valid_out_bitsignaltemp),
        .clock(clock),
        .resetn(resetn)
    );

    // feedback_sync_out(GPOUT,7)
    assign out_feedback_out_3 = i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_feedback_out;
    assign out_feedback_valid_out_3 = i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_feedback_valid_out;

    // sync_out_39(GPOUT,10)@4
    assign out_stall_out = i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_stall_out;

    // adapt_scalar_trunc_314_4_sel_x(BITSELECT,13)@4
    assign adapt_scalar_trunc_314_4_sel_x_b = i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_data_out[4:0];

    // sync_out_40(GPOUT,11)@4
    assign out_data_out = adapt_scalar_trunc_314_4_sel_x_b;
    assign out_valid_out = i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_314_1gr_valid_out;

endmodule
