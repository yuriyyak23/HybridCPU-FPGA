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

// SystemVerilog created from i_sfc_logic_s_c0_in_for_body_i_hybridcpu_signextend_kernels_c0_enter1_hybridcpu_signextend_kernel_196_0gr
// Created for function/kernel HybridCPU_SignExtend_Kernel
// SystemVerilog created on Fri Nov  6 06:17:16 2026


(* altera_attribute = "-name AUTO_SHIFT_REGISTER_RECOGNITION OFF; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 10037; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 15400; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 12020; -name MESSAGE_DISABLE 12030; -name MESSAGE_DISABLE 12010; -name MESSAGE_DISABLE 12110; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 13410; -name MESSAGE_DISABLE 113007; -name MESSAGE_DISABLE 10958" *)
module HybridCPU_SignExtend_Kernel_i_sfc_logic_s_c0_in_fo000000r (
    input wire [63:0] in_arg_in,
    input wire [63:0] in_arg_out,
    input wire [63:0] in_arg_in9_0_tpl,
    input wire [63:0] in_arg_out4_0_tpl,
    output wire [0:0] out_c0_exi2_0_tpl,
    output wire [63:0] out_c0_exi2_1_tpl,
    output wire [63:0] out_c0_exi2_2_tpl,
    output wire [0:0] out_o_valid,
    output wire [0:0] out_unnamed_HybridCPU_SignExtend_Kernel0,
    input wire [0:0] in_c0_eni2_0_tpl,
    input wire [0:0] in_c0_eni2_1_tpl,
    input wire [0:0] in_c0_eni2_2_tpl,
    input wire [0:0] in_i_valid,
    input wire clock,
    input wire resetn
    );

    wire [0:0] GND_q;
    wire [0:0] VCC_q;
    wire [4:0] c_i5_0_199_26_q;
    wire [58:0] i_conv_i_hybridcpu_signextend_kernel_199_4gr_vt_const_63_q;
    wire [63:0] i_conv_i_hybridcpu_signextend_kernel_199_4gr_vt_join_q;
    wire [4:0] i_conv_i_hybridcpu_signextend_kernel_199_4gr_vt_select_4_b;
    wire [25:0] i_inc_i_zext_hybridcpu_signextend_kernel_199_16_vt_const_31_q;
    wire [31:0] i_inc_i_zext_hybridcpu_signextend_kernel_199_16_vt_join_q;
    wire [5:0] i_inc_i_zext_hybridcpu_signextend_kernel_199_16_vt_select_5_b;
    wire [5:0] i_llvm_fpga_int_add_i6_i5_i1_inc_i_i_hybridcpu_signextend_kernel_199_15_a;
    wire [5:0] i_llvm_fpga_int_add_i6_i5_i1_inc_i_i_hybridcpu_signextend_kernel_199_15_b;
    logic [5:0] i_llvm_fpga_int_add_i6_i5_i1_inc_i_i_hybridcpu_signextend_kernel_199_15_o;
    wire [5:0] i_llvm_fpga_int_add_i6_i5_i1_inc_i_i_hybridcpu_signextend_kernel_199_15_q;
    wire [4:0] i_llvm_fpga_pop_i5_i_0_i109_narrowing_pop3_hybridcpu_signextend_kernel_199_2gr_out_data_out;
    wire [0:0] i_llvm_fpga_pop_i5_i_0_i109_narrowing_pop3_hybridcpu_signextend_kernel_199_2gr_out_feedback_stall_out_3;
    wire [7:0] i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_199_18_out_feedback_out_3;
    wire [0:0] i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_199_18_out_feedback_valid_out_3;
    wire [63:0] i_llvm_fpga_sync_buffer_p1024_arg_in_sync_buffer_hybridcpu_signextend_kernel_199_7gr_out_buffer_out;
    wire [63:0] i_llvm_fpga_sync_buffer_p1024_arg_out_sync_buffer_hybridcpu_signextend_kernel_199_12_out_buffer_out;
    wire [26:0] i_unnamed_hybridcpu_signextend_kernel_199_3gr_vt_const_31_q;
    wire [31:0] i_unnamed_hybridcpu_signextend_kernel_199_3gr_vt_join_q;
    wire [4:0] i_unnamed_hybridcpu_signextend_kernel_199_3gr_vt_select_4_b;
    wire [1:0] i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_c_i2_0_245_1gr_x_q;
    wire [61:0] i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_narrow_x_b;
    wire [63:0] i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_shift_join_x_q;
    wire [63:0] i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_dupName_0_trunc_sel_x_b;
    wire [2:0] i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_c_i3_0_293_1gr_x_q;
    wire [60:0] i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_narrow_x_b;
    wire [63:0] i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_shift_join_x_q;
    wire [63:0] i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_dupName_0_trunc_sel_x_b;
    wire [60:0] i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_narrow_x_b;
    wire [63:0] i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_shift_join_x_q;
    wire [63:0] i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_dupName_0_trunc_sel_x_b;
    wire [61:0] i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_narrow_x_b;
    wire [63:0] i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_shift_join_x_q;
    wire [63:0] i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_dupName_0_trunc_sel_x_b;
    wire [63:0] i_conv_i_hybridcpu_signextend_kernel_199_4gr_sel_x_b;
    wire [31:0] i_inc_i_zext_hybridcpu_signextend_kernel_199_16_sel_x_b;
    wire [63:0] i_llvm_fpga_sync_buffer_s_class_sycl_v1_ranges_arg_in9_sync_buffer_hybridcpu_signextend_kernel_199_5gr_aunroll_x_out_buffer_out_0_tpl;
    wire [63:0] i_llvm_fpga_sync_buffer_s_class_sycl_v1_ranges_arg_out4_sync_buffer_hybridcpu_signextend_kernel_199_10_aunroll_x_out_buffer_out_0_tpl;
    wire [4:0] i_unnamed_hybridcpu_signextend_kernel_199_17_sel_x_b;
    wire [31:0] i_unnamed_hybridcpu_signextend_kernel_199_3gr_sel_x_b;
    (* preserve_syn_only *) reg [0:0] valid_fanout_reg0_q;
    (* preserve_syn_only *) reg [0:0] valid_fanout_reg1_q;
    (* preserve_syn_only *) reg [0:0] valid_fanout_reg2_q;
    (* preserve_syn_only *) reg [0:0] valid_fanout_reg3_q;
    (* preserve_syn_only *) reg [0:0] valid_fanout_reg4_q;
    (* preserve_syn_only *) reg [0:0] valid_fanout_reg5_q;
    (* preserve_syn_only *) reg [0:0] valid_fanout_reg6_q;
    wire [61:0] i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_lhsMSBs_select_b;
    wire [62:0] i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_MSBs_sums_a;
    wire [62:0] i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_MSBs_sums_b;
    logic [62:0] i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_MSBs_sums_o;
    wire [62:0] i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_MSBs_sums_q;
    wire [64:0] i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_split_join_q;
    wire [60:0] i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_lhsMSBs_select_b;
    wire [61:0] i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_MSBs_sums_a;
    wire [61:0] i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_MSBs_sums_b;
    logic [61:0] i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_MSBs_sums_o;
    wire [61:0] i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_MSBs_sums_q;
    wire [64:0] i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_split_join_q;
    wire [60:0] i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_lhsMSBs_select_b;
    wire [61:0] i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_MSBs_sums_a;
    wire [61:0] i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_MSBs_sums_b;
    logic [61:0] i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_MSBs_sums_o;
    wire [61:0] i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_MSBs_sums_q;
    wire [64:0] i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_split_join_q;
    wire [61:0] i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_lhsMSBs_select_b;
    wire [62:0] i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_MSBs_sums_a;
    wire [62:0] i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_MSBs_sums_b;
    logic [62:0] i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_MSBs_sums_o;
    wire [62:0] i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_MSBs_sums_q;
    wire [64:0] i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_split_join_q;
    wire [61:0] i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_rhsMSBs_select_bit_select_merged_b;
    wire [1:0] i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_rhsMSBs_select_bit_select_merged_c;
    wire [60:0] i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_rhsMSBs_select_bit_select_merged_b;
    wire [2:0] i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_rhsMSBs_select_bit_select_merged_c;
    wire [61:0] i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_rhsMSBs_select_bit_select_merged_b;
    wire [1:0] i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_rhsMSBs_select_bit_select_merged_c;
    wire [60:0] i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_rhsMSBs_select_bit_select_merged_b;
    wire [2:0] i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_rhsMSBs_select_bit_select_merged_c;
    reg [0:0] redist0_sync_together_199_35_aunroll_x_in_c0_eni2_1_tpl_2_q;
    reg [0:0] redist0_sync_together_199_35_aunroll_x_in_c0_eni2_1_tpl_2_delay_0;
    reg [0:0] redist1_sync_together_199_35_aunroll_x_in_c0_eni2_2_tpl_2_q;
    reg [0:0] redist1_sync_together_199_35_aunroll_x_in_c0_eni2_2_tpl_2_delay_0;
    reg [0:0] redist2_sync_together_199_35_aunroll_x_in_i_valid_1_q;
    reg [63:0] redist3_i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_dupName_0_trunc_sel_x_b_1_q;
    reg [63:0] redist4_i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_dupName_0_trunc_sel_x_b_1_q;


    // VCC(CONSTANT,1)
    assign VCC_q = 1'b1;

    // redist2_sync_together_199_35_aunroll_x_in_i_valid_1(DELAY,102)
    always_ff @ (posedge clock)
    begin
        if (!resetn)
        begin
            redist2_sync_together_199_35_aunroll_x_in_i_valid_1_q <= '0;
        end
        else
        begin
            redist2_sync_together_199_35_aunroll_x_in_i_valid_1_q <= in_i_valid;
        end
    end

    // valid_fanout_reg0(REG,65)@3 + 1
    always_ff @ (posedge clock)
    begin
        if (0)
        begin
        end
        else
        begin
            valid_fanout_reg0_q <= redist2_sync_together_199_35_aunroll_x_in_i_valid_1_q;
        end
    end

    // i_conv_i_hybridcpu_signextend_kernel_199_4gr_vt_const_63(CONSTANT,11)
    assign i_conv_i_hybridcpu_signextend_kernel_199_4gr_vt_const_63_q = 59'b00000000000000000000000000000000000000000000000000000000000;

    // i_unnamed_hybridcpu_signextend_kernel_199_3gr_vt_const_31(CONSTANT,25)
    assign i_unnamed_hybridcpu_signextend_kernel_199_3gr_vt_const_31_q = 27'b000000000000000000000000000;

    // valid_fanout_reg1(REG,66)@3 + 1
    always_ff @ (posedge clock)
    begin
        if (0)
        begin
        end
        else
        begin
            valid_fanout_reg1_q <= redist2_sync_together_199_35_aunroll_x_in_i_valid_1_q;
        end
    end

    // valid_fanout_reg6(REG,71)@3 + 1
    always_ff @ (posedge clock)
    begin
        if (0)
        begin
        end
        else
        begin
            valid_fanout_reg6_q <= redist2_sync_together_199_35_aunroll_x_in_i_valid_1_q;
        end
    end

    // i_inc_i_zext_hybridcpu_signextend_kernel_199_16_vt_const_31(CONSTANT,15)
    assign i_inc_i_zext_hybridcpu_signextend_kernel_199_16_vt_const_31_q = 26'b00000000000000000000000000;

    // i_llvm_fpga_int_add_i6_i5_i1_inc_i_i_hybridcpu_signextend_kernel_199_15(ADD,18)@4
    assign i_llvm_fpga_int_add_i6_i5_i1_inc_i_i_hybridcpu_signextend_kernel_199_15_a = {1'b0, i_llvm_fpga_pop_i5_i_0_i109_narrowing_pop3_hybridcpu_signextend_kernel_199_2gr_out_data_out};
    assign i_llvm_fpga_int_add_i6_i5_i1_inc_i_i_hybridcpu_signextend_kernel_199_15_b = {5'b00000, VCC_q};
    assign i_llvm_fpga_int_add_i6_i5_i1_inc_i_i_hybridcpu_signextend_kernel_199_15_o = $unsigned(i_llvm_fpga_int_add_i6_i5_i1_inc_i_i_hybridcpu_signextend_kernel_199_15_a) + $unsigned(i_llvm_fpga_int_add_i6_i5_i1_inc_i_i_hybridcpu_signextend_kernel_199_15_b);
    assign i_llvm_fpga_int_add_i6_i5_i1_inc_i_i_hybridcpu_signextend_kernel_199_15_q = i_llvm_fpga_int_add_i6_i5_i1_inc_i_i_hybridcpu_signextend_kernel_199_15_o[5:0];

    // i_inc_i_zext_hybridcpu_signextend_kernel_199_16_sel_x(BITSELECT,56)@4
    assign i_inc_i_zext_hybridcpu_signextend_kernel_199_16_sel_x_b = {26'b00000000000000000000000000, i_llvm_fpga_int_add_i6_i5_i1_inc_i_i_hybridcpu_signextend_kernel_199_15_q[5:0]};

    // i_inc_i_zext_hybridcpu_signextend_kernel_199_16_vt_select_5(BITSELECT,17)@4
    assign i_inc_i_zext_hybridcpu_signextend_kernel_199_16_vt_select_5_b = i_inc_i_zext_hybridcpu_signextend_kernel_199_16_sel_x_b[5:0];

    // i_inc_i_zext_hybridcpu_signextend_kernel_199_16_vt_join(BITJOIN,16)@4
    assign i_inc_i_zext_hybridcpu_signextend_kernel_199_16_vt_join_q = {i_inc_i_zext_hybridcpu_signextend_kernel_199_16_vt_const_31_q, i_inc_i_zext_hybridcpu_signextend_kernel_199_16_vt_select_5_b};

    // i_unnamed_hybridcpu_signextend_kernel_199_17_sel_x(BITSELECT,59)@4
    assign i_unnamed_hybridcpu_signextend_kernel_199_17_sel_x_b = i_inc_i_zext_hybridcpu_signextend_kernel_199_16_vt_join_q[4:0];

    // redist1_sync_together_199_35_aunroll_x_in_c0_eni2_2_tpl_2(DELAY,101)
    always_ff @ (posedge clock)
    begin
        if (0)
        begin
        end
        else
        begin
            redist1_sync_together_199_35_aunroll_x_in_c0_eni2_2_tpl_2_delay_0 <= $unsigned(in_c0_eni2_2_tpl);
            redist1_sync_together_199_35_aunroll_x_in_c0_eni2_2_tpl_2_q <= $signed(redist1_sync_together_199_35_aunroll_x_in_c0_eni2_2_tpl_2_delay_0);
        end
    end

    // i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_199_18(BLACKBOX,20)@4
    // out out_feedback_out_3@20000000
    // out out_feedback_valid_out_3@20000000
    HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i5_i_000000r thei_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_199_18 (
        .in_c0_ene2(redist1_sync_together_199_35_aunroll_x_in_c0_eni2_2_tpl_2_q),
        .in_data_in(i_unnamed_hybridcpu_signextend_kernel_199_17_sel_x_b),
        .in_feedback_stall_in_3(i_llvm_fpga_pop_i5_i_0_i109_narrowing_pop3_hybridcpu_signextend_kernel_199_2gr_out_feedback_stall_out_3),
        .in_stall_in(GND_q),
        .in_valid_in(valid_fanout_reg6_q),
        .out_data_out(),
        .out_feedback_out_3(i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_199_18_out_feedback_out_3),
        .out_feedback_valid_out_3(i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_199_18_out_feedback_valid_out_3),
        .out_stall_out(),
        .out_valid_out(),
        .clock(clock),
        .resetn(resetn)
    );

    // redist0_sync_together_199_35_aunroll_x_in_c0_eni2_1_tpl_2(DELAY,100)
    always_ff @ (posedge clock)
    begin
        if (0)
        begin
        end
        else
        begin
            redist0_sync_together_199_35_aunroll_x_in_c0_eni2_1_tpl_2_delay_0 <= $unsigned(in_c0_eni2_1_tpl);
            redist0_sync_together_199_35_aunroll_x_in_c0_eni2_1_tpl_2_q <= $signed(redist0_sync_together_199_35_aunroll_x_in_c0_eni2_1_tpl_2_delay_0);
        end
    end

    // c_i5_0_199_26(CONSTANT,9)
    assign c_i5_0_199_26_q = 5'b00000;

    // i_llvm_fpga_pop_i5_i_0_i109_narrowing_pop3_hybridcpu_signextend_kernel_199_2gr(BLACKBOX,19)@4
    // out out_feedback_stall_out_3@20000000
    HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i5_i_0000000r thei_llvm_fpga_pop_i5_i_0_i109_narrowing_pop3_hybridcpu_signextend_kernel_199_2gr (
        .in_data_in(c_i5_0_199_26_q),
        .in_dir(redist0_sync_together_199_35_aunroll_x_in_c0_eni2_1_tpl_2_q),
        .in_feedback_in_3(i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_199_18_out_feedback_out_3),
        .in_feedback_valid_in_3(i_llvm_fpga_push_i5_i_0_i109_narrowing_push3_hybridcpu_signextend_kernel_199_18_out_feedback_valid_out_3),
        .in_predicate(GND_q),
        .in_stall_in(GND_q),
        .in_valid_in(valid_fanout_reg1_q),
        .out_data_out(i_llvm_fpga_pop_i5_i_0_i109_narrowing_pop3_hybridcpu_signextend_kernel_199_2gr_out_data_out),
        .out_feedback_stall_out_3(i_llvm_fpga_pop_i5_i_0_i109_narrowing_pop3_hybridcpu_signextend_kernel_199_2gr_out_feedback_stall_out_3),
        .out_stall_out(),
        .out_valid_out(),
        .clock(clock),
        .resetn(resetn)
    );

    // i_unnamed_hybridcpu_signextend_kernel_199_3gr_sel_x(BITSELECT,60)@4
    assign i_unnamed_hybridcpu_signextend_kernel_199_3gr_sel_x_b = {27'b000000000000000000000000000, i_llvm_fpga_pop_i5_i_0_i109_narrowing_pop3_hybridcpu_signextend_kernel_199_2gr_out_data_out[4:0]};

    // i_unnamed_hybridcpu_signextend_kernel_199_3gr_vt_select_4(BITSELECT,27)@4
    assign i_unnamed_hybridcpu_signextend_kernel_199_3gr_vt_select_4_b = i_unnamed_hybridcpu_signextend_kernel_199_3gr_sel_x_b[4:0];

    // i_unnamed_hybridcpu_signextend_kernel_199_3gr_vt_join(BITJOIN,26)@4
    assign i_unnamed_hybridcpu_signextend_kernel_199_3gr_vt_join_q = {i_unnamed_hybridcpu_signextend_kernel_199_3gr_vt_const_31_q, i_unnamed_hybridcpu_signextend_kernel_199_3gr_vt_select_4_b};

    // i_conv_i_hybridcpu_signextend_kernel_199_4gr_sel_x(BITSELECT,55)@4
    assign i_conv_i_hybridcpu_signextend_kernel_199_4gr_sel_x_b = {32'b00000000000000000000000000000000, i_unnamed_hybridcpu_signextend_kernel_199_3gr_vt_join_q[31:0]};

    // i_conv_i_hybridcpu_signextend_kernel_199_4gr_vt_select_4(BITSELECT,13)@4
    assign i_conv_i_hybridcpu_signextend_kernel_199_4gr_vt_select_4_b = i_conv_i_hybridcpu_signextend_kernel_199_4gr_sel_x_b[4:0];

    // i_conv_i_hybridcpu_signextend_kernel_199_4gr_vt_join(BITJOIN,12)@4
    assign i_conv_i_hybridcpu_signextend_kernel_199_4gr_vt_join_q = {i_conv_i_hybridcpu_signextend_kernel_199_4gr_vt_const_63_q, i_conv_i_hybridcpu_signextend_kernel_199_4gr_vt_select_4_b};

    // i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_narrow_x(BITSELECT,45)@4
    assign i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_narrow_x_b = i_conv_i_hybridcpu_signextend_kernel_199_4gr_vt_join_q[60:0];

    // i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_c_i3_0_293_1gr_x(CONSTANT,38)
    assign i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_c_i3_0_293_1gr_x_q = 3'b000;

    // i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_shift_join_x(BITJOIN,46)@4
    assign i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_shift_join_x_q = {i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_narrow_x_b, i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_c_i3_0_293_1gr_x_q};

    // i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_lhsMSBs_select(BITSELECT,88)@4
    assign i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_lhsMSBs_select_b = $signed(i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_shift_join_x_q[63:3]);

    // i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_MSBs_sums(ADD,89)@4
    assign i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_MSBs_sums_a = {1'b0, i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_lhsMSBs_select_b};
    assign i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_MSBs_sums_b = {1'b0, i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_rhsMSBs_select_bit_select_merged_b};
    assign i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_MSBs_sums_o = $unsigned(i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_MSBs_sums_a) + $unsigned(i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_MSBs_sums_b);
    assign i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_MSBs_sums_q = $signed(i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_MSBs_sums_o[61:0]);

    // valid_fanout_reg4(REG,69)@2 + 1
    always_ff @ (posedge clock)
    begin
        if (0)
        begin
        end
        else
        begin
            valid_fanout_reg4_q <= in_i_valid;
        end
    end

    // i_llvm_fpga_sync_buffer_s_class_sycl_v1_ranges_arg_out4_sync_buffer_hybridcpu_signextend_kernel_199_10_aunroll_x(BLACKBOX,58)@0
    // in in_i_dependence@3
    // in in_valid_in@3
    // out out_valid_out@3
    // out out_buffer_out_0_tpl@3
    HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000003r thei_llvm_fpga_sync_buffer_s_class_sycl_v1_ranges_arg_out4_sync_buffer_hybridcpu_signextend_kernel_199_10_aunroll_x (
        .in_i_dependence(GND_q),
        .in_stall_in(GND_q),
        .in_valid_in(valid_fanout_reg4_q),
        .in_buffer_in_0_tpl(in_arg_out4_0_tpl),
        .out_stall_out(),
        .out_valid_out(),
        .out_buffer_out_0_tpl(i_llvm_fpga_sync_buffer_s_class_sycl_v1_ranges_arg_out4_sync_buffer_hybridcpu_signextend_kernel_199_10_aunroll_x_out_buffer_out_0_tpl),
        .clock(clock),
        .resetn(resetn)
    );

    // i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_narrow_x(BITSELECT,39)@3
    assign i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_narrow_x_b = i_llvm_fpga_sync_buffer_s_class_sycl_v1_ranges_arg_out4_sync_buffer_hybridcpu_signextend_kernel_199_10_aunroll_x_out_buffer_out_0_tpl[60:0];

    // i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_shift_join_x(BITJOIN,40)@3
    assign i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_shift_join_x_q = {i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_narrow_x_b, i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_c_i3_0_293_1gr_x_q};

    // i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_lhsMSBs_select(BITSELECT,83)@3
    assign i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_lhsMSBs_select_b = $signed(i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_shift_join_x_q[63:3]);

    // i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_MSBs_sums(ADD,84)@3
    assign i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_MSBs_sums_a = {1'b0, i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_lhsMSBs_select_b};
    assign i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_MSBs_sums_b = {1'b0, i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_rhsMSBs_select_bit_select_merged_b};
    assign i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_MSBs_sums_o = $unsigned(i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_MSBs_sums_a) + $unsigned(i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_MSBs_sums_b);
    assign i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_MSBs_sums_q = $signed(i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_MSBs_sums_o[61:0]);

    // valid_fanout_reg5(REG,70)@2 + 1
    always_ff @ (posedge clock)
    begin
        if (0)
        begin
        end
        else
        begin
            valid_fanout_reg5_q <= in_i_valid;
        end
    end

    // i_llvm_fpga_sync_buffer_p1024_arg_out_sync_buffer_hybridcpu_signextend_kernel_199_12(BLACKBOX,22)@0
    // in in_i_dependence@3
    // in in_valid_in@3
    // out out_buffer_out@3
    // out out_valid_out@3
    HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000001r thei_llvm_fpga_sync_buffer_p1024_arg_out_sync_buffer_hybridcpu_signextend_kernel_199_12 (
        .in_buffer_in(in_arg_out),
        .in_i_dependence(GND_q),
        .in_stall_in(GND_q),
        .in_valid_in(valid_fanout_reg5_q),
        .out_buffer_out(i_llvm_fpga_sync_buffer_p1024_arg_out_sync_buffer_hybridcpu_signextend_kernel_199_12_out_buffer_out),
        .out_stall_out(),
        .out_valid_out(),
        .clock(clock),
        .resetn(resetn)
    );

    // i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_rhsMSBs_select_bit_select_merged(BITSELECT,97)@3
    assign i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_rhsMSBs_select_bit_select_merged_b = $signed(i_llvm_fpga_sync_buffer_p1024_arg_out_sync_buffer_hybridcpu_signextend_kernel_199_12_out_buffer_out[63:3]);
    assign i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_rhsMSBs_select_bit_select_merged_c = $signed(i_llvm_fpga_sync_buffer_p1024_arg_out_sync_buffer_hybridcpu_signextend_kernel_199_12_out_buffer_out[2:0]);

    // i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_split_join(BITJOIN,85)@3
    assign i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_split_join_q = {i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_MSBs_sums_q, i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_rhsMSBs_select_bit_select_merged_c};

    // i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_dupName_0_trunc_sel_x(BITSELECT,42)@3
    assign i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_dupName_0_trunc_sel_x_b = i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_add_x_split_join_q[63:0];

    // redist3_i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_dupName_0_trunc_sel_x_b_1(DELAY,103)
    always_ff @ (posedge clock)
    begin
        if (0)
        begin
        end
        else
        begin
            redist3_i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_dupName_0_trunc_sel_x_b_1_q <= i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_dupName_0_trunc_sel_x_b;
        end
    end

    // i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_rhsMSBs_select_bit_select_merged(BITSELECT,99)@4
    assign i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_rhsMSBs_select_bit_select_merged_b = $signed(redist3_i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_dupName_0_trunc_sel_x_b_1_q[63:3]);
    assign i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_rhsMSBs_select_bit_select_merged_c = $signed(redist3_i_add_ptr_i_hybridcpu_signextend_kernel_293_0gr_dupName_0_trunc_sel_x_b_1_q[2:0]);

    // i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_split_join(BITJOIN,90)@4
    assign i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_split_join_q = {i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_MSBs_sums_q, i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_rhsMSBs_select_bit_select_merged_c};

    // i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_dupName_0_trunc_sel_x(BITSELECT,48)@4
    assign i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_dupName_0_trunc_sel_x_b = i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_add_x_split_join_q[63:0];

    // i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_narrow_x(BITSELECT,51)@4
    assign i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_narrow_x_b = i_conv_i_hybridcpu_signextend_kernel_199_4gr_vt_join_q[61:0];

    // i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_c_i2_0_245_1gr_x(CONSTANT,32)
    assign i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_c_i2_0_245_1gr_x_q = 2'b00;

    // i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_shift_join_x(BITJOIN,52)@4
    assign i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_shift_join_x_q = {i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_narrow_x_b, i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_c_i2_0_245_1gr_x_q};

    // i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_lhsMSBs_select(BITSELECT,93)@4
    assign i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_lhsMSBs_select_b = $signed(i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_shift_join_x_q[63:2]);

    // i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_MSBs_sums(ADD,94)@4
    assign i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_MSBs_sums_a = {1'b0, i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_lhsMSBs_select_b};
    assign i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_MSBs_sums_b = {1'b0, i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_rhsMSBs_select_bit_select_merged_b};
    assign i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_MSBs_sums_o = $unsigned(i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_MSBs_sums_a) + $unsigned(i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_MSBs_sums_b);
    assign i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_MSBs_sums_q = $signed(i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_MSBs_sums_o[62:0]);

    // valid_fanout_reg2(REG,67)@2 + 1
    always_ff @ (posedge clock)
    begin
        if (0)
        begin
        end
        else
        begin
            valid_fanout_reg2_q <= in_i_valid;
        end
    end

    // i_llvm_fpga_sync_buffer_s_class_sycl_v1_ranges_arg_in9_sync_buffer_hybridcpu_signextend_kernel_199_5gr_aunroll_x(BLACKBOX,57)@0
    // in in_i_dependence@3
    // in in_valid_in@3
    // out out_valid_out@3
    // out out_buffer_out_0_tpl@3
    HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000002r thei_llvm_fpga_sync_buffer_s_class_sycl_v1_ranges_arg_in9_sync_buffer_hybridcpu_signextend_kernel_199_5gr_aunroll_x (
        .in_i_dependence(GND_q),
        .in_stall_in(GND_q),
        .in_valid_in(valid_fanout_reg2_q),
        .in_buffer_in_0_tpl(in_arg_in9_0_tpl),
        .out_stall_out(),
        .out_valid_out(),
        .out_buffer_out_0_tpl(i_llvm_fpga_sync_buffer_s_class_sycl_v1_ranges_arg_in9_sync_buffer_hybridcpu_signextend_kernel_199_5gr_aunroll_x_out_buffer_out_0_tpl),
        .clock(clock),
        .resetn(resetn)
    );

    // i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_narrow_x(BITSELECT,33)@3
    assign i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_narrow_x_b = i_llvm_fpga_sync_buffer_s_class_sycl_v1_ranges_arg_in9_sync_buffer_hybridcpu_signextend_kernel_199_5gr_aunroll_x_out_buffer_out_0_tpl[61:0];

    // i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_shift_join_x(BITJOIN,34)@3
    assign i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_shift_join_x_q = {i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_narrow_x_b, i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_c_i2_0_245_1gr_x_q};

    // i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_lhsMSBs_select(BITSELECT,78)@3
    assign i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_lhsMSBs_select_b = $signed(i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_shift_join_x_q[63:2]);

    // i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_MSBs_sums(ADD,79)@3
    assign i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_MSBs_sums_a = {1'b0, i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_lhsMSBs_select_b};
    assign i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_MSBs_sums_b = {1'b0, i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_rhsMSBs_select_bit_select_merged_b};
    assign i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_MSBs_sums_o = $unsigned(i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_MSBs_sums_a) + $unsigned(i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_MSBs_sums_b);
    assign i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_MSBs_sums_q = $signed(i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_MSBs_sums_o[62:0]);

    // valid_fanout_reg3(REG,68)@2 + 1
    always_ff @ (posedge clock)
    begin
        if (0)
        begin
        end
        else
        begin
            valid_fanout_reg3_q <= in_i_valid;
        end
    end

    // i_llvm_fpga_sync_buffer_p1024_arg_in_sync_buffer_hybridcpu_signextend_kernel_199_7gr(BLACKBOX,21)@0
    // in in_i_dependence@3
    // in in_valid_in@3
    // out out_buffer_out@3
    // out out_valid_out@3
    HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000000r thei_llvm_fpga_sync_buffer_p1024_arg_in_sync_buffer_hybridcpu_signextend_kernel_199_7gr (
        .in_buffer_in(in_arg_in),
        .in_i_dependence(GND_q),
        .in_stall_in(GND_q),
        .in_valid_in(valid_fanout_reg3_q),
        .out_buffer_out(i_llvm_fpga_sync_buffer_p1024_arg_in_sync_buffer_hybridcpu_signextend_kernel_199_7gr_out_buffer_out),
        .out_stall_out(),
        .out_valid_out(),
        .clock(clock),
        .resetn(resetn)
    );

    // i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_rhsMSBs_select_bit_select_merged(BITSELECT,96)@3
    assign i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_rhsMSBs_select_bit_select_merged_b = $signed(i_llvm_fpga_sync_buffer_p1024_arg_in_sync_buffer_hybridcpu_signextend_kernel_199_7gr_out_buffer_out[63:2]);
    assign i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_rhsMSBs_select_bit_select_merged_c = $signed(i_llvm_fpga_sync_buffer_p1024_arg_in_sync_buffer_hybridcpu_signextend_kernel_199_7gr_out_buffer_out[1:0]);

    // i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_split_join(BITJOIN,80)@3
    assign i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_split_join_q = {i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_MSBs_sums_q, i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_rhsMSBs_select_bit_select_merged_c};

    // i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_dupName_0_trunc_sel_x(BITSELECT,36)@3
    assign i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_dupName_0_trunc_sel_x_b = i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_add_x_split_join_q[63:0];

    // redist4_i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_dupName_0_trunc_sel_x_b_1(DELAY,104)
    always_ff @ (posedge clock)
    begin
        if (0)
        begin
        end
        else
        begin
            redist4_i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_dupName_0_trunc_sel_x_b_1_q <= i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_dupName_0_trunc_sel_x_b;
        end
    end

    // i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_rhsMSBs_select_bit_select_merged(BITSELECT,98)@4
    assign i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_rhsMSBs_select_bit_select_merged_b = $signed(redist4_i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_dupName_0_trunc_sel_x_b_1_q[63:2]);
    assign i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_rhsMSBs_select_bit_select_merged_c = $signed(redist4_i_add_ptr_i85_hybridcpu_signextend_kernel_245_0gr_dupName_0_trunc_sel_x_b_1_q[1:0]);

    // i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_split_join(BITJOIN,95)@4
    assign i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_split_join_q = {i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_MSBs_sums_q, i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_rhsMSBs_select_bit_select_merged_c};

    // i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_dupName_0_trunc_sel_x(BITSELECT,54)@4
    assign i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_dupName_0_trunc_sel_x_b = i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_add_x_split_join_q[63:0];

    // GND(CONSTANT,0)
    assign GND_q = 1'b0;

    // sync_out_1_aunroll_x(GPOUT,63)@4
    assign out_c0_exi2_0_tpl = GND_q;
    assign out_c0_exi2_1_tpl = i_arrayidx_i_i_hybridcpu_signextend_kernel_254_0gr_dupName_0_trunc_sel_x_b;
    assign out_c0_exi2_2_tpl = i_arrayidx_i31_i_hybridcpu_signextend_kernel_302_0gr_dupName_0_trunc_sel_x_b;
    assign out_o_valid = valid_fanout_reg0_q;
    assign out_unnamed_HybridCPU_SignExtend_Kernel0 = GND_q;

endmodule
