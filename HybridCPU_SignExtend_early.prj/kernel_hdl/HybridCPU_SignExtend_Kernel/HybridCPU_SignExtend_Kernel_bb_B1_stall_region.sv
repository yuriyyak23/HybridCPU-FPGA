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

// SystemVerilog created from bb_HybridCPU_SignExtend_Kernel_B1_stall_region
// Created for function/kernel HybridCPU_SignExtend_Kernel
// SystemVerilog created on Fri Nov  6 06:17:16 2026


(* altera_attribute = "-name AUTO_SHIFT_REGISTER_RECOGNITION OFF; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 10037; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 15400; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 12020; -name MESSAGE_DISABLE 12030; -name MESSAGE_DISABLE 12010; -name MESSAGE_DISABLE 12110; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 13410; -name MESSAGE_DISABLE 113007; -name MESSAGE_DISABLE 10958" *)
module HybridCPU_SignExtend_Kernel_bb_B1_stall_region (
    input wire [63:0] in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdata,
    input wire [0:0] in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writeack,
    input wire [0:0] in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_waitrequest,
    input wire [0:0] in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdatavalid,
    output wire [0:0] out_aclp_to_limiter_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_exiting_valid_out,
    output wire [0:0] out_aclp_to_limiter_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_exiting_stall_out,
    input wire [0:0] in_pipeline_stall_in,
    output wire [0:0] out_pipeline_valid_out,
    input wire [63:0] in_arg_in,
    input wire [63:0] in_arg_out,
    input wire [0:0] in_flush,
    input wire [0:0] in_stall_in,
    output wire [0:0] out_stall_out,
    output wire [0:0] out_fpga_indvars_iv_pop2_struct_0_shuffle_bit3,
    output wire [0:0] out_valid_out,
    input wire [0:0] in_forked,
    input wire [0:0] in_valid_in,
    input wire [63:0] in_memdep_HybridCPU_SignExtend_Kernel_avm_readdata,
    input wire [0:0] in_memdep_HybridCPU_SignExtend_Kernel_avm_writeack,
    input wire [0:0] in_memdep_HybridCPU_SignExtend_Kernel_avm_waitrequest,
    input wire [0:0] in_memdep_HybridCPU_SignExtend_Kernel_avm_readdatavalid,
    output wire [34:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address,
    output wire [0:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable,
    output wire [0:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read,
    output wire [0:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write,
    output wire [63:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata,
    output wire [7:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable,
    output wire [0:0] out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount,
    output wire [34:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_address,
    output wire [0:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_enable,
    output wire [0:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_read,
    output wire [0:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_write,
    output wire [63:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata,
    output wire [7:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable,
    output wire [0:0] out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount,
    output wire [0:0] out_lsu_memdep_o_active,
    input wire [63:0] in_arg_in9_0_tpl,
    input wire [63:0] in_arg_out4_0_tpl,
    input wire clock,
    input wire resetn
    );

    wire [0:0] GND_q;
    wire [0:0] VCC_q;
    wire [3:0] c_i4_1_77_23_q;
    wire [3:0] c_i4_6_77_20_q;
    wire [4:0] i_fpga_indvars_iv_next_hybridcpu_signextend_kernel_77_4gr_a;
    wire [4:0] i_fpga_indvars_iv_next_hybridcpu_signextend_kernel_77_4gr_b;
    logic [4:0] i_fpga_indvars_iv_next_hybridcpu_signextend_kernel_77_4gr_o;
    wire [4:0] i_fpga_indvars_iv_next_hybridcpu_signextend_kernel_77_4gr_q;
    wire [0:0] i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_lsu_memdep_o_active;
    wire [34:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_address;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount;
    wire [7:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_enable;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_read;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_write;
    wire [63:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_o_stall;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_o_valid;
    wire [0:0] i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_o_writeack;
    wire [31:0] i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_o_readdata;
    wire [0:0] i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_o_stall;
    wire [0:0] i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_o_valid;
    wire [34:0] i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address;
    wire [0:0] i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount;
    wire [7:0] i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable;
    wire [0:0] i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable;
    wire [0:0] i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read;
    wire [0:0] i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write;
    wire [63:0] i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata;
    wire [0:0] i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_exiting_stall_out;
    wire [0:0] i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_exiting_valid_out;
    wire [0:0] i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_not_exitcond_stall_out;
    wire [0:0] i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_pipeline_valid_out;
    wire [0:0] i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_stall_out;
    wire [0:0] i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_valid_out;
    wire [0:0] i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_out_data_out;
    wire [0:0] i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_out_feedback_stall_out_4;
    wire [0:0] i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_out_stall_out;
    wire [0:0] i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_out_valid_out;
    wire [3:0] i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_out_data_out;
    wire [0:0] i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_out_feedback_stall_out_2;
    wire [0:0] i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_out_stall_out;
    wire [0:0] i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_out_valid_out;
    wire [0:0] i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17_out_feedback_out_4;
    wire [0:0] i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17_out_feedback_valid_out_4;
    wire [0:0] i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17_out_stall_out;
    wire [0:0] i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17_out_valid_out;
    wire [0:0] i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr_out_feedback_out_1;
    wire [0:0] i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr_out_feedback_valid_out_1;
    wire [0:0] i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr_out_stall_out;
    wire [0:0] i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr_out_valid_out;
    wire [7:0] i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_out_feedback_out_2;
    wire [0:0] i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_out_feedback_valid_out_2;
    wire [0:0] i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_out_stall_out;
    wire [0:0] i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_out_valid_out;
    wire [0:0] HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_stall_out;
    wire [0:0] HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_valid_out;
    wire [0:0] HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl;
    wire [3:0] bgTrunc_i_fpga_indvars_iv_next_hybridcpu_signextend_kernel_77_4gr_sel_x_b;
    wire [7:0] i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_a;
    wire [0:0] i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q;
    wire [3:0] i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_c_i4_0_148_4gr_x_q;
    wire [7:0] i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_element_extension_148_3_x_q;
    wire [0:0] i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_o_stall;
    wire [0:0] i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_o_valid;
    wire [63:0] i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_1_tpl;
    wire [63:0] i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl;
    wire [0:0] i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x_out_o_stall;
    wire [0:0] i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x_out_o_valid;
    wire [63:0] i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x_out_c1_exit_1_tpl;
    wire [0:0] redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_i_valid;
    wire redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_i_valid_bitsignaltemp;
    wire [0:0] redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_i_stall;
    wire redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_i_stall_bitsignaltemp;
    wire [63:0] redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_i_data;
    wire [0:0] redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_o_valid;
    wire redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_o_valid_bitsignaltemp;
    wire [0:0] redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_o_stall;
    wire redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_o_stall_bitsignaltemp;
    wire [63:0] redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_o_data;
    wire [0:0] redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_i_valid;
    wire redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_i_valid_bitsignaltemp;
    wire [0:0] redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_i_stall;
    wire redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_i_stall_bitsignaltemp;
    wire [0:0] redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_i_data;
    wire [0:0] redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_o_valid;
    wire redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_o_valid_bitsignaltemp;
    wire [0:0] redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_o_stall;
    wire redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_o_stall_bitsignaltemp;
    wire [0:0] redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_o_data;
    reg [0:0] redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_q;
    reg [0:0] redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_q;
    reg [0:0] redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_q;
    reg [0:0] redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_q;
    reg [0:0] redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_q;
    reg [0:0] redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_q;
    wire [0:0] redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_i_valid;
    wire redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_i_valid_bitsignaltemp;
    wire [0:0] redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_i_stall;
    wire redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_i_stall_bitsignaltemp;
    wire [0:0] redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_i_data;
    wire [0:0] redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_o_valid;
    wire redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_o_valid_bitsignaltemp;
    wire [0:0] redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_o_stall;
    wire redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_o_stall_bitsignaltemp;
    wire [0:0] redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_o_data;
    reg [0:0] redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_q;
    reg [0:0] redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_q;
    reg [0:0] redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_q;
    reg [0:0] redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_q;
    reg [0:0] redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_q;
    reg [0:0] redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_q;
    reg [0:0] redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_q;
    wire [0:0] bubble_join_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_q;
    wire [0:0] bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_b;
    wire [31:0] bubble_join_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_q;
    wire [31:0] bubble_select_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_b;
    wire [0:0] bubble_join_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_q;
    wire [0:0] bubble_select_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_b;
    wire [3:0] bubble_join_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_q;
    wire [3:0] bubble_select_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_b;
    wire [0:0] bubble_join_stall_entry_q;
    wire [0:0] bubble_select_stall_entry_b;
    wire [0:0] bubble_join_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_q;
    wire [0:0] bubble_select_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_b;
    wire [127:0] bubble_join_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_q;
    wire [63:0] bubble_select_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_b;
    wire [63:0] bubble_select_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_c;
    wire [63:0] bubble_join_i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x_q;
    wire [63:0] bubble_select_i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x_b;
    wire [63:0] bubble_join_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_q;
    wire [63:0] bubble_select_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_b;
    wire [0:0] bubble_join_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_q;
    wire [0:0] bubble_select_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_b;
    wire [0:0] bubble_join_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_q;
    wire [0:0] bubble_select_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_b;
    wire [0:0] SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_wireValid;
    wire [0:0] SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_wireStall;
    wire [0:0] SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_StallValid;
    wire [0:0] SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_toReg0;
    reg [0:0] SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_fromReg0;
    wire [0:0] SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_consumed0;
    wire [0:0] SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_toReg1;
    reg [0:0] SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_fromReg1;
    wire [0:0] SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_consumed1;
    wire [0:0] SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_or0;
    wire [0:0] SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_backStall;
    wire [0:0] SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_V0;
    wire [0:0] SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_V1;
    wire [0:0] SE_out_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_wireValid;
    wire [0:0] SE_out_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_backStall;
    wire [0:0] SE_out_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_V0;
    wire [0:0] SE_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_wireValid;
    wire [0:0] SE_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_backStall;
    wire [0:0] SE_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_V0;
    wire [0:0] SE_out_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_wireValid;
    wire [0:0] SE_out_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_and0;
    wire [0:0] SE_out_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_backStall;
    wire [0:0] SE_out_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_V0;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_wireValid;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_wireStall;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_StallValid;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_toReg0;
    reg [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg0;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_consumed0;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_toReg1;
    reg [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg1;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_consumed1;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_toReg2;
    reg [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg2;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_consumed2;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_toReg3;
    reg [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg3;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_consumed3;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_or0;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_or1;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_or2;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_backStall;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_V0;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_V1;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_V2;
    wire [0:0] SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_V3;
    wire [0:0] SE_out_i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17_wireValid;
    wire [0:0] SE_out_i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17_backStall;
    wire [0:0] SE_out_i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr_wireValid;
    wire [0:0] SE_out_i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr_backStall;
    wire [0:0] SE_out_i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_wireValid;
    wire [0:0] SE_out_i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_backStall;
    wire [0:0] SE_stall_entry_wireValid;
    wire [0:0] SE_stall_entry_backStall;
    wire [0:0] SE_stall_entry_V0;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_wireValid;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_wireStall;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_StallValid;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_toReg0;
    reg [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_fromReg0;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_consumed0;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_toReg1;
    reg [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_fromReg1;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_consumed1;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_toReg2;
    reg [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_fromReg2;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_consumed2;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_or0;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_or1;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_backStall;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_V0;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_V1;
    wire [0:0] SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_V2;
    wire [0:0] SE_in_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_wireValid;
    wire [0:0] SE_in_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_and0;
    wire [0:0] SE_in_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_backStall;
    wire [0:0] SE_in_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_V0;
    wire [0:0] SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_wireValid;
    wire [0:0] SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_wireStall;
    wire [0:0] SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_StallValid;
    wire [0:0] SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_toReg0;
    reg [0:0] SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_fromReg0;
    wire [0:0] SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_consumed0;
    wire [0:0] SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_toReg1;
    reg [0:0] SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_fromReg1;
    wire [0:0] SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_consumed1;
    wire [0:0] SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_or0;
    wire [0:0] SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_backStall;
    wire [0:0] SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_V0;
    wire [0:0] SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_V1;
    wire [0:0] SE_out_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_wireValid;
    wire [0:0] SE_out_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_and0;
    wire [0:0] SE_out_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_backStall;
    wire [0:0] SE_out_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_V0;
    wire [0:0] SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_wireValid;
    wire [0:0] SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_and0;
    wire [0:0] SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_and1;
    wire [0:0] SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_backStall;
    wire [0:0] SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_V0;
    reg [0:0] SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_R_v_0;
    reg [0:0] SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_R_v_1;
    wire [0:0] SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_v_s_0;
    wire [0:0] SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_s_tv_0;
    wire [0:0] SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_s_tv_1;
    wire [0:0] SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_backEN;
    wire [0:0] SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_or0;
    wire [0:0] SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_backStall;
    wire [0:0] SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_V0;
    wire [0:0] SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_V1;
    reg [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_R_v_0;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_v_s_0;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_s_tv_0;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_backEN;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_backStall;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_V0;
    reg [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_R_v_0;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_v_s_0;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_s_tv_0;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_backEN;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_backStall;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_V0;
    reg [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_R_v_0;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_v_s_0;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_s_tv_0;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_backEN;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_backStall;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_V0;
    reg [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_R_v_0;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_v_s_0;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_s_tv_0;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_backEN;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_backStall;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_V0;
    reg [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_R_v_0;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_v_s_0;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_s_tv_0;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_backEN;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_backStall;
    wire [0:0] SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_V0;
    wire [0:0] SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_wireValid;
    wire [0:0] SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_wireStall;
    wire [0:0] SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_StallValid;
    wire [0:0] SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_toReg0;
    reg [0:0] SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_fromReg0;
    wire [0:0] SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_consumed0;
    wire [0:0] SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_toReg1;
    reg [0:0] SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_fromReg1;
    wire [0:0] SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_consumed1;
    wire [0:0] SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_or0;
    wire [0:0] SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_backStall;
    wire [0:0] SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_V0;
    wire [0:0] SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_V1;
    reg [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_R_v_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_v_s_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_s_tv_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_backEN;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_backStall;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_V0;
    reg [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_R_v_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_v_s_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_s_tv_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_backEN;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_backStall;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_V0;
    reg [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_R_v_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_v_s_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_s_tv_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_backEN;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_backStall;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_V0;
    reg [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_R_v_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_v_s_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_s_tv_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_backEN;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_backStall;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_V0;
    reg [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_R_v_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_v_s_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_s_tv_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_backEN;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_backStall;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_V0;
    reg [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_R_v_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_v_s_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_s_tv_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_backEN;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_backStall;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_V0;
    reg [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_R_v_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_v_s_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_s_tv_0;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_backEN;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_backStall;
    wire [0:0] SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_V0;
    wire [0:0] SE_bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_wireValid;
    wire [0:0] SE_bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_and0;
    wire [0:0] SE_bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_backStall;
    wire [0:0] SE_bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_V0;
    wire [0:0] SE_out_bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_wireValid;
    wire [0:0] SE_out_bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_backStall;
    wire [0:0] SE_out_bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_V0;
    wire [0:0] bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_i_valid;
    wire bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_i_valid_bitsignaltemp;
    wire [0:0] bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_i_stall;
    wire bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_i_stall_bitsignaltemp;
    wire [0:0] bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_o_valid;
    wire bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_o_valid_bitsignaltemp;
    wire [0:0] bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_o_stall;
    wire bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_o_stall_bitsignaltemp;
    wire [0:0] bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_i_valid;
    wire bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_i_valid_bitsignaltemp;
    wire [0:0] bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_i_stall;
    wire bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_i_stall_bitsignaltemp;
    wire [0:0] bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_o_valid;
    wire bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_o_valid_bitsignaltemp;
    wire [0:0] bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_o_stall;
    wire bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_o_stall_bitsignaltemp;
    wire [0:0] SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_i_valid;
    reg [0:0] SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_r_valid;
    reg [0:0] SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_r_data0;
    wire [0:0] SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_backStall;
    wire [0:0] SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_V;
    wire [0:0] SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_D0;
    reg [0:0] rst_sync_rst_sclrn;


    // i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_c_i4_0_148_4gr_x(CONSTANT,64)
    assign i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_c_i4_0_148_4gr_x_q = 4'b0000;

    // bubble_join_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr(BITJOIN,108)
    assign bubble_join_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_q = i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_out_data_out;

    // bubble_select_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr(BITSELECT,109)
    assign bubble_select_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_b = bubble_join_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_q[3:0];

    // i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_element_extension_148_3_x(BITJOIN,66)@2
    assign i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_element_extension_148_3_x_q = {i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_c_i4_0_148_4gr_x_q, bubble_select_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_b};

    // i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x(CHOOSEBITS,62)@2
    assign i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_a = i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_element_extension_148_3_x_q;
    assign i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q = i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_a[3:3];

    // i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr(LOGICAL,14)@2
    assign i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q = i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q ^ VCC_q;

    // bubble_join_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo(BITJOIN,131)
    assign bubble_join_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_q = redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_o_data;

    // bubble_select_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo(BITSELECT,132)
    assign bubble_select_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_b = bubble_join_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_q[0:0];

    // redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0(REG,89)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_q <= 1'b0;
        end
        else if (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_backEN == 1'b1)
        begin
            redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_q <= bubble_select_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_b;
        end
    end

    // redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1(REG,90)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_q <= 1'b0;
        end
        else if (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_backEN == 1'b1)
        begin
            redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_q <= redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_q;
        end
    end

    // redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2(REG,91)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_q <= 1'b0;
        end
        else if (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_backEN == 1'b1)
        begin
            redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_q <= redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_q;
        end
    end

    // redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3(REG,92)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_q <= 1'b0;
        end
        else if (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_backEN == 1'b1)
        begin
            redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_q <= redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_q;
        end
    end

    // SE_out_i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17(STALLENABLE,146)
    // Backward Stall generation
    assign SE_out_i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17_backStall = $unsigned(1'b0);
    // Computing multiple Valid(s)
    assign SE_out_i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17_wireValid = i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17_out_valid_out;

    // redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4(REG,93)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_q <= 1'b0;
        end
        else if (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_backEN == 1'b1)
        begin
            redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_q <= SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_D0;
        end
    end

    // redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5(REG,94)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_q <= 1'b0;
        end
        else if (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_backEN == 1'b1)
        begin
            redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_q <= redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_q;
        end
    end

    // redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6(REG,95)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_q <= 1'b0;
        end
        else if (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_backEN == 1'b1)
        begin
            redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_q <= redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_q;
        end
    end

    // SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0(STALLENABLE,167)
    // Valid signal propagation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_V0 = SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_R_v_0;
    // Stall signal propagation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_s_tv_0 = SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_backStall & SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_R_v_0;
    // Backward Enable generation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_backEN = ~ (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_s_tv_0);
    // Determine whether to write valid data into the first register stage
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_v_s_0 = SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_backEN & SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_V1;
    // Backward Stall generation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_backStall = ~ (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_v_s_0);
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_R_v_0 <= 1'b0;
        end
        else
        begin
            if (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_backEN == 1'b0)
            begin
                SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_R_v_0 <= SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_R_v_0 & SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_s_tv_0;
            end
            else
            begin
                SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_R_v_0 <= SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_v_s_0;
            end

        end
    end

    // SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1(STALLENABLE,168)
    // Valid signal propagation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_V0 = SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_R_v_0;
    // Stall signal propagation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_s_tv_0 = SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_backStall & SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_R_v_0;
    // Backward Enable generation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_backEN = ~ (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_s_tv_0);
    // Determine whether to write valid data into the first register stage
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_v_s_0 = SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_backEN & SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_V0;
    // Backward Stall generation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_backStall = ~ (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_v_s_0);
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_R_v_0 <= 1'b0;
        end
        else
        begin
            if (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_backEN == 1'b0)
            begin
                SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_R_v_0 <= SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_R_v_0 & SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_s_tv_0;
            end
            else
            begin
                SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_R_v_0 <= SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_v_s_0;
            end

        end
    end

    // SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2(STALLENABLE,169)
    // Valid signal propagation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_V0 = SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_R_v_0;
    // Stall signal propagation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_s_tv_0 = SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_backStall & SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_R_v_0;
    // Backward Enable generation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_backEN = ~ (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_s_tv_0);
    // Determine whether to write valid data into the first register stage
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_v_s_0 = SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_backEN & SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_V0;
    // Backward Stall generation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_backStall = ~ (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_v_s_0);
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_R_v_0 <= 1'b0;
        end
        else
        begin
            if (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_backEN == 1'b0)
            begin
                SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_R_v_0 <= SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_R_v_0 & SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_s_tv_0;
            end
            else
            begin
                SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_R_v_0 <= SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_v_s_0;
            end

        end
    end

    // SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3(STALLENABLE,170)
    // Valid signal propagation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_V0 = SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_R_v_0;
    // Stall signal propagation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_s_tv_0 = SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_backStall & SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_R_v_0;
    // Backward Enable generation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_backEN = ~ (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_s_tv_0);
    // Determine whether to write valid data into the first register stage
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_v_s_0 = SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_backEN & SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_V0;
    // Backward Stall generation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_backStall = ~ (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_v_s_0);
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_R_v_0 <= 1'b0;
        end
        else
        begin
            if (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_backEN == 1'b0)
            begin
                SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_R_v_0 <= SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_R_v_0 & SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_s_tv_0;
            end
            else
            begin
                SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_R_v_0 <= SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_v_s_0;
            end

        end
    end

    // SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4(STALLENABLE,171)
    // Valid signal propagation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_V0 = SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_R_v_0;
    // Stall signal propagation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_s_tv_0 = i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_out_stall_out & SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_R_v_0;
    // Backward Enable generation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_backEN = ~ (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_s_tv_0);
    // Determine whether to write valid data into the first register stage
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_v_s_0 = SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_backEN & SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_V0;
    // Backward Stall generation
    assign SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_backStall = ~ (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_v_s_0);
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_R_v_0 <= 1'b0;
        end
        else
        begin
            if (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_backEN == 1'b0)
            begin
                SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_R_v_0 <= SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_R_v_0 & SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_s_tv_0;
            end
            else
            begin
                SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_R_v_0 <= SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_v_s_0;
            end

        end
    end

    // bubble_join_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12(BITJOIN,101)
    assign bubble_join_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_q = i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_o_readdata;

    // bubble_select_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12(BITSELECT,102)
    assign bubble_select_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_b = bubble_join_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_q[31:0];

    // bubble_join_i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x(BITJOIN,122)
    assign bubble_join_i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x_q = i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x_out_c1_exit_1_tpl;

    // bubble_select_i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x(BITSELECT,123)
    assign bubble_select_i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x_b = bubble_join_i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x_q[63:0];

    // bubble_join_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo(BITJOIN,125)
    assign bubble_join_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_q = redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_o_data;

    // bubble_select_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo(BITSELECT,126)
    assign bubble_select_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_b = bubble_join_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_q[63:0];

    // i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16(BLACKBOX,15)@82
    // in in_i_stall@20000000
    // out out_lsu_memdep_o_active@20000000
    // out out_memdep_HybridCPU_SignExtend_Kernel_avm_address@20000000
    // out out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount@20000000
    // out out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable@20000000
    // out out_memdep_HybridCPU_SignExtend_Kernel_avm_enable@20000000
    // out out_memdep_HybridCPU_SignExtend_Kernel_avm_read@20000000
    // out out_memdep_HybridCPU_SignExtend_Kernel_avm_write@20000000
    // out out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata@20000000
    // out out_o_stall@20000000
    // out out_o_valid@91
    // out out_o_writeack@91
    HybridCPU_SignExtend_Kernel_i_llvm_fpga_mem_memdep000000r thei_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16 (
        .in_flush(in_flush),
        .in_i_address(bubble_select_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_b),
        .in_i_predicate(GND_q),
        .in_i_stall(SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_backStall),
        .in_i_valid(SE_out_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_V0),
        .in_i_writedata(bubble_select_i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x_b),
        .in_memdep_HybridCPU_SignExtend_Kernel_avm_readdata(in_memdep_HybridCPU_SignExtend_Kernel_avm_readdata),
        .in_memdep_HybridCPU_SignExtend_Kernel_avm_readdatavalid(in_memdep_HybridCPU_SignExtend_Kernel_avm_readdatavalid),
        .in_memdep_HybridCPU_SignExtend_Kernel_avm_waitrequest(in_memdep_HybridCPU_SignExtend_Kernel_avm_waitrequest),
        .in_memdep_HybridCPU_SignExtend_Kernel_avm_writeack(in_memdep_HybridCPU_SignExtend_Kernel_avm_writeack),
        .out_lsu_memdep_o_active(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_lsu_memdep_o_active),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_address(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_address),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_enable(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_enable),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_read(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_read),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_write(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_write),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata),
        .out_o_stall(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_o_stall),
        .out_o_valid(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_o_valid),
        .out_o_writeack(i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_o_writeack),
        .clock(clock),
        .resetn(resetn)
    );

    // redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo(STALLFIFO,80)
    assign redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_i_valid = SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_V1;
    assign redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_i_stall = SE_out_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_backStall;
    assign redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_i_data = bubble_select_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_c;
    assign redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_i_valid_bitsignaltemp = redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_i_valid[0];
    assign redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_i_stall_bitsignaltemp = redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_i_stall[0];
    assign redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_o_valid[0] = redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_o_valid_bitsignaltemp;
    assign redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_o_stall[0] = redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_o_stall_bitsignaltemp;
    hld_fifo #(
        .DEPTH(75),
        .WIDTH(64),
        .STYLE("ms"),
        .ALMOST_FULL_CUTOFF(0),
        .ALMOST_EMPTY_CUTOFF(0),
        .STALL_IN_EARLINESS(0),
        .VALID_IN_EARLINESS(0),
        .USE_STALL_LATENCY_UPSTREAM(0),
        .USE_STALL_LATENCY_DOWNSTREAM(0),
        .NEVER_OVERFLOWS(0),
        .RESET_EXTERNALLY_HELD(1),
        .ASYNC_RESET(0),
        .SYNCHRONIZE_RESET(1)
    ) theredist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo (
        .i_valid(redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_i_valid_bitsignaltemp),
        .i_stall(redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_i_stall_bitsignaltemp),
        .i_data(bubble_select_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_c),
        .o_valid(redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_o_valid_bitsignaltemp),
        .o_stall(redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_o_stall_bitsignaltemp),
        .o_data(redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_o_data),
        .clock(clock),
        .resetn(resetn)
    );

    // SE_out_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo(STALLENABLE,163)
    // Valid signal propagation
    assign SE_out_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_V0 = SE_out_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_wireValid;
    // Backward Stall generation
    assign SE_out_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_backStall = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_o_stall | ~ (SE_out_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_wireValid);
    // Computing multiple Valid(s)
    assign SE_out_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_and0 = redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_o_valid;
    assign SE_out_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_wireValid = i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x_out_o_valid & SE_out_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_and0;

    // i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x(BLACKBOX,69)@78
    // in in_i_stall@20000000
    // out out_o_stall@20000000
    // out out_o_valid@82
    // out out_c1_exit_0_tpl@82
    // out out_c1_exit_1_tpl@82
    HybridCPU_SignExtend_Kernel_i_sfc_s_c1_in_for_body0000004 thei_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x (
        .in_i_stall(SE_out_redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_backStall),
        .in_i_valid(SE_out_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_V0),
        .in_c1_eni1_0_tpl(GND_q),
        .in_c1_eni1_1_tpl(bubble_select_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_b),
        .out_o_stall(i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x_out_o_stall),
        .out_o_valid(i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x_out_o_valid),
        .out_c1_exit_0_tpl(),
        .out_c1_exit_1_tpl(i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x_out_c1_exit_1_tpl),
        .clock(clock),
        .resetn(resetn)
    );

    // SE_out_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12(STALLENABLE,138)
    // Valid signal propagation
    assign SE_out_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_V0 = SE_out_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_wireValid;
    // Backward Stall generation
    assign SE_out_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_backStall = i_sfc_s_c1_in_for_body_i_hybridcpu_signextend_kernels_c1_enter_hybridcpu_signextend_kernel_77_14_aunroll_x_out_o_stall | ~ (SE_out_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_wireValid);
    // Computing multiple Valid(s)
    assign SE_out_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_wireValid = i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_o_valid;

    // bubble_join_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr(BITJOIN,105)
    assign bubble_join_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_q = i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_out_data_out;

    // bubble_select_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr(BITSELECT,106)
    assign bubble_select_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_b = bubble_join_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_q[0:0];

    // redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0(REG,82)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_q <= 1'b0;
        end
        else if (SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_backEN == 1'b1)
        begin
            redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_q <= bubble_select_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_b;
        end
    end

    // i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x(BLACKBOX,68)@2
    // in in_i_stall@20000000
    // out out_o_stall@20000000
    // out out_o_valid@8
    // out out_c0_exit_0_tpl@8
    // out out_c0_exit_1_tpl@8
    // out out_c0_exit_2_tpl@8
    HybridCPU_SignExtend_Kernel_i_sfc_s_c0_in_for_body0000000 thei_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x (
        .in_arg_in(in_arg_in),
        .in_arg_out(in_arg_out),
        .in_forked(redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_q),
        .in_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not(i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q),
        .in_i_stall(SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_backStall),
        .in_i_valid(SE_in_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_V0),
        .in_arg_in9_0_tpl(in_arg_in9_0_tpl),
        .in_arg_out4_0_tpl(in_arg_out4_0_tpl),
        .in_c0_eni2_0_tpl(GND_q),
        .in_c0_eni2_1_tpl(redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_q),
        .in_c0_eni2_2_tpl(i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q),
        .out_o_stall(i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_o_stall),
        .out_o_valid(i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_o_valid),
        .out_c0_exit_0_tpl(),
        .out_c0_exit_1_tpl(i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_1_tpl),
        .out_c0_exit_2_tpl(i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl),
        .clock(clock),
        .resetn(resetn)
    );

    // bubble_join_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x(BITJOIN,119)
    assign bubble_join_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_q = {i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl, i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_1_tpl};

    // bubble_select_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x(BITSELECT,120)
    assign bubble_select_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_b = bubble_join_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_q[63:0];
    assign bubble_select_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_c = bubble_join_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_q[127:64];

    // i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12(BLACKBOX,16)@8
    // in in_i_stall@20000000
    // out out_o_readdata@78
    // out out_o_stall@20000000
    // out out_o_valid@78
    // out out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address@20000000
    // out out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount@20000000
    // out out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable@20000000
    // out out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable@20000000
    // out out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read@20000000
    // out out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write@20000000
    // out out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata@20000000
    HybridCPU_SignExtend_Kernel_i_llvm_fpga_mem_unname000000r thei_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12 (
        .in_flush(in_flush),
        .in_i_address(bubble_select_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_b),
        .in_i_dependence(bubble_select_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_b),
        .in_i_predicate(GND_q),
        .in_i_stall(SE_out_i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_backStall),
        .in_i_valid(SE_out_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_V0),
        .in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdata(in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdata),
        .in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdatavalid(in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdatavalid),
        .in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_waitrequest(in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_waitrequest),
        .in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writeack(in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writeack),
        .out_o_readdata(i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_o_readdata),
        .out_o_stall(i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_o_stall),
        .out_o_valid(i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_o_valid),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address(i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount(i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable(i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable(i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read(i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write(i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata(i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata),
        .clock(clock),
        .resetn(resetn)
    );

    // SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x(STALLENABLE,159)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_fromReg0 <= '0;
            SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_fromReg1 <= '0;
        end
        else
        begin
            // Successor 0
            SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_fromReg0 <= SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_toReg0;
            // Successor 1
            SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_fromReg1 <= SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_toReg1;
        end
    end
    // Input Stall processing
    assign SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_consumed0 = (~ (SE_out_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_backStall) & SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_wireValid) | SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_fromReg0;
    assign SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_consumed1 = (~ (redist0_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_c0_exit_2_tpl_74_fifo_o_stall) & SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_wireValid) | SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_fromReg1;
    // Consuming
    assign SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_StallValid = SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_backStall & SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_wireValid;
    assign SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_toReg0 = SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_StallValid & SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_consumed0;
    assign SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_toReg1 = SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_StallValid & SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_consumed1;
    // Backward Stall generation
    assign SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_or0 = SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_consumed0;
    assign SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_wireStall = ~ (SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_consumed1 & SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_or0);
    assign SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_backStall = SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_wireStall;
    // Valid signal propagation
    assign SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_V0 = SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_wireValid & ~ (SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_fromReg0);
    assign SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_V1 = SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_wireValid & ~ (SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_fromReg1);
    // Computing multiple Valid(s)
    assign SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_wireValid = i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_o_valid;

    // SE_out_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr(STALLENABLE,142)
    // Valid signal propagation
    assign SE_out_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_V0 = SE_out_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_wireValid;
    // Backward Stall generation
    assign SE_out_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_backStall = i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_o_stall | ~ (SE_out_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_wireValid);
    // Computing multiple Valid(s)
    assign SE_out_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_and0 = i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_out_valid_out;
    assign SE_out_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_wireValid = SE_out_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_V0 & SE_out_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_and0;

    // redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0(REG,83)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_q <= 1'b0;
        end
        else if (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_backEN == 1'b1)
        begin
            redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_q <= redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_q;
        end
    end

    // redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1(REG,84)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_q <= 1'b0;
        end
        else if (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_backEN == 1'b1)
        begin
            redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_q <= redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_q;
        end
    end

    // redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2(REG,85)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_q <= 1'b0;
        end
        else if (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_backEN == 1'b1)
        begin
            redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_q <= redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_1_q;
        end
    end

    // redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3(REG,86)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_q <= 1'b0;
        end
        else if (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_backEN == 1'b1)
        begin
            redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_q <= redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_2_q;
        end
    end

    // redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4(REG,87)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_q <= 1'b0;
        end
        else if (SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_backEN == 1'b1)
        begin
            redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_q <= redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_3_q;
        end
    end

    // i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr(BLACKBOX,18)@7
    // in in_stall_in@20000000
    // out out_data_out@8
    // out out_feedback_stall_out_4@20000000
    // out out_stall_out@20000000
    // out out_valid_out@8
    HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i1_mem000000r thei_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr (
        .in_data_in(GND_q),
        .in_dir(redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_q),
        .in_feedback_in_4(i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17_out_feedback_out_4),
        .in_feedback_valid_in_4(i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17_out_feedback_valid_out_4),
        .in_predicate(GND_q),
        .in_stall_in(SE_out_i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_backStall),
        .in_valid_in(SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_4_V0),
        .out_data_out(i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_out_data_out),
        .out_feedback_stall_out_4(i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_out_feedback_stall_out_4),
        .out_stall_out(i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_out_stall_out),
        .out_valid_out(i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_out_valid_out),
        .clock(clock),
        .resetn(rst_sync_rst_sclrn[0])
    );

    // bubble_join_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16(BITJOIN,97)
    assign bubble_join_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_q = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_o_writeack;

    // bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16(BITSELECT,98)
    assign bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_b = bubble_join_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_q[0:0];

    // i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17(BLACKBOX,20)@91
    // in in_stall_in@20000000
    // out out_data_out@92
    // out out_feedback_out_4@20000000
    // out out_feedback_valid_out_4@20000000
    // out out_stall_out@20000000
    // out out_valid_out@92
    HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_me000000r thei_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17 (
        .in_data_in(bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_b),
        .in_feedback_stall_in_4(i_llvm_fpga_pop_i1_memdep_phi_pop4_hybridcpu_signextend_kernel_77_1gr_out_feedback_stall_out_4),
        .in_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not(redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_q),
        .in_stall_in(SE_out_i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17_backStall),
        .in_valid_in(SE_bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_V0),
        .out_data_out(),
        .out_feedback_out_4(i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17_out_feedback_out_4),
        .out_feedback_valid_out_4(i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17_out_feedback_valid_out_4),
        .out_stall_out(i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17_out_stall_out),
        .out_valid_out(i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17_out_valid_out),
        .clock(clock),
        .resetn(rst_sync_rst_sclrn[0])
    );

    // bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg(STALLFIFO,233)
    assign bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_i_valid = SE_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_V0;
    assign bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_i_stall = SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_backStall;
    assign bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_i_valid_bitsignaltemp = bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_i_valid[0];
    assign bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_i_stall_bitsignaltemp = bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_i_stall[0];
    assign bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_o_valid[0] = bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_o_valid_bitsignaltemp;
    assign bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_o_stall[0] = bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_o_stall_bitsignaltemp;
    hld_fifo_zero_width #(
        .DEPTH(7),
        .STYLE("ll"),
        .ALMOST_FULL_CUTOFF(0),
        .ALMOST_EMPTY_CUTOFF(0),
        .STALL_IN_EARLINESS(0),
        .VALID_IN_EARLINESS(0),
        .USE_STALL_LATENCY_UPSTREAM(0),
        .USE_STALL_LATENCY_DOWNSTREAM(0),
        .NEVER_OVERFLOWS(0),
        .RESET_EXTERNALLY_HELD(1),
        .ASYNC_RESET(0),
        .SYNCHRONIZE_RESET(1)
    ) thebubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg (
        .i_valid(bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_i_valid_bitsignaltemp),
        .i_stall(bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_i_stall_bitsignaltemp),
        .o_valid(bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_o_valid_bitsignaltemp),
        .o_stall(bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_o_stall_bitsignaltemp),
        .clock(clock),
        .resetn(resetn)
    );

    // SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo(STALLENABLE,165)
    // Valid signal propagation
    assign SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_V0 = SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_wireValid;
    // Backward Stall generation
    assign SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_backStall = in_stall_in | ~ (SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_wireValid);
    // Computing multiple Valid(s)
    assign SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_and0 = redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_o_valid;
    assign SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_and1 = bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_o_valid & SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_and0;
    assign SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_wireValid = SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_V0 & SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_and1;

    // SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16(STALLENABLE,136)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_fromReg0 <= '0;
            SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_fromReg1 <= '0;
        end
        else
        begin
            // Successor 0
            SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_fromReg0 <= SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_toReg0;
            // Successor 1
            SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_fromReg1 <= SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_toReg1;
        end
    end
    // Input Stall processing
    assign SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_consumed0 = (~ (SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_backStall) & SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_wireValid) | SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_fromReg0;
    assign SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_consumed1 = (~ (SE_bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_backStall) & SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_wireValid) | SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_fromReg1;
    // Consuming
    assign SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_StallValid = SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_backStall & SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_wireValid;
    assign SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_toReg0 = SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_StallValid & SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_consumed0;
    assign SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_toReg1 = SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_StallValid & SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_consumed1;
    // Backward Stall generation
    assign SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_or0 = SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_consumed0;
    assign SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_wireStall = ~ (SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_consumed1 & SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_or0);
    assign SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_backStall = SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_wireStall;
    // Valid signal propagation
    assign SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_V0 = SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_wireValid & ~ (SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_fromReg0);
    assign SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_V1 = SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_wireValid & ~ (SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_fromReg1);
    // Computing multiple Valid(s)
    assign SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_wireValid = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_o_valid;

    // SE_bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16(STALLENABLE,184)
    // Valid signal propagation
    assign SE_bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_V0 = SE_bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_wireValid;
    // Backward Stall generation
    assign SE_bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_backStall = i_llvm_fpga_push_i1_memdep_phi_push4_hybridcpu_signextend_kernel_77_17_out_stall_out | ~ (SE_bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_wireValid);
    // Computing multiple Valid(s)
    assign SE_bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_and0 = SE_out_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_V1;
    assign SE_bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_wireValid = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_V0 & SE_bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_and0;

    // SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6(STALLENABLE,180)
    // Valid signal propagation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_V0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_R_v_0;
    // Stall signal propagation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_s_tv_0 = SE_bubble_select_i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_backStall & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_R_v_0;
    // Backward Enable generation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_backEN = ~ (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_s_tv_0);
    // Determine whether to write valid data into the first register stage
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_v_s_0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_backEN & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_V0;
    // Backward Stall generation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_backStall = ~ (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_v_s_0);
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_R_v_0 <= 1'b0;
        end
        else
        begin
            if (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_backEN == 1'b0)
            begin
                SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_R_v_0 <= SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_R_v_0 & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_s_tv_0;
            end
            else
            begin
                SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_R_v_0 <= SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_v_s_0;
            end

        end
    end

    // SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5(STALLENABLE,179)
    // Valid signal propagation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_V0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_R_v_0;
    // Stall signal propagation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_s_tv_0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_6_backStall & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_R_v_0;
    // Backward Enable generation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_backEN = ~ (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_s_tv_0);
    // Determine whether to write valid data into the first register stage
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_v_s_0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_backEN & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_V0;
    // Backward Stall generation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_backStall = ~ (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_v_s_0);
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_R_v_0 <= 1'b0;
        end
        else
        begin
            if (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_backEN == 1'b0)
            begin
                SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_R_v_0 <= SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_R_v_0 & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_s_tv_0;
            end
            else
            begin
                SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_R_v_0 <= SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_v_s_0;
            end

        end
    end

    // SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4(STALLENABLE,178)
    // Valid signal propagation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_V0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_R_v_0;
    // Stall signal propagation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_s_tv_0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_5_backStall & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_R_v_0;
    // Backward Enable generation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_backEN = ~ (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_s_tv_0);
    // Determine whether to write valid data into the first register stage
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_v_s_0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_backEN & SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_V;
    // Backward Stall generation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_backStall = ~ (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_backEN);
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_R_v_0 <= 1'b0;
        end
        else
        begin
            if (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_backEN == 1'b0)
            begin
                SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_R_v_0 <= SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_R_v_0 & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_s_tv_0;
            end
            else
            begin
                SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_R_v_0 <= SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_v_s_0;
            end

        end
    end

    // SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4(STALLREG,235)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_r_valid <= 1'b0;
            SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_r_data0 <= 1'bx;
        end
        else
        begin
            // Valid
            SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_r_valid <= SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_backStall & (SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_r_valid | SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_i_valid);

            if (SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_r_valid == 1'b0)
            begin
                // Data(s)
                SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_r_data0 <= $unsigned(redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_q);
            end

        end
    end
    // Computing multiple Valid(s)
    assign SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_i_valid = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_V0;
    // Stall signal propagation
    assign SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_backStall = SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_r_valid | ~ (SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_i_valid);

    // Valid
    assign SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_V = SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_r_valid == 1'b1 ? SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_r_valid : SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_i_valid;

    assign SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_D0 = $signed(SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_r_valid == 1'b1 ? SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_r_data0 : redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_q);

    // SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3(STALLENABLE,177)
    // Valid signal propagation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_V0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_R_v_0;
    // Stall signal propagation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_s_tv_0 = SR_SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_4_backStall & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_R_v_0;
    // Backward Enable generation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_backEN = ~ (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_s_tv_0);
    // Determine whether to write valid data into the first register stage
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_v_s_0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_backEN & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_V0;
    // Backward Stall generation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_backStall = ~ (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_v_s_0);
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_R_v_0 <= 1'b0;
        end
        else
        begin
            if (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_backEN == 1'b0)
            begin
                SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_R_v_0 <= SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_R_v_0 & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_s_tv_0;
            end
            else
            begin
                SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_R_v_0 <= SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_v_s_0;
            end

        end
    end

    // SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2(STALLENABLE,176)
    // Valid signal propagation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_V0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_R_v_0;
    // Stall signal propagation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_s_tv_0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_3_backStall & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_R_v_0;
    // Backward Enable generation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_backEN = ~ (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_s_tv_0);
    // Determine whether to write valid data into the first register stage
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_v_s_0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_backEN & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_V0;
    // Backward Stall generation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_backStall = ~ (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_v_s_0);
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_R_v_0 <= 1'b0;
        end
        else
        begin
            if (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_backEN == 1'b0)
            begin
                SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_R_v_0 <= SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_R_v_0 & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_s_tv_0;
            end
            else
            begin
                SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_R_v_0 <= SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_v_s_0;
            end

        end
    end

    // SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1(STALLENABLE,175)
    // Valid signal propagation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_V0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_R_v_0;
    // Stall signal propagation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_s_tv_0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_2_backStall & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_R_v_0;
    // Backward Enable generation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_backEN = ~ (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_s_tv_0);
    // Determine whether to write valid data into the first register stage
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_v_s_0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_backEN & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_V0;
    // Backward Stall generation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_backStall = ~ (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_v_s_0);
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_R_v_0 <= 1'b0;
        end
        else
        begin
            if (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_backEN == 1'b0)
            begin
                SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_R_v_0 <= SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_R_v_0 & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_s_tv_0;
            end
            else
            begin
                SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_R_v_0 <= SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_v_s_0;
            end

        end
    end

    // SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0(STALLENABLE,174)
    // Valid signal propagation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_V0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_R_v_0;
    // Stall signal propagation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_s_tv_0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_1_backStall & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_R_v_0;
    // Backward Enable generation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_backEN = ~ (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_s_tv_0);
    // Determine whether to write valid data into the first register stage
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_v_s_0 = SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_backEN & SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_V0;
    // Backward Stall generation
    assign SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_backStall = ~ (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_v_s_0);
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_R_v_0 <= 1'b0;
        end
        else
        begin
            if (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_backEN == 1'b0)
            begin
                SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_R_v_0 <= SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_R_v_0 & SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_s_tv_0;
            end
            else
            begin
                SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_R_v_0 <= SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_v_s_0;
            end

        end
    end

    // SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo(STALLENABLE,173)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_fromReg0 <= '0;
            SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_fromReg1 <= '0;
        end
        else
        begin
            // Successor 0
            SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_fromReg0 <= SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_toReg0;
            // Successor 1
            SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_fromReg1 <= SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_toReg1;
        end
    end
    // Input Stall processing
    assign SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_consumed0 = (~ (SE_redist5_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_89_0_backStall) & SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_wireValid) | SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_fromReg0;
    assign SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_consumed1 = (~ (i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr_out_stall_out) & SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_wireValid) | SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_fromReg1;
    // Consuming
    assign SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_StallValid = SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_backStall & SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_wireValid;
    assign SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_toReg0 = SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_StallValid & SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_consumed0;
    assign SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_toReg1 = SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_StallValid & SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_consumed1;
    // Backward Stall generation
    assign SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_or0 = SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_consumed0;
    assign SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_wireStall = ~ (SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_consumed1 & SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_or0);
    assign SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_backStall = SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_wireStall;
    // Valid signal propagation
    assign SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_V0 = SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_wireValid & ~ (SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_fromReg0);
    assign SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_V1 = SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_wireValid & ~ (SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_fromReg1);
    // Computing multiple Valid(s)
    assign SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_wireValid = redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_o_valid;

    // redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo(STALLFIFO,88)
    assign redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_i_valid = SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_V2;
    assign redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_i_stall = SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_backStall;
    assign redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_i_data = i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q;
    assign redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_i_valid_bitsignaltemp = redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_i_valid[0];
    assign redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_i_stall_bitsignaltemp = redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_i_stall[0];
    assign redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_o_valid[0] = redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_o_valid_bitsignaltemp;
    assign redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_o_stall[0] = redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_o_stall_bitsignaltemp;
    hld_fifo #(
        .DEPTH(83),
        .WIDTH(1),
        .STYLE("ms"),
        .ALMOST_FULL_CUTOFF(0),
        .ALMOST_EMPTY_CUTOFF(0),
        .STALL_IN_EARLINESS(0),
        .VALID_IN_EARLINESS(0),
        .USE_STALL_LATENCY_UPSTREAM(0),
        .USE_STALL_LATENCY_DOWNSTREAM(0),
        .NEVER_OVERFLOWS(0),
        .RESET_EXTERNALLY_HELD(1),
        .ASYNC_RESET(0),
        .SYNCHRONIZE_RESET(1)
    ) theredist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo (
        .i_valid(redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_i_valid_bitsignaltemp),
        .i_stall(redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_i_stall_bitsignaltemp),
        .i_data(i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q),
        .o_valid(redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_o_valid_bitsignaltemp),
        .o_stall(redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_o_stall_bitsignaltemp),
        .o_data(redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_o_data),
        .clock(clock),
        .resetn(resetn)
    );

    // redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo(STALLFIFO,81)
    assign redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_i_valid = SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_V1;
    assign redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_i_stall = SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_backStall;
    assign redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_i_data = i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q;
    assign redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_i_valid_bitsignaltemp = redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_i_valid[0];
    assign redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_i_stall_bitsignaltemp = redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_i_stall[0];
    assign redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_o_valid[0] = redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_o_valid_bitsignaltemp;
    assign redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_o_stall[0] = redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_o_stall_bitsignaltemp;
    hld_fifo #(
        .DEPTH(90),
        .WIDTH(1),
        .STYLE("ms"),
        .ALMOST_FULL_CUTOFF(0),
        .ALMOST_EMPTY_CUTOFF(0),
        .STALL_IN_EARLINESS(0),
        .VALID_IN_EARLINESS(0),
        .USE_STALL_LATENCY_UPSTREAM(0),
        .USE_STALL_LATENCY_DOWNSTREAM(0),
        .NEVER_OVERFLOWS(0),
        .RESET_EXTERNALLY_HELD(1),
        .ASYNC_RESET(0),
        .SYNCHRONIZE_RESET(1)
    ) theredist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo (
        .i_valid(redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_i_valid_bitsignaltemp),
        .i_stall(redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_i_stall_bitsignaltemp),
        .i_data(i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q),
        .o_valid(redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_o_valid_bitsignaltemp),
        .o_stall(redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_o_stall_bitsignaltemp),
        .o_data(redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_o_data),
        .clock(clock),
        .resetn(resetn)
    );

    // SE_in_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x(STALLENABLE,158)
    // Valid signal propagation
    assign SE_in_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_V0 = SE_in_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_wireValid;
    // Backward Stall generation
    assign SE_in_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_backStall = i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_out_o_stall | ~ (SE_in_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_wireValid);
    // Computing multiple Valid(s)
    assign SE_in_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_and0 = SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_V0;
    assign SE_in_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_wireValid = SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_V0 & SE_in_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_and0;

    // SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr(STALLENABLE,144)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg0 <= '0;
            SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg1 <= '0;
            SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg2 <= '0;
            SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg3 <= '0;
        end
        else
        begin
            // Successor 0
            SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg0 <= SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_toReg0;
            // Successor 1
            SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg1 <= SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_toReg1;
            // Successor 2
            SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg2 <= SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_toReg2;
            // Successor 3
            SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg3 <= SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_toReg3;
        end
    end
    // Input Stall processing
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_consumed0 = (~ (SE_in_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_backStall) & SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_wireValid) | SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg0;
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_consumed1 = (~ (redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_o_stall) & SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_wireValid) | SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg1;
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_consumed2 = (~ (redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_o_stall) & SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_wireValid) | SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg2;
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_consumed3 = (~ (i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_out_stall_out) & SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_wireValid) | SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg3;
    // Consuming
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_StallValid = SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_backStall & SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_wireValid;
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_toReg0 = SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_StallValid & SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_consumed0;
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_toReg1 = SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_StallValid & SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_consumed1;
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_toReg2 = SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_StallValid & SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_consumed2;
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_toReg3 = SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_StallValid & SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_consumed3;
    // Backward Stall generation
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_or0 = SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_consumed0;
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_or1 = SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_consumed1 & SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_or0;
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_or2 = SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_consumed2 & SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_or1;
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_wireStall = ~ (SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_consumed3 & SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_or2);
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_backStall = SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_wireStall;
    // Valid signal propagation
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_V0 = SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_wireValid & ~ (SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg0);
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_V1 = SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_wireValid & ~ (SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg1);
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_V2 = SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_wireValid & ~ (SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg2);
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_V3 = SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_wireValid & ~ (SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_fromReg3);
    // Computing multiple Valid(s)
    assign SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_wireValid = i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_out_valid_out;

    // SE_out_i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr(STALLENABLE,150)
    // Backward Stall generation
    assign SE_out_i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_backStall = $unsigned(1'b0);
    // Computing multiple Valid(s)
    assign SE_out_i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_wireValid = i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_out_valid_out;

    // c_i4_1_77_23(CONSTANT,7)
    assign c_i4_1_77_23_q = 4'b1111;

    // i_fpga_indvars_iv_next_hybridcpu_signextend_kernel_77_4gr(ADD,13)@2
    assign i_fpga_indvars_iv_next_hybridcpu_signextend_kernel_77_4gr_a = {1'b0, bubble_select_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_b};
    assign i_fpga_indvars_iv_next_hybridcpu_signextend_kernel_77_4gr_b = {1'b0, c_i4_1_77_23_q};
    assign i_fpga_indvars_iv_next_hybridcpu_signextend_kernel_77_4gr_o = $unsigned(i_fpga_indvars_iv_next_hybridcpu_signextend_kernel_77_4gr_a) + $unsigned(i_fpga_indvars_iv_next_hybridcpu_signextend_kernel_77_4gr_b);
    assign i_fpga_indvars_iv_next_hybridcpu_signextend_kernel_77_4gr_q = i_fpga_indvars_iv_next_hybridcpu_signextend_kernel_77_4gr_o[4:0];

    // bgTrunc_i_fpga_indvars_iv_next_hybridcpu_signextend_kernel_77_4gr_sel_x(BITSELECT,57)@2
    assign bgTrunc_i_fpga_indvars_iv_next_hybridcpu_signextend_kernel_77_4gr_sel_x_b = i_fpga_indvars_iv_next_hybridcpu_signextend_kernel_77_4gr_q[3:0];

    // i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr(BLACKBOX,22)@2
    // in in_stall_in@20000000
    // out out_data_out@3
    // out out_feedback_out_2@20000000
    // out out_feedback_valid_out_2@20000000
    // out out_stall_out@20000000
    // out out_valid_out@3
    HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i4_fp000000r thei_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr (
        .in_data_in(bgTrunc_i_fpga_indvars_iv_next_hybridcpu_signextend_kernel_77_4gr_sel_x_b),
        .in_feedback_stall_in_2(i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_out_feedback_stall_out_2),
        .in_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not(i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q),
        .in_stall_in(SE_out_i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_backStall),
        .in_valid_in(SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_V3),
        .out_data_out(),
        .out_feedback_out_2(i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_out_feedback_out_2),
        .out_feedback_valid_out_2(i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_out_feedback_valid_out_2),
        .out_stall_out(i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_out_stall_out),
        .out_valid_out(i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_out_valid_out),
        .clock(clock),
        .resetn(rst_sync_rst_sclrn[0])
    );

    // bubble_join_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x(BITJOIN,115)
    assign bubble_join_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_q = HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl;

    // bubble_select_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x(BITSELECT,116)
    assign bubble_select_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_b = bubble_join_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_q[0:0];

    // c_i4_6_77_20(CONSTANT,8)
    assign c_i4_6_77_20_q = 4'b0110;

    // i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr(BLACKBOX,19)@1
    // in in_stall_in@20000000
    // out out_data_out@2
    // out out_feedback_stall_out_2@20000000
    // out out_stall_out@20000000
    // out out_valid_out@2
    HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i4_fpg000000r thei_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr (
        .in_data_in(c_i4_6_77_20_q),
        .in_dir(bubble_select_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_b),
        .in_feedback_in_2(i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_out_feedback_out_2),
        .in_feedback_valid_in_2(i_llvm_fpga_push_i4_fpga_indvars_iv_push2_hybridcpu_signextend_kernel_77_8gr_out_feedback_valid_out_2),
        .in_predicate(GND_q),
        .in_stall_in(SE_out_i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_backStall),
        .in_valid_in(SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_V2),
        .out_data_out(i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_out_data_out),
        .out_feedback_stall_out_2(i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_out_feedback_stall_out_2),
        .out_stall_out(i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_out_stall_out),
        .out_valid_out(i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_out_valid_out),
        .clock(clock),
        .resetn(rst_sync_rst_sclrn[0])
    );

    // SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0(STALLENABLE,166)
    // Valid signal propagation
    assign SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_V0 = SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_R_v_0;
    assign SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_V1 = SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_R_v_1;
    // Stall signal propagation
    assign SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_s_tv_0 = SE_in_i_sfc_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_77_10_aunroll_x_backStall & SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_R_v_0;
    assign SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_s_tv_1 = SE_redist3_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_6_0_backStall & SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_R_v_1;
    // Backward Enable generation
    assign SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_or0 = SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_s_tv_0;
    assign SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_backEN = ~ (SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_s_tv_1 | SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_or0);
    // Determine whether to write valid data into the first register stage
    assign SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_v_s_0 = SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_backEN & SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_V1;
    // Backward Stall generation
    assign SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_backStall = ~ (SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_v_s_0);
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_R_v_0 <= 1'b0;
            SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_R_v_1 <= 1'b0;
        end
        else
        begin
            if (SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_backEN == 1'b0)
            begin
                SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_R_v_0 <= SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_R_v_0 & SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_s_tv_0;
            end
            else
            begin
                SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_R_v_0 <= SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_v_s_0;
            end

            if (SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_backEN == 1'b0)
            begin
                SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_R_v_1 <= SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_R_v_1 & SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_s_tv_1;
            end
            else
            begin
                SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_R_v_1 <= SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_v_s_0;
            end

        end
    end

    // bubble_join_stall_entry(BITJOIN,111)
    assign bubble_join_stall_entry_q = in_forked;

    // bubble_select_stall_entry(BITSELECT,112)
    assign bubble_select_stall_entry_b = bubble_join_stall_entry_q[0:0];

    // SE_stall_entry(STALLENABLE,151)
    // Valid signal propagation
    assign SE_stall_entry_V0 = SE_stall_entry_wireValid;
    // Backward Stall generation
    assign SE_stall_entry_backStall = HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_stall_out | ~ (SE_stall_entry_wireValid);
    // Computing multiple Valid(s)
    assign SE_stall_entry_wireValid = in_valid_in;

    // HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x(BLACKBOX,56)@0
    // in in_stall_in@20000000
    // out out_stall_out@20000000
    // out out_valid_out@1
    // out out_data_out_0_tpl@1
    HybridCPU_SignExtend_Kernel_B1_merge_reg theHybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x (
        .in_stall_in(SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_backStall),
        .in_valid_in(SE_stall_entry_V0),
        .in_data_in_0_tpl(bubble_select_stall_entry_b),
        .out_stall_out(HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_stall_out),
        .out_valid_out(HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_valid_out),
        .out_data_out_0_tpl(HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl),
        .clock(clock),
        .resetn(rst_sync_rst_sclrn[0])
    );

    // SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x(STALLENABLE,154)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_fromReg0 <= '0;
            SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_fromReg1 <= '0;
            SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_fromReg2 <= '0;
        end
        else
        begin
            // Successor 0
            SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_fromReg0 <= SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_toReg0;
            // Successor 1
            SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_fromReg1 <= SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_toReg1;
            // Successor 2
            SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_fromReg2 <= SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_toReg2;
        end
    end
    // Input Stall processing
    assign SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_consumed0 = (~ (bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_o_stall) & SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_wireValid) | SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_fromReg0;
    assign SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_consumed1 = (~ (SE_redist2_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_data_out_0_tpl_1_0_backStall) & SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_wireValid) | SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_fromReg1;
    assign SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_consumed2 = (~ (i_llvm_fpga_pop_i4_fpga_indvars_iv_pop2_hybridcpu_signextend_kernel_77_2gr_out_stall_out) & SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_wireValid) | SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_fromReg2;
    // Consuming
    assign SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_StallValid = SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_backStall & SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_wireValid;
    assign SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_toReg0 = SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_StallValid & SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_consumed0;
    assign SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_toReg1 = SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_StallValid & SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_consumed1;
    assign SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_toReg2 = SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_StallValid & SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_consumed2;
    // Backward Stall generation
    assign SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_or0 = SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_consumed0;
    assign SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_or1 = SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_consumed1 & SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_or0;
    assign SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_wireStall = ~ (SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_consumed2 & SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_or1);
    assign SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_backStall = SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_wireStall;
    // Valid signal propagation
    assign SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_V0 = SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_wireValid & ~ (SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_fromReg0);
    assign SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_V1 = SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_wireValid & ~ (SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_fromReg1);
    assign SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_V2 = SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_wireValid & ~ (SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_fromReg2);
    // Computing multiple Valid(s)
    assign SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_wireValid = HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_out_valid_out;

    // bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg(STALLFIFO,234)
    assign bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_i_valid = SE_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_V0;
    assign bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_i_stall = SE_out_bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_backStall;
    assign bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_i_valid_bitsignaltemp = bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_i_valid[0];
    assign bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_i_stall_bitsignaltemp = bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_i_stall[0];
    assign bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_o_valid[0] = bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_o_valid_bitsignaltemp;
    assign bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_o_stall[0] = bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_o_stall_bitsignaltemp;
    hld_fifo_zero_width #(
        .DEPTH(84),
        .STYLE("ms"),
        .ALMOST_FULL_CUTOFF(0),
        .ALMOST_EMPTY_CUTOFF(0),
        .STALL_IN_EARLINESS(0),
        .VALID_IN_EARLINESS(0),
        .USE_STALL_LATENCY_UPSTREAM(0),
        .USE_STALL_LATENCY_DOWNSTREAM(0),
        .NEVER_OVERFLOWS(0),
        .RESET_EXTERNALLY_HELD(1),
        .ASYNC_RESET(0),
        .SYNCHRONIZE_RESET(1)
    ) thebubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg (
        .i_valid(bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_i_valid_bitsignaltemp),
        .i_stall(bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_i_stall_bitsignaltemp),
        .o_valid(bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_o_valid_bitsignaltemp),
        .o_stall(bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_o_stall_bitsignaltemp),
        .clock(clock),
        .resetn(resetn)
    );

    // SE_out_bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1(STALLENABLE,212)
    // Valid signal propagation
    assign SE_out_bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_V0 = SE_out_bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_wireValid;
    // Backward Stall generation
    assign SE_out_bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_backStall = i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_stall_out | ~ (SE_out_bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_wireValid);
    // Computing multiple Valid(s)
    assign SE_out_bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_wireValid = bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_reg_o_valid;

    // SE_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr(STALLENABLE,140)
    // Valid signal propagation
    assign SE_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_V0 = SE_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_wireValid;
    // Backward Stall generation
    assign SE_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_backStall = bubble_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_1_reg_o_stall | ~ (SE_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_wireValid);
    // Computing multiple Valid(s)
    assign SE_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_wireValid = i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_valid_out;

    // SE_out_i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr(STALLENABLE,148)
    // Backward Stall generation
    assign SE_out_i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr_backStall = $unsigned(1'b0);
    // Computing multiple Valid(s)
    assign SE_out_i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr_wireValid = i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr_out_valid_out;

    // i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr(BLACKBOX,21)@84
    // in in_stall_in@20000000
    // out out_data_out@85
    // out out_feedback_out_1@20000000
    // out out_feedback_valid_out_1@20000000
    // out out_stall_out@20000000
    // out out_valid_out@85
    HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_no000000r thei_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr (
        .in_data_in(bubble_select_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_b),
        .in_feedback_stall_in_1(i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_not_exitcond_stall_out),
        .in_stall_in(SE_out_i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr_backStall),
        .in_valid_in(SE_out_redist4_i_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_not_hybridcpu_signextend_kernel_77_6gr_q_82_fifo_V1),
        .out_data_out(),
        .out_feedback_out_1(i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr_out_feedback_out_1),
        .out_feedback_valid_out_1(i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr_out_feedback_valid_out_1),
        .out_stall_out(i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr_out_stall_out),
        .out_valid_out(i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr_out_valid_out),
        .clock(clock),
        .resetn(rst_sync_rst_sclrn[0])
    );

    // GND(CONSTANT,0)
    assign GND_q = 1'b0;

    // VCC(CONSTANT,1)
    assign VCC_q = 1'b1;

    // i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr(BLACKBOX,17)@84
    // in in_stall_in@20000000
    // out out_data_out@85
    // out out_exiting_stall_out@20000000
    // out out_exiting_valid_out@20000000
    // out out_initeration_stall_out@20000000
    // out out_not_exitcond_stall_out@20000000
    // out out_pipeline_valid_out@20000000
    // out out_stall_out@20000000
    // out out_valid_out@85
    HybridCPU_SignExtend_Kernel_i_llvm_fpga_pipeline_k000000r thei_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr (
        .in_data_in(VCC_q),
        .in_initeration_in(GND_q),
        .in_initeration_valid_in(GND_q),
        .in_not_exitcond_in(i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr_out_feedback_out_1),
        .in_not_exitcond_valid_in(i_llvm_fpga_push_i1_notexitcond_hybridcpu_signextend_kernel_77_7gr_out_feedback_valid_out_1),
        .in_pipeline_stall_in(in_pipeline_stall_in),
        .in_stall_in(SE_out_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_backStall),
        .in_valid_in(SE_out_bubble_out_HybridCPU_SignExtend_Kernel_B1_merge_reg_aunroll_x_1_V0),
        .out_data_out(),
        .out_exiting_stall_out(i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_exiting_stall_out),
        .out_exiting_valid_out(i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_exiting_valid_out),
        .out_initeration_stall_out(),
        .out_not_exitcond_stall_out(i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_not_exitcond_stall_out),
        .out_pipeline_valid_out(i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_pipeline_valid_out),
        .out_stall_out(i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_stall_out),
        .out_valid_out(i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_valid_out),
        .clock(clock),
        .resetn(rst_sync_rst_sclrn[0])
    );

    // ext_sig_sync_out(GPOUT,12)
    assign out_aclp_to_limiter_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_exiting_valid_out = i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_exiting_valid_out;
    assign out_aclp_to_limiter_i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_exiting_stall_out = i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_exiting_stall_out;

    // pipeline_valid_out_sync(GPOUT,46)
    assign out_pipeline_valid_out = i_llvm_fpga_pipeline_keep_going_hybridcpu_signextend_kernel_77_0gr_out_pipeline_valid_out;

    // sync_out_65(GPOUT,53)@0
    assign out_stall_out = SE_stall_entry_backStall;

    // bubble_join_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo(BITJOIN,128)
    assign bubble_join_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_q = redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_o_data;

    // bubble_select_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo(BITSELECT,129)
    assign bubble_select_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_b = bubble_join_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_q[0:0];

    // sync_out_66(GPOUT,54)@91
    assign out_fpga_indvars_iv_pop2_struct_0_shuffle_bit3 = bubble_select_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_b;
    assign out_valid_out = SE_out_redist1_i_llvm_fpga_bit_shuffle_i1_s_s_in_for_body_i_hybridcpu_signextend_kernel_fpgaunique_1s_v1i32_fpga_indvars_iv_pop2_struct_0_shuffle_bit3_hybridcpu_signextend_kernel_148_0gr_NO_NAME_x_q_89_fifo_V0;

    // dupName_0_ext_sig_sync_out_x(GPOUT,59)
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address = i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address;
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable = i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable;
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read = i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read;
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write = i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write;
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata = i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata;
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable = i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable;
    assign out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount = i_llvm_fpga_mem_unnamed_hybridcpu_signextend_kernel1_hybridcpu_signextend_kernel_77_12_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount;

    // dupName_1_ext_sig_sync_out_x(GPOUT,60)
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_address = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_address;
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_enable = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_enable;
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_read = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_read;
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_write = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_write;
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata;
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable;
    assign out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount;

    // dupName_2_ext_sig_sync_out_x(GPOUT,61)
    assign out_lsu_memdep_o_active = i_llvm_fpga_mem_memdep_hybridcpu_signextend_kernel_77_16_out_lsu_memdep_o_active;

    // rst_sync(RESETSYNC,236)
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
