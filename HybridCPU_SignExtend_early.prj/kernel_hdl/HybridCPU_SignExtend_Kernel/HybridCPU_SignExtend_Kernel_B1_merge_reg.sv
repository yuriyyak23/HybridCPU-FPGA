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

// SystemVerilog created from HybridCPU_SignExtend_Kernel_B1_merge_reg
// Created for function/kernel HybridCPU_SignExtend_Kernel
// SystemVerilog created on Fri Nov  6 06:17:16 2026


(* altera_attribute = "-name AUTO_SHIFT_REGISTER_RECOGNITION OFF; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 10037; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 15400; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 12020; -name MESSAGE_DISABLE 12030; -name MESSAGE_DISABLE 12010; -name MESSAGE_DISABLE 12110; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 13410; -name MESSAGE_DISABLE 113007; -name MESSAGE_DISABLE 10958" *)
module HybridCPU_SignExtend_Kernel_B1_merge_reg (
    input wire [0:0] in_stall_in,
    output wire [0:0] out_stall_out,
    output wire [0:0] out_data_out_0_tpl,
    output wire [0:0] out_valid_out,
    input wire [0:0] in_data_in_0_tpl,
    input wire [0:0] in_valid_in,
    input wire clock,
    input wire resetn
    );

    wire [0:0] VCC_q;
    reg [0:0] HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_q;
    wire [0:0] HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_and_stall_in_q;
    wire [0:0] HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_not_q;
    wire [0:0] stall_in_not_q;
    wire [0:0] stall_in_not_or_HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_q;
    reg [0:0] HybridCPU_SignExtend_Kernel_B1_merge_reg_data_reg_0_x_q;


    // VCC(CONSTANT,1)
    assign VCC_q = 1'b1;

    // stall_in_not(LOGICAL,6)
    assign stall_in_not_q = ~ (in_stall_in);

    // HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_not(LOGICAL,4)
    assign HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_not_q = ~ (HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_q);

    // stall_in_not_or_HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg(LOGICAL,7)
    assign stall_in_not_or_HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_q = HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_not_q | stall_in_not_q;

    // HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg(REG,2)
    always_ff @ (posedge clock)
    begin
        if (!resetn)
        begin
            HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_q <= $unsigned(1'b0);
        end
        else if (stall_in_not_or_HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_q == 1'b1)
        begin
            HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_q <= in_valid_in;
        end
    end

    // HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_and_stall_in(LOGICAL,3)
    assign HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_and_stall_in_q = HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_q & in_stall_in;

    // sync_out_7(GPOUT,8)@20000000
    assign out_stall_out = HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_and_stall_in_q;

    // HybridCPU_SignExtend_Kernel_B1_merge_reg_data_reg_0_x(REG,9)
    always_ff @ (posedge clock)
    begin
        if (!resetn)
        begin
            HybridCPU_SignExtend_Kernel_B1_merge_reg_data_reg_0_x_q <= $unsigned(1'b0);
        end
        else if (stall_in_not_or_HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_q == 1'b1)
        begin
            HybridCPU_SignExtend_Kernel_B1_merge_reg_data_reg_0_x_q <= in_data_in_0_tpl;
        end
    end

    // sync_out_8_aunroll_x(GPOUT,10)@1
    assign out_data_out_0_tpl = HybridCPU_SignExtend_Kernel_B1_merge_reg_data_reg_0_x_q;
    assign out_valid_out = HybridCPU_SignExtend_Kernel_B1_merge_reg_valid_reg_q;

endmodule
