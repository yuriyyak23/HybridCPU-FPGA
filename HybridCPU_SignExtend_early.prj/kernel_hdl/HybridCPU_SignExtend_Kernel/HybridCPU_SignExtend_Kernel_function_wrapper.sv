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

// SystemVerilog created from HybridCPU_SignExtend_Kernel_function_wrapper
// Created for function/kernel HybridCPU_SignExtend_Kernel
// SystemVerilog created on Fri Nov  6 06:17:16 2026


(* altera_attribute = "-name AUTO_SHIFT_REGISTER_RECOGNITION OFF; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 10037; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 15400; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 12020; -name MESSAGE_DISABLE 12030; -name MESSAGE_DISABLE 12010; -name MESSAGE_DISABLE 12110; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 13410; -name MESSAGE_DISABLE 113007; -name MESSAGE_DISABLE 10958" *)
module HybridCPU_SignExtend_Kernel_function_wrapper (
    input wire [63:0] avm_memdep_HybridCPU_SignExtend_Kernel_readdata,
    input wire [0:0] avm_memdep_HybridCPU_SignExtend_Kernel_readdatavalid,
    input wire [0:0] avm_memdep_HybridCPU_SignExtend_Kernel_waitrequest,
    input wire [0:0] avm_memdep_HybridCPU_SignExtend_Kernel_writeack,
    input wire [63:0] avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_readdata,
    input wire [0:0] avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_readdatavalid,
    input wire [0:0] avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_waitrequest,
    input wire [0:0] avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_writeack,
    input wire [511:0] kernel_arguments,
    input wire [0:0] local_router_hang,
    input wire [0:0] stall_in,
    input wire [0:0] start,
    input wire [0:0] valid_in,
    output wire [34:0] avm_memdep_HybridCPU_SignExtend_Kernel_address,
    output wire [0:0] avm_memdep_HybridCPU_SignExtend_Kernel_burstcount,
    output wire [7:0] avm_memdep_HybridCPU_SignExtend_Kernel_byteenable,
    output wire [0:0] avm_memdep_HybridCPU_SignExtend_Kernel_enable,
    output wire [0:0] avm_memdep_HybridCPU_SignExtend_Kernel_read,
    output wire [0:0] avm_memdep_HybridCPU_SignExtend_Kernel_write,
    output wire [63:0] avm_memdep_HybridCPU_SignExtend_Kernel_writedata,
    output wire [34:0] avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_address,
    output wire [0:0] avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_burstcount,
    output wire [7:0] avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_byteenable,
    output wire [0:0] avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_enable,
    output wire [0:0] avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_read,
    output wire [0:0] avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_write,
    output wire [63:0] avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_writedata,
    output wire [0:0] has_a_lsu_active,
    output wire [0:0] has_a_write_pending,
    output wire [0:0] kernel_valid_in,
    output wire [0:0] kernel_valid_out,
    input wire clock,
    input wire resetn
    );

    wire [0:0] GND_q;
    wire [63:0] arg_arg_in6_select_b;
    wire [63:0] arg_arg_in7_select_b;
    wire [63:0] arg_arg_in9_select_b;
    wire [63:0] arg_arg_in_select_b;
    wire [63:0] arg_arg_out1_select_b;
    wire [63:0] arg_arg_out2_select_b;
    wire [63:0] arg_arg_out4_select_b;
    wire [63:0] arg_arg_out_select_b;
    wire [0:0] valid_in_pulse_q;
    reg [0:0] valid_pulse_reg1_NO_SHIFT_REG_q;
    reg [0:0] valid_pulse_reg2_NO_SHIFT_REG_q;
    wire [34:0] HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_address;
    wire [0:0] HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount;
    wire [7:0] HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable;
    wire [0:0] HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_enable;
    wire [0:0] HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_read;
    wire [0:0] HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_write;
    wire [63:0] HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata;
    wire [0:0] HybridCPU_SignExtend_Kernel_function_aunroll_x_out_o_active_memdep;
    wire [34:0] HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address;
    wire [0:0] HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount;
    wire [7:0] HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable;
    wire [0:0] HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable;
    wire [0:0] HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read;
    wire [0:0] HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write;
    wire [63:0] HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata;
    wire [0:0] HybridCPU_SignExtend_Kernel_function_aunroll_x_out_valid_out;
    reg [0:0] rst_sync_rst_sclrn;


    // arg_arg_out4_select(BITSELECT,8)
    assign arg_arg_out4_select_b = kernel_arguments[255:192];

    // arg_arg_out2_select(BITSELECT,7)
    assign arg_arg_out2_select_b = kernel_arguments[191:128];

    // arg_arg_out1_select(BITSELECT,6)
    assign arg_arg_out1_select_b = kernel_arguments[127:64];

    // arg_arg_in9_select(BITSELECT,4)
    assign arg_arg_in9_select_b = kernel_arguments[511:448];

    // arg_arg_in7_select(BITSELECT,3)
    assign arg_arg_in7_select_b = kernel_arguments[447:384];

    // arg_arg_in6_select(BITSELECT,2)
    assign arg_arg_in6_select_b = kernel_arguments[383:320];

    // valid_pulse_reg2_NO_SHIFT_REG(REG,39)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            valid_pulse_reg2_NO_SHIFT_REG_q <= $unsigned(1'b0);
        end
        else
        begin
            valid_pulse_reg2_NO_SHIFT_REG_q <= valid_pulse_reg1_NO_SHIFT_REG_q;
        end
    end

    // valid_pulse_reg1_NO_SHIFT_REG(REG,38)
    always_ff @ (posedge clock)
    begin
        if (!rst_sync_rst_sclrn[0])
        begin
            valid_pulse_reg1_NO_SHIFT_REG_q <= $unsigned(1'b0);
        end
        else
        begin
            valid_pulse_reg1_NO_SHIFT_REG_q <= start;
        end
    end

    // valid_in_pulse(LOGICAL,37)
    assign valid_in_pulse_q = valid_pulse_reg1_NO_SHIFT_REG_q & valid_pulse_reg2_NO_SHIFT_REG_q;

    // GND(CONSTANT,0)
    assign GND_q = 1'b0;

    // arg_arg_out_select(BITSELECT,9)
    assign arg_arg_out_select_b = kernel_arguments[63:0];

    // arg_arg_in_select(BITSELECT,5)
    assign arg_arg_in_select_b = kernel_arguments[319:256];

    // HybridCPU_SignExtend_Kernel_function_aunroll_x(BLACKBOX,41)
    HybridCPU_SignExtend_Kernel_function theHybridCPU_SignExtend_Kernel_function_aunroll_x (
        .in_arg_arg_in(arg_arg_in_select_b),
        .in_arg_arg_out(arg_arg_out_select_b),
        .in_memdep_HybridCPU_SignExtend_Kernel_avm_readdata(avm_memdep_HybridCPU_SignExtend_Kernel_readdata),
        .in_memdep_HybridCPU_SignExtend_Kernel_avm_readdatavalid(avm_memdep_HybridCPU_SignExtend_Kernel_readdatavalid),
        .in_memdep_HybridCPU_SignExtend_Kernel_avm_waitrequest(avm_memdep_HybridCPU_SignExtend_Kernel_waitrequest),
        .in_memdep_HybridCPU_SignExtend_Kernel_avm_writeack(avm_memdep_HybridCPU_SignExtend_Kernel_writeack),
        .in_stall_in(GND_q),
        .in_start(start),
        .in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdata(avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_readdata),
        .in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_readdatavalid(avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_readdatavalid),
        .in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_waitrequest(avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_waitrequest),
        .in_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writeack(avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_writeack),
        .in_valid_in(valid_in_pulse_q),
        .in_arg_arg_in6_0_tpl(arg_arg_in6_select_b),
        .in_arg_arg_in7_0_tpl(arg_arg_in7_select_b),
        .in_arg_arg_in9_0_tpl(arg_arg_in9_select_b),
        .in_arg_arg_out1_0_tpl(arg_arg_out1_select_b),
        .in_arg_arg_out2_0_tpl(arg_arg_out2_select_b),
        .in_arg_arg_out4_0_tpl(arg_arg_out4_select_b),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_address(HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_address),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount(HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable(HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_enable(HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_enable),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_read(HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_read),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_write(HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_write),
        .out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata(HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata),
        .out_o_active_memdep(HybridCPU_SignExtend_Kernel_function_aunroll_x_out_o_active_memdep),
        .out_stall_out(),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address(HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount(HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable(HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable(HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read(HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write(HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write),
        .out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata(HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata),
        .out_valid_out(HybridCPU_SignExtend_Kernel_function_aunroll_x_out_valid_out),
        .clock(clock),
        .resetn(resetn)
    );

    // avm_memdep_HybridCPU_SignExtend_Kernel_address(GPOUT,55)
    assign avm_memdep_HybridCPU_SignExtend_Kernel_address = HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_address;

    // avm_memdep_HybridCPU_SignExtend_Kernel_burstcount(GPOUT,56)
    assign avm_memdep_HybridCPU_SignExtend_Kernel_burstcount = HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_burstcount;

    // avm_memdep_HybridCPU_SignExtend_Kernel_byteenable(GPOUT,57)
    assign avm_memdep_HybridCPU_SignExtend_Kernel_byteenable = HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_byteenable;

    // avm_memdep_HybridCPU_SignExtend_Kernel_enable(GPOUT,58)
    assign avm_memdep_HybridCPU_SignExtend_Kernel_enable = HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_enable;

    // avm_memdep_HybridCPU_SignExtend_Kernel_read(GPOUT,59)
    assign avm_memdep_HybridCPU_SignExtend_Kernel_read = HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_read;

    // avm_memdep_HybridCPU_SignExtend_Kernel_write(GPOUT,60)
    assign avm_memdep_HybridCPU_SignExtend_Kernel_write = HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_write;

    // avm_memdep_HybridCPU_SignExtend_Kernel_writedata(GPOUT,61)
    assign avm_memdep_HybridCPU_SignExtend_Kernel_writedata = HybridCPU_SignExtend_Kernel_function_aunroll_x_out_memdep_HybridCPU_SignExtend_Kernel_avm_writedata;

    // avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_address(GPOUT,62)
    assign avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_address = HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_address;

    // avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_burstcount(GPOUT,63)
    assign avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_burstcount = HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_burstcount;

    // avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_byteenable(GPOUT,64)
    assign avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_byteenable = HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_byteenable;

    // avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_enable(GPOUT,65)
    assign avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_enable = HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_enable;

    // avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_read(GPOUT,66)
    assign avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_read = HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_read;

    // avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_write(GPOUT,67)
    assign avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_write = HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_write;

    // avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_writedata(GPOUT,68)
    assign avm_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_writedata = HybridCPU_SignExtend_Kernel_function_aunroll_x_out_unnamed_HybridCPU_SignExtend_Kernel1_HybridCPU_SignExtend_Kernel_avm_writedata;

    // has_a_lsu_active(GPOUT,69)
    assign has_a_lsu_active = HybridCPU_SignExtend_Kernel_function_aunroll_x_out_o_active_memdep;

    // has_a_write_pending(GPOUT,70)
    assign has_a_write_pending = HybridCPU_SignExtend_Kernel_function_aunroll_x_out_o_active_memdep;

    // kernel_valid_in(GPOUT,71)
    assign kernel_valid_in = valid_in_pulse_q;

    // kernel_valid_out(GPOUT,72)
    assign kernel_valid_out = HybridCPU_SignExtend_Kernel_function_aunroll_x_out_valid_out;

    // rst_sync(RESETSYNC,73)
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
