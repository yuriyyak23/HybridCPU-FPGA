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

// SystemVerilog created from i_sfc_logic_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_365_0gr
// Created for function/kernel HybridCPU_SignExtend_Kernel
// SystemVerilog created on Fri Nov  6 06:17:16 2026


(* altera_attribute = "-name AUTO_SHIFT_REGISTER_RECOGNITION OFF; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 10037; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 15400; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 12020; -name MESSAGE_DISABLE 12030; -name MESSAGE_DISABLE 12010; -name MESSAGE_DISABLE 12110; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 13410; -name MESSAGE_DISABLE 113007; -name MESSAGE_DISABLE 10958" *)
module HybridCPU_SignExtend_Kernel_i_sfc_logic_s_c1_in_fo000000r (
    output wire [0:0] out_c1_exi1_0_tpl,
    output wire [63:0] out_c1_exi1_1_tpl,
    output wire [0:0] out_o_valid,
    output wire [0:0] out_unnamed_HybridCPU_SignExtend_Kernel0,
    input wire [0:0] in_c1_eni1_0_tpl,
    input wire [31:0] in_c1_eni1_1_tpl,
    input wire [0:0] in_i_valid,
    input wire clock,
    input wire resetn
    );

    wire [0:0] GND_q;
    wire [0:0] VCC_q;
    wire [63:0] c_i64_0_368_11_q;
    wire [63:0] c_i64_4294967296_368_12_q;
    wire [0:0] i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_s;
    reg [63:0] i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_q;
    wire [31:0] i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_vt_const_31_q;
    wire [63:0] i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_vt_join_q;
    wire [31:0] i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_vt_select_63_b;
    wire [63:0] i_conv_i_i_hybridcpu_signextend_kernel_368_3gr_vt_join_q;
    wire [31:0] i_conv_i_i_hybridcpu_signextend_kernel_368_3gr_vt_select_31_b;
    wire [63:0] i_or_i_i_hybridcpu_signextend_kernel_368_6gr_q;
    wire [63:0] i_conv_i_i_hybridcpu_signextend_kernel_368_3gr_sel_x_b;
    wire [31:0] i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_0s_v1i32_struct_0_shuffle_bit31_hybridcpu_signextend_kernel_376_0gr_NO_NAME_x_a;
    wire [0:0] i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_0s_v1i32_struct_0_shuffle_bit31_hybridcpu_signextend_kernel_376_0gr_NO_NAME_x_q;


    // i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_vt_const_31(CONSTANT,6)
    assign i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_vt_const_31_q = 32'b00000000000000000000000000000000;

    // i_conv_i_i_hybridcpu_signextend_kernel_368_3gr_sel_x(BITSELECT,15)@78
    assign i_conv_i_i_hybridcpu_signextend_kernel_368_3gr_sel_x_b = {32'b00000000000000000000000000000000, in_c1_eni1_1_tpl[31:0]};

    // i_conv_i_i_hybridcpu_signextend_kernel_368_3gr_vt_select_31(BITSELECT,12)@78
    assign i_conv_i_i_hybridcpu_signextend_kernel_368_3gr_vt_select_31_b = i_conv_i_i_hybridcpu_signextend_kernel_368_3gr_sel_x_b[31:0];

    // i_conv_i_i_hybridcpu_signextend_kernel_368_3gr_vt_join(BITJOIN,11)@78
    assign i_conv_i_i_hybridcpu_signextend_kernel_368_3gr_vt_join_q = {i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_vt_const_31_q, i_conv_i_i_hybridcpu_signextend_kernel_368_3gr_vt_select_31_b};

    // c_i64_4294967296_368_12(CONSTANT,4)
    assign c_i64_4294967296_368_12_q = 64'b1111111111111111111111111111111100000000000000000000000000000000;

    // c_i64_0_368_11(CONSTANT,3)
    assign c_i64_0_368_11_q = 64'b0000000000000000000000000000000000000000000000000000000000000000;

    // i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_0s_v1i32_struct_0_shuffle_bit31_hybridcpu_signextend_kernel_376_0gr_NO_NAME_x(CHOOSEBITS,16)@78
    assign i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_0s_v1i32_struct_0_shuffle_bit31_hybridcpu_signextend_kernel_376_0gr_NO_NAME_x_a = in_c1_eni1_1_tpl;
    assign i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_0s_v1i32_struct_0_shuffle_bit31_hybridcpu_signextend_kernel_376_0gr_NO_NAME_x_q = i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_0s_v1i32_struct_0_shuffle_bit31_hybridcpu_signextend_kernel_376_0gr_NO_NAME_x_a[31:31];

    // VCC(CONSTANT,1)
    assign VCC_q = 1'b1;

    // i_cond_i_i_hybridcpu_signextend_kernel_368_5gr(MUX,5)@78
    assign i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_s = i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_0s_v1i32_struct_0_shuffle_bit31_hybridcpu_signextend_kernel_376_0gr_NO_NAME_x_q;
    always_comb 
    begin
        unique case (i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_s)
            1'b0 : i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_q = c_i64_0_368_11_q;
            1'b1 : i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_q = c_i64_4294967296_368_12_q;
            default : i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_q = 64'b0;
        endcase
    end

    // i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_vt_select_63(BITSELECT,8)@78
    assign i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_vt_select_63_b = i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_q[63:32];

    // i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_vt_join(BITJOIN,7)@78
    assign i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_vt_join_q = {i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_vt_select_63_b, i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_vt_const_31_q};

    // i_or_i_i_hybridcpu_signextend_kernel_368_6gr(LOGICAL,13)@78
    assign i_or_i_i_hybridcpu_signextend_kernel_368_6gr_q = i_cond_i_i_hybridcpu_signextend_kernel_368_5gr_vt_join_q | i_conv_i_i_hybridcpu_signextend_kernel_368_3gr_vt_join_q;

    // GND(CONSTANT,0)
    assign GND_q = 1'b0;

    // sync_out_2_aunroll_x(GPOUT,20)@78
    assign out_c1_exi1_0_tpl = GND_q;
    assign out_c1_exi1_1_tpl = i_or_i_i_hybridcpu_signextend_kernel_368_6gr_q;
    assign out_o_valid = in_i_valid;
    assign out_unnamed_HybridCPU_SignExtend_Kernel0 = GND_q;

endmodule
