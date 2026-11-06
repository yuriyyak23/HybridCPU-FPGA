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

// SystemVerilog created from i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg
// Created for function/kernel HybridCPU_SignExtend_Kernel
// SystemVerilog created on Fri Nov  6 06:17:16 2026


(* altera_attribute = "-name AUTO_SHIFT_REGISTER_RECOGNITION OFF; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 10037; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 15400; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 12020; -name MESSAGE_DISABLE 12030; -name MESSAGE_DISABLE 12010; -name MESSAGE_DISABLE 12110; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 13410; -name MESSAGE_DISABLE 113007; -name MESSAGE_DISABLE 10958" *)
module HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i4_fp000000g (
    input wire [0:0] in_stall_in,
    output wire [0:0] out_stall_out,
    output wire [3:0] out_data_out,
    output wire [0:0] out_valid_out,
    input wire [3:0] in_data_in,
    input wire [0:0] in_valid_in,
    input wire clock,
    input wire resetn
    );

    wire [0:0] VCC_q;
    reg [3:0] i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_data_reg_q;
    reg [0:0] i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_q;
    wire [0:0] i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_and_stall_in_q;
    wire [0:0] i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_not_q;
    wire [0:0] stall_in_not_q;
    wire [0:0] stall_in_not_or_i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_q;


    // VCC(CONSTANT,1)
    assign VCC_q = 1'b1;

    // stall_in_not(LOGICAL,7)
    assign stall_in_not_q = ~ (in_stall_in);

    // i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_not(LOGICAL,5)
    assign i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_not_q = ~ (i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_q);

    // stall_in_not_or_i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg(LOGICAL,8)
    assign stall_in_not_or_i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_q = i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_not_q | stall_in_not_q;

    // i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg(REG,3)
    always_ff @ (posedge clock)
    begin
        if (!resetn)
        begin
            i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_q <= $unsigned(1'b0);
        end
        else if (stall_in_not_or_i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_q == 1'b1)
        begin
            i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_q <= in_valid_in;
        end
    end

    // i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_and_stall_in(LOGICAL,4)
    assign i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_and_stall_in_q = i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_q & in_stall_in;

    // sync_out_25(GPOUT,9)@20000000
    assign out_stall_out = i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_and_stall_in_q;

    // i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_data_reg(REG,2)
    always_ff @ (posedge clock)
    begin
        if (!resetn)
        begin
            i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_data_reg_q <= $unsigned(4'b0000);
        end
        else if (stall_in_not_or_i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_q == 1'b1)
        begin
            i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_data_reg_q <= in_data_in;
        end
    end

    // sync_out_26(GPOUT,10)@20000000
    assign out_data_out = i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_data_reg_q;
    assign out_valid_out = i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_reg_valid_reg_q;

endmodule
