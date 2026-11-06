package require -exact qsys 15.0
set_module_property NAME HybridCPU_SignExtend_early_di
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property GROUP oneAPI
set_module_property DISPLAY_NAME HybridCPU_SignExtend_early_di
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property SUPPORTED_DEVICE_FAMILIES {"Agilex 7" "Agilex"}
#### Synthesis fileset
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL HybridCPU_SignExtend_early_di
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file "HybridCPU_SignExtend_early_di.sv" SYSTEM_VERILOG PATH "./HybridCPU_SignExtend_early_di.sv"
add_fileset_file "dspba_library_ver.sv" SYSTEM_VERILOG PATH "./ip/windows64/lib/dspba/Libraries/sv/base/dspba_library_ver.sv"
add_fileset_file "acl_ecc_pkg.sv" SYSTEM_VERILOG PATH "./ip/acl_ecc_pkg.sv"
add_fileset_file "acl_data_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_data_fifo.sv"
add_fileset_file "acl_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_fifo.sv"
add_fileset_file "acl_altera_syncram_wrapped.sv" SYSTEM_VERILOG PATH "./ip/acl_altera_syncram_wrapped.sv"
add_fileset_file "acl_scfifo_wrapped.sv" SYSTEM_VERILOG PATH "./ip/acl_scfifo_wrapped.sv"
add_fileset_file "acl_ecc_decoder.sv" SYSTEM_VERILOG PATH "./ip/acl_ecc_decoder.sv"
add_fileset_file "acl_ecc_encoder.sv" SYSTEM_VERILOG PATH "./ip/acl_ecc_encoder.sv"
add_fileset_file "acl_ll_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_ll_fifo.sv"
add_fileset_file "acl_ll_ram_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_ll_ram_fifo.sv"
add_fileset_file "acl_valid_fifo_counter.sv" SYSTEM_VERILOG PATH "./ip/acl_valid_fifo_counter.sv"
add_fileset_file "acl_dspba_valid_fifo_counter.sv" SYSTEM_VERILOG PATH "./ip/acl_dspba_valid_fifo_counter.sv"
add_fileset_file "acl_staging_reg.sv" SYSTEM_VERILOG PATH "./ip/acl_staging_reg.sv"
add_fileset_file "hld_fifo.sv" SYSTEM_VERILOG PATH "./ip/hld_fifo.sv"
add_fileset_file "acl_mid_speed_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_mid_speed_fifo.sv"
add_fileset_file "acl_latency_one_ram_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_latency_one_ram_fifo.sv"
add_fileset_file "acl_latency_zero_ram_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_latency_zero_ram_fifo.sv"
add_fileset_file "hld_fifo_zero_width.sv" SYSTEM_VERILOG PATH "./ip/hld_fifo_zero_width.sv"
add_fileset_file "acl_high_speed_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_high_speed_fifo.sv"
add_fileset_file "acl_low_latency_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_low_latency_fifo.sv"
add_fileset_file "acl_zero_latency_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_zero_latency_fifo.sv"
add_fileset_file "acl_fanout_pipeline.sv" SYSTEM_VERILOG PATH "./ip/acl_fanout_pipeline.sv"
add_fileset_file "acl_std_synchronizer_nocut.sv" SYSTEM_VERILOG PATH "./ip/acl_std_synchronizer_nocut.sv"
add_fileset_file "acl_tessellated_incr_decr_threshold.sv" SYSTEM_VERILOG PATH "./ip/acl_tessellated_incr_decr_threshold.sv"
add_fileset_file "acl_tessellated_incr_lookahead.sv" SYSTEM_VERILOG PATH "./ip/acl_tessellated_incr_lookahead.sv"
add_fileset_file "acl_reset_handler.sv" SYSTEM_VERILOG PATH "./ip/acl_reset_handler.sv"
add_fileset_file "acl_lfsr.sv" SYSTEM_VERILOG PATH "./ip/acl_lfsr.sv"
add_fileset_file "acl_mlab_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_mlab_fifo.sv"
add_fileset_file "acl_parameter_assert.svh" SYSTEM_VERILOG PATH "./ip/acl_parameter_assert.svh"
add_fileset_file "acl_pipeline.sv" SYSTEM_VERILOG PATH "./ip/acl_pipeline.sv"
add_fileset_file "acl_pop.sv" SYSTEM_VERILOG PATH "./ip/acl_pop.sv"
add_fileset_file "acl_push.sv" SYSTEM_VERILOG PATH "./ip/acl_push.sv"
add_fileset_file "acl_dspba_buffer.sv" SYSTEM_VERILOG PATH "./ip/acl_dspba_buffer.sv"
add_fileset_file "acl_token_fifo_counter.sv" SYSTEM_VERILOG PATH "./ip/acl_token_fifo_counter.sv"
add_fileset_file "acl_full_detector.sv" SYSTEM_VERILOG PATH "./ip/acl_full_detector.sv"
add_fileset_file "acl_tessellated_incr_decr_decr.sv" SYSTEM_VERILOG PATH "./ip/acl_tessellated_incr_decr_decr.sv"
add_fileset_file "lsu_top.sv" SYSTEM_VERILOG PATH "./ip/lsu_top.sv"
add_fileset_file "lsu_permute_address.sv" SYSTEM_VERILOG PATH "./ip/lsu_permute_address.sv"
add_fileset_file "lsu_pipelined.sv" SYSTEM_VERILOG PATH "./ip/lsu_pipelined.sv"
add_fileset_file "lsu_enabled.sv" SYSTEM_VERILOG PATH "./ip/lsu_enabled.sv"
add_fileset_file "lsu_basic_coalescer.sv" SYSTEM_VERILOG PATH "./ip/lsu_basic_coalescer.sv"
add_fileset_file "lsu_simple.sv" SYSTEM_VERILOG PATH "./ip/lsu_simple.sv"
add_fileset_file "lsu_burst_host.sv" SYSTEM_VERILOG PATH "./ip/lsu_burst_host.sv"
add_fileset_file "lsu_bursting_load_stores.sv" SYSTEM_VERILOG PATH "./ip/lsu_bursting_load_stores.sv"
add_fileset_file "lsu_non_aligned_write.sv" SYSTEM_VERILOG PATH "./ip/lsu_non_aligned_write.sv"
add_fileset_file "lsu_read_cache.sv" SYSTEM_VERILOG PATH "./ip/lsu_read_cache.sv"
add_fileset_file "lsu_atomic.sv" SYSTEM_VERILOG PATH "./ip/lsu_atomic.sv"
add_fileset_file "lsu_prefetch_block.sv" SYSTEM_VERILOG PATH "./ip/lsu_prefetch_block.sv"
add_fileset_file "lsu_wide_wrapper.sv" SYSTEM_VERILOG PATH "./ip/lsu_wide_wrapper.sv"
add_fileset_file "lsu_streaming_prefetch.sv" SYSTEM_VERILOG PATH "./ip/lsu_streaming_prefetch.sv"
add_fileset_file "acl_aligned_burst_coalesced_lsu.sv" SYSTEM_VERILOG PATH "./ip/acl_aligned_burst_coalesced_lsu.sv"
add_fileset_file "acl_toggle_detect.sv" SYSTEM_VERILOG PATH "./ip/acl_toggle_detect.sv"
add_fileset_file "acl_debug_mem.sv" SYSTEM_VERILOG PATH "./ip/acl_debug_mem.sv"
add_fileset_file "lsu_burst_coalesced_pipelined_write.sv" SYSTEM_VERILOG PATH "./ip/lsu_burst_coalesced_pipelined_write.sv"
add_fileset_file "lsu_burst_coalesced_pipelined_read.sv" SYSTEM_VERILOG PATH "./ip/lsu_burst_coalesced_pipelined_read.sv"
add_fileset_file "acl_fifo_stall_valid_lookahead.sv" SYSTEM_VERILOG PATH "./ip/acl_fifo_stall_valid_lookahead.sv"
add_fileset_file "hld_global_load_store.sv" SYSTEM_VERILOG PATH "./ip/hld_global_load_store.sv"
add_fileset_file "hld_lsu.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu.sv"
add_fileset_file "hld_lsu_burst_coalescer.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_burst_coalescer.sv"
add_fileset_file "hld_lsu_coalescer_dynamic_timeout.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_coalescer_dynamic_timeout.sv"
add_fileset_file "hld_lsu_data_aligner.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_data_aligner.sv"
add_fileset_file "hld_lsu_read_cache.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_read_cache.sv"
add_fileset_file "hld_lsu_read_data_alignment.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_read_data_alignment.sv"
add_fileset_file "hld_lsu_unaligned_controller.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_unaligned_controller.sv"
add_fileset_file "hld_lsu_word_coalescer.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_word_coalescer.sv"
add_fileset_file "hld_lsu_write_data_alignment.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_write_data_alignment.sv"
add_fileset_file "hld_lsu_write_kernel_downstream.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_write_kernel_downstream.sv"
add_fileset_file "acl_shift_register_no_reset.sv" SYSTEM_VERILOG PATH "./ip/acl_shift_register_no_reset.sv"
add_fileset_file "hld_loop_profiler.sv" SYSTEM_VERILOG PATH "./ip/hld_loop_profiler.sv"
add_fileset_file "hld_sim_latency_tracker.sv" SYSTEM_VERILOG PATH "./ip/hld_sim_latency_tracker.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_function_wrapper.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_function_wrapper.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_function.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_function.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_bb_B2.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_bb_B2.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_B2_branch.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_B2_branch.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_B2_merge.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_B2_merge.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_bb_B2_stall_region.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_bb_B2_stall_region.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_pipeline_k000001r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_pipeline_k000001r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_bb_B0.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_bb_B0.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_B0_branch.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_B0_branch.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_B0_merge.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_B0_merge.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_bb_B0_stall_region.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_bb_B0_stall_region.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_B0_merge_reg.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_B0_merge_reg.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_bb_B1.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_bb_B1.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_B1_branch.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_B1_branch.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_B1_merge.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_B1_merge.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_bb_B1_stall_region.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_bb_B1_stall_region.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_mem_memdep000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_mem_memdep000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_mem_unname000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_mem_unname000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_readdata_reg_unnamed_10000000.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_readdata_reg_unnamed_10000000.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_pipeline_k000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_pipeline_k000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_pipeline_k000000g.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_pipeline_k000000g.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i1_mem000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i1_mem000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i1_mem000000g.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i1_mem000000g.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i4_fpg000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i4_fpg000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i4_fpg000000g.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i4_fpg000000g.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_me000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_me000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_me000000g.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_me000000g.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_no000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_no000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_no000000g.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_no000000g.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i4_fp000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i4_fp000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i4_fp000000g.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i4_fp000000g.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_B1_merge_reg.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_B1_merge_reg.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_sfc_s_c0_in_for_body0000000.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_sfc_s_c0_in_for_body0000000.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000001r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000001r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000000o.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000000o.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_sfc_logic_s_c0_in_fo000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_sfc_logic_s_c0_in_fo000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i5_i_0000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i5_i_0000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i5_i_000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i5_i_000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000001r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000001r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000002r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000002r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000003r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000003r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_sfc_s_c1_in_for_body0000004.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_sfc_s_c1_in_for_body0000004.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000002r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000002r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000003r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000003r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000001o.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000001o.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_sfc_logic_s_c1_in_fo000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_sfc_logic_s_c1_in_fo000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_bb_B1_sr_1.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_bb_B1_sr_1.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_bb_B2_sr_0.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_bb_B2_sr_0.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_function_cra_agent.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_function_cra_agent.sv"
add_fileset_file "acl_start_signal_chain_element.sv" SYSTEM_VERILOG PATH "./ip/acl_start_signal_chain_element.sv"
add_fileset_file "acl_task_copy_finish_detector.sv" SYSTEM_VERILOG PATH "./ip/acl_task_copy_finish_detector.sv"
add_fileset_file "acl_finish_signal_chain_element.sv" SYSTEM_VERILOG PATH "./ip/acl_finish_signal_chain_element.sv"
add_fileset_file "acl_reset_handler.sv" SYSTEM_VERILOG PATH "./ip/acl_reset_handler.sv"
add_fileset_file "acl_fanout_pipeline.sv" SYSTEM_VERILOG PATH "./ip/acl_fanout_pipeline.sv"
add_fileset_file "acl_std_synchronizer_nocut.sv" SYSTEM_VERILOG PATH "./ip/acl_std_synchronizer_nocut.sv"
add_fileset_file "acl_ecc_pkg.sv" SYSTEM_VERILOG PATH "./ip/acl_ecc_pkg.sv"
add_fileset_file "lsu_ic_top.sv" SYSTEM_VERILOG PATH "./ip/lsu_ic_top.sv"
add_fileset_file "lsu_ic_hybrid.sv" SYSTEM_VERILOG PATH "./ip/lsu_ic_hybrid.sv"
add_fileset_file "lsu_ic_token.sv" SYSTEM_VERILOG PATH "./ip/lsu_ic_token.sv"
add_fileset_file "lsu_ic_unbalance.sv" SYSTEM_VERILOG PATH "./ip/lsu_ic_unbalance.sv"
add_fileset_file "lsu_n_fast.sv" SYSTEM_VERILOG PATH "./ip/lsu_n_fast.sv"
add_fileset_file "lsu_n_token.sv" SYSTEM_VERILOG PATH "./ip/lsu_n_token.sv"
add_fileset_file "lsu_rd_back.sv" SYSTEM_VERILOG PATH "./ip/lsu_rd_back.sv"
add_fileset_file "lsu_rd_back_n.sv" SYSTEM_VERILOG PATH "./ip/lsu_rd_back_n.sv"
add_fileset_file "lsu_swdimm_token_ring.sv" SYSTEM_VERILOG PATH "./ip/lsu_swdimm_token_ring.sv"
add_fileset_file "lsu_token_ring.sv" SYSTEM_VERILOG PATH "./ip/lsu_token_ring.sv"
add_fileset_file "acl_scfifo_wrapped.sv" SYSTEM_VERILOG PATH "./ip/acl_scfifo_wrapped.sv"
add_fileset_file "acl_skid_buffer.sv" SYSTEM_VERILOG PATH "./ip/acl_skid_buffer.sv"
add_fileset_file "acl_high_speed_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_high_speed_fifo.sv"
add_fileset_file "acl_altera_syncram_wrapped.sv" SYSTEM_VERILOG PATH "./ip/acl_altera_syncram_wrapped.sv"
add_fileset_file "acl_ecc_encoder.sv" SYSTEM_VERILOG PATH "./ip/acl_ecc_encoder.sv"
add_fileset_file "acl_ecc_decoder.sv" SYSTEM_VERILOG PATH "./ip/acl_ecc_decoder.sv"
add_fileset_file "acl_lfsr.sv" SYSTEM_VERILOG PATH "./ip/acl_lfsr.sv"
add_fileset_file "acl_tessellated_incr_decr_threshold.sv" SYSTEM_VERILOG PATH "./ip/acl_tessellated_incr_decr_threshold.sv"
add_fileset_file "acl_tessellated_incr_lookahead.sv" SYSTEM_VERILOG PATH "./ip/acl_tessellated_incr_lookahead.sv"
add_fileset_file "cra_ring_node.sv" SYSTEM_VERILOG PATH "./ip/cra_ring_node.sv"
add_fileset_file "cra_ring_root.sv" SYSTEM_VERILOG PATH "./ip/cra_ring_root.sv"
add_fileset_file "cra_ring_rom.sv" SYSTEM_VERILOG PATH "./ip/cra_ring_rom.sv"
add_fileset_file "acl_rom_module.sv" SYSTEM_VERILOG PATH "./ip/acl_rom_module.sv"
#### Simulation fileset
add_fileset SIM_VERILOG SIM_VERILOG "" ""
set_fileset_property SIM_VERILOG TOP_LEVEL HybridCPU_SignExtend_early_di
set_fileset_property SIM_VERILOG ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file "HybridCPU_SignExtend_early_di.sv" SYSTEM_VERILOG PATH "./HybridCPU_SignExtend_early_di.sv"
add_fileset_file "dspba_library_ver.sv" SYSTEM_VERILOG PATH "./ip/windows64/lib/dspba/Libraries/sv/base/dspba_library_ver.sv"
add_fileset_file "acl_ecc_pkg.sv" SYSTEM_VERILOG PATH "./ip/acl_ecc_pkg.sv"
add_fileset_file "acl_data_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_data_fifo.sv"
add_fileset_file "acl_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_fifo.sv"
add_fileset_file "acl_altera_syncram_wrapped.sv" SYSTEM_VERILOG PATH "./ip/acl_altera_syncram_wrapped.sv"
add_fileset_file "acl_scfifo_wrapped.sv" SYSTEM_VERILOG PATH "./ip/acl_scfifo_wrapped.sv"
add_fileset_file "acl_ecc_decoder.sv" SYSTEM_VERILOG PATH "./ip/acl_ecc_decoder.sv"
add_fileset_file "acl_ecc_encoder.sv" SYSTEM_VERILOG PATH "./ip/acl_ecc_encoder.sv"
add_fileset_file "acl_ll_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_ll_fifo.sv"
add_fileset_file "acl_ll_ram_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_ll_ram_fifo.sv"
add_fileset_file "acl_valid_fifo_counter.sv" SYSTEM_VERILOG PATH "./ip/acl_valid_fifo_counter.sv"
add_fileset_file "acl_dspba_valid_fifo_counter.sv" SYSTEM_VERILOG PATH "./ip/acl_dspba_valid_fifo_counter.sv"
add_fileset_file "acl_staging_reg.sv" SYSTEM_VERILOG PATH "./ip/acl_staging_reg.sv"
add_fileset_file "hld_fifo.sv" SYSTEM_VERILOG PATH "./ip/hld_fifo.sv"
add_fileset_file "acl_mid_speed_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_mid_speed_fifo.sv"
add_fileset_file "acl_latency_one_ram_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_latency_one_ram_fifo.sv"
add_fileset_file "acl_latency_zero_ram_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_latency_zero_ram_fifo.sv"
add_fileset_file "hld_fifo_zero_width.sv" SYSTEM_VERILOG PATH "./ip/hld_fifo_zero_width.sv"
add_fileset_file "acl_high_speed_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_high_speed_fifo.sv"
add_fileset_file "acl_low_latency_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_low_latency_fifo.sv"
add_fileset_file "acl_zero_latency_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_zero_latency_fifo.sv"
add_fileset_file "acl_fanout_pipeline.sv" SYSTEM_VERILOG PATH "./ip/acl_fanout_pipeline.sv"
add_fileset_file "acl_std_synchronizer_nocut.sv" SYSTEM_VERILOG PATH "./ip/acl_std_synchronizer_nocut.sv"
add_fileset_file "acl_tessellated_incr_decr_threshold.sv" SYSTEM_VERILOG PATH "./ip/acl_tessellated_incr_decr_threshold.sv"
add_fileset_file "acl_tessellated_incr_lookahead.sv" SYSTEM_VERILOG PATH "./ip/acl_tessellated_incr_lookahead.sv"
add_fileset_file "acl_reset_handler.sv" SYSTEM_VERILOG PATH "./ip/acl_reset_handler.sv"
add_fileset_file "acl_lfsr.sv" SYSTEM_VERILOG PATH "./ip/acl_lfsr.sv"
add_fileset_file "acl_mlab_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_mlab_fifo.sv"
add_fileset_file "acl_parameter_assert.svh" SYSTEM_VERILOG PATH "./ip/acl_parameter_assert.svh"
add_fileset_file "acl_pipeline.sv" SYSTEM_VERILOG PATH "./ip/acl_pipeline.sv"
add_fileset_file "acl_pop.sv" SYSTEM_VERILOG PATH "./ip/acl_pop.sv"
add_fileset_file "acl_push.sv" SYSTEM_VERILOG PATH "./ip/acl_push.sv"
add_fileset_file "acl_dspba_buffer.sv" SYSTEM_VERILOG PATH "./ip/acl_dspba_buffer.sv"
add_fileset_file "acl_token_fifo_counter.sv" SYSTEM_VERILOG PATH "./ip/acl_token_fifo_counter.sv"
add_fileset_file "acl_full_detector.sv" SYSTEM_VERILOG PATH "./ip/acl_full_detector.sv"
add_fileset_file "acl_tessellated_incr_decr_decr.sv" SYSTEM_VERILOG PATH "./ip/acl_tessellated_incr_decr_decr.sv"
add_fileset_file "lsu_top.sv" SYSTEM_VERILOG PATH "./ip/lsu_top.sv"
add_fileset_file "lsu_permute_address.sv" SYSTEM_VERILOG PATH "./ip/lsu_permute_address.sv"
add_fileset_file "lsu_pipelined.sv" SYSTEM_VERILOG PATH "./ip/lsu_pipelined.sv"
add_fileset_file "lsu_enabled.sv" SYSTEM_VERILOG PATH "./ip/lsu_enabled.sv"
add_fileset_file "lsu_basic_coalescer.sv" SYSTEM_VERILOG PATH "./ip/lsu_basic_coalescer.sv"
add_fileset_file "lsu_simple.sv" SYSTEM_VERILOG PATH "./ip/lsu_simple.sv"
add_fileset_file "lsu_burst_host.sv" SYSTEM_VERILOG PATH "./ip/lsu_burst_host.sv"
add_fileset_file "lsu_bursting_load_stores.sv" SYSTEM_VERILOG PATH "./ip/lsu_bursting_load_stores.sv"
add_fileset_file "lsu_non_aligned_write.sv" SYSTEM_VERILOG PATH "./ip/lsu_non_aligned_write.sv"
add_fileset_file "lsu_read_cache.sv" SYSTEM_VERILOG PATH "./ip/lsu_read_cache.sv"
add_fileset_file "lsu_atomic.sv" SYSTEM_VERILOG PATH "./ip/lsu_atomic.sv"
add_fileset_file "lsu_prefetch_block.sv" SYSTEM_VERILOG PATH "./ip/lsu_prefetch_block.sv"
add_fileset_file "lsu_wide_wrapper.sv" SYSTEM_VERILOG PATH "./ip/lsu_wide_wrapper.sv"
add_fileset_file "lsu_streaming_prefetch.sv" SYSTEM_VERILOG PATH "./ip/lsu_streaming_prefetch.sv"
add_fileset_file "acl_aligned_burst_coalesced_lsu.sv" SYSTEM_VERILOG PATH "./ip/acl_aligned_burst_coalesced_lsu.sv"
add_fileset_file "acl_toggle_detect.sv" SYSTEM_VERILOG PATH "./ip/acl_toggle_detect.sv"
add_fileset_file "acl_debug_mem.sv" SYSTEM_VERILOG PATH "./ip/acl_debug_mem.sv"
add_fileset_file "lsu_burst_coalesced_pipelined_write.sv" SYSTEM_VERILOG PATH "./ip/lsu_burst_coalesced_pipelined_write.sv"
add_fileset_file "lsu_burst_coalesced_pipelined_read.sv" SYSTEM_VERILOG PATH "./ip/lsu_burst_coalesced_pipelined_read.sv"
add_fileset_file "acl_fifo_stall_valid_lookahead.sv" SYSTEM_VERILOG PATH "./ip/acl_fifo_stall_valid_lookahead.sv"
add_fileset_file "hld_global_load_store.sv" SYSTEM_VERILOG PATH "./ip/hld_global_load_store.sv"
add_fileset_file "hld_lsu.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu.sv"
add_fileset_file "hld_lsu_burst_coalescer.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_burst_coalescer.sv"
add_fileset_file "hld_lsu_coalescer_dynamic_timeout.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_coalescer_dynamic_timeout.sv"
add_fileset_file "hld_lsu_data_aligner.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_data_aligner.sv"
add_fileset_file "hld_lsu_read_cache.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_read_cache.sv"
add_fileset_file "hld_lsu_read_data_alignment.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_read_data_alignment.sv"
add_fileset_file "hld_lsu_unaligned_controller.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_unaligned_controller.sv"
add_fileset_file "hld_lsu_word_coalescer.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_word_coalescer.sv"
add_fileset_file "hld_lsu_write_data_alignment.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_write_data_alignment.sv"
add_fileset_file "hld_lsu_write_kernel_downstream.sv" SYSTEM_VERILOG PATH "./ip/hld_lsu_write_kernel_downstream.sv"
add_fileset_file "acl_shift_register_no_reset.sv" SYSTEM_VERILOG PATH "./ip/acl_shift_register_no_reset.sv"
add_fileset_file "hld_loop_profiler.sv" SYSTEM_VERILOG PATH "./ip/hld_loop_profiler.sv"
add_fileset_file "hld_sim_latency_tracker.sv" SYSTEM_VERILOG PATH "./ip/hld_sim_latency_tracker.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_function_wrapper.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_function_wrapper.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_function.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_function.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_bb_B2.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_bb_B2.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_B2_branch.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_B2_branch.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_B2_merge.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_B2_merge.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_bb_B2_stall_region.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_bb_B2_stall_region.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_pipeline_k000001r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_pipeline_k000001r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_bb_B0.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_bb_B0.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_B0_branch.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_B0_branch.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_B0_merge.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_B0_merge.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_bb_B0_stall_region.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_bb_B0_stall_region.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_B0_merge_reg.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_B0_merge_reg.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_bb_B1.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_bb_B1.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_B1_branch.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_B1_branch.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_B1_merge.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_B1_merge.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_bb_B1_stall_region.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_bb_B1_stall_region.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_mem_memdep000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_mem_memdep000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_mem_unname000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_mem_unname000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_readdata_reg_unnamed_10000000.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_readdata_reg_unnamed_10000000.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_pipeline_k000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_pipeline_k000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_pipeline_k000000g.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_pipeline_k000000g.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i1_mem000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i1_mem000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i1_mem000000g.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i1_mem000000g.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i4_fpg000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i4_fpg000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i4_fpg000000g.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i4_fpg000000g.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_me000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_me000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_me000000g.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_me000000g.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_no000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_no000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_no000000g.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i1_no000000g.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i4_fp000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i4_fp000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i4_fp000000g.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i4_fp000000g.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_B1_merge_reg.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_B1_merge_reg.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_sfc_s_c0_in_for_body0000000.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_sfc_s_c0_in_for_body0000000.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000001r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000001r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000000o.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000000o.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_sfc_logic_s_c0_in_fo000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_sfc_logic_s_c0_in_fo000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i5_i_0000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_pop_i5_i_0000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i5_i_000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_push_i5_i_000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000001r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000001r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000002r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000002r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000003r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sync_buffe000003r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_sfc_s_c1_in_for_body0000004.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_sfc_s_c1_in_for_body0000004.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000002r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000002r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000003r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000003r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000001o.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_llvm_fpga_sfc_exit_s000001o.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_i_sfc_logic_s_c1_in_fo000000r.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_i_sfc_logic_s_c1_in_fo000000r.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_bb_B1_sr_1.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_bb_B1_sr_1.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_bb_B2_sr_0.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_bb_B2_sr_0.sv"
add_fileset_file "HybridCPU_SignExtend_Kernel_function_cra_agent.sv" SYSTEM_VERILOG PATH "./kernel_hdl/HybridCPU_SignExtend_Kernel/HybridCPU_SignExtend_Kernel_function_cra_agent.sv"
add_fileset_file "acl_start_signal_chain_element.sv" SYSTEM_VERILOG PATH "./ip/acl_start_signal_chain_element.sv"
add_fileset_file "acl_task_copy_finish_detector.sv" SYSTEM_VERILOG PATH "./ip/acl_task_copy_finish_detector.sv"
add_fileset_file "acl_finish_signal_chain_element.sv" SYSTEM_VERILOG PATH "./ip/acl_finish_signal_chain_element.sv"
add_fileset_file "acl_reset_handler.sv" SYSTEM_VERILOG PATH "./ip/acl_reset_handler.sv"
add_fileset_file "acl_fanout_pipeline.sv" SYSTEM_VERILOG PATH "./ip/acl_fanout_pipeline.sv"
add_fileset_file "acl_std_synchronizer_nocut.sv" SYSTEM_VERILOG PATH "./ip/acl_std_synchronizer_nocut.sv"
add_fileset_file "acl_ecc_pkg.sv" SYSTEM_VERILOG PATH "./ip/acl_ecc_pkg.sv"
add_fileset_file "lsu_ic_top.sv" SYSTEM_VERILOG PATH "./ip/lsu_ic_top.sv"
add_fileset_file "lsu_ic_hybrid.sv" SYSTEM_VERILOG PATH "./ip/lsu_ic_hybrid.sv"
add_fileset_file "lsu_ic_token.sv" SYSTEM_VERILOG PATH "./ip/lsu_ic_token.sv"
add_fileset_file "lsu_ic_unbalance.sv" SYSTEM_VERILOG PATH "./ip/lsu_ic_unbalance.sv"
add_fileset_file "lsu_n_fast.sv" SYSTEM_VERILOG PATH "./ip/lsu_n_fast.sv"
add_fileset_file "lsu_n_token.sv" SYSTEM_VERILOG PATH "./ip/lsu_n_token.sv"
add_fileset_file "lsu_rd_back.sv" SYSTEM_VERILOG PATH "./ip/lsu_rd_back.sv"
add_fileset_file "lsu_rd_back_n.sv" SYSTEM_VERILOG PATH "./ip/lsu_rd_back_n.sv"
add_fileset_file "lsu_swdimm_token_ring.sv" SYSTEM_VERILOG PATH "./ip/lsu_swdimm_token_ring.sv"
add_fileset_file "lsu_token_ring.sv" SYSTEM_VERILOG PATH "./ip/lsu_token_ring.sv"
add_fileset_file "acl_scfifo_wrapped.sv" SYSTEM_VERILOG PATH "./ip/acl_scfifo_wrapped.sv"
add_fileset_file "acl_skid_buffer.sv" SYSTEM_VERILOG PATH "./ip/acl_skid_buffer.sv"
add_fileset_file "acl_high_speed_fifo.sv" SYSTEM_VERILOG PATH "./ip/acl_high_speed_fifo.sv"
add_fileset_file "acl_altera_syncram_wrapped.sv" SYSTEM_VERILOG PATH "./ip/acl_altera_syncram_wrapped.sv"
add_fileset_file "acl_ecc_encoder.sv" SYSTEM_VERILOG PATH "./ip/acl_ecc_encoder.sv"
add_fileset_file "acl_ecc_decoder.sv" SYSTEM_VERILOG PATH "./ip/acl_ecc_decoder.sv"
add_fileset_file "acl_lfsr.sv" SYSTEM_VERILOG PATH "./ip/acl_lfsr.sv"
add_fileset_file "acl_tessellated_incr_decr_threshold.sv" SYSTEM_VERILOG PATH "./ip/acl_tessellated_incr_decr_threshold.sv"
add_fileset_file "acl_tessellated_incr_lookahead.sv" SYSTEM_VERILOG PATH "./ip/acl_tessellated_incr_lookahead.sv"
add_fileset_file "cra_ring_node.sv" SYSTEM_VERILOG PATH "./ip/cra_ring_node.sv"
add_fileset_file "cra_ring_root.sv" SYSTEM_VERILOG PATH "./ip/cra_ring_root.sv"
add_fileset_file "cra_ring_rom.sv" SYSTEM_VERILOG PATH "./ip/cra_ring_rom.sv"
add_fileset_file "acl_rom_module.sv" SYSTEM_VERILOG PATH "./ip/acl_rom_module.sv"
#### Primary clock for the component
add_interface clock clock end
set_interface_property clock ENABLED true
add_interface_port clock clock clk input 1

#### resetn
add_interface resetn reset end
set_interface_property resetn associatedClock clock
set_interface_property resetn synchronousEdges BOTH
add_interface_port resetn resetn reset_n input 1

####  device_exception_bus
add_interface device_exception_bus conduit end
set_interface_property device_exception_bus ENABLED true
set_interface_property device_exception_bus associatedClock clock
set_interface_property device_exception_bus associatedReset resetn
add_interface_port device_exception_bus device_exception_bus data output 64

#### IRQ interfaces kernel_irqs
add_interface kernel_irqs interrupt end
set_interface_property kernel_irqs ENABLED true
set_interface_property kernel_irqs associatedClock clock
add_interface_port kernel_irqs kernel_irqs irq output 1

#### Host interface avm_mem_gmem0_0_port_0_0_rw with base address 0
add_interface avm_mem_gmem0_0_port_0_0_rw avalon start
set_interface_property avm_mem_gmem0_0_port_0_0_rw ENABLED true
set_interface_property avm_mem_gmem0_0_port_0_0_rw associatedClock clock
set_interface_property avm_mem_gmem0_0_port_0_0_rw associatedReset resetn
set_interface_property avm_mem_gmem0_0_port_0_0_rw burstOnBurstBoundariesOnly false
set_interface_property avm_mem_gmem0_0_port_0_0_rw doStreamReads false
set_interface_property avm_mem_gmem0_0_port_0_0_rw doStreamWrites false
set_interface_property avm_mem_gmem0_0_port_0_0_rw linewrapBursts false
set_interface_property avm_mem_gmem0_0_port_0_0_rw readWaitTime 0
add_interface_port avm_mem_gmem0_0_port_0_0_rw avm_mem_gmem0_0_port_0_0_rw_address address output 35
add_interface_port avm_mem_gmem0_0_port_0_0_rw avm_mem_gmem0_0_port_0_0_rw_byteenable byteenable output 8
add_interface_port avm_mem_gmem0_0_port_0_0_rw avm_mem_gmem0_0_port_0_0_rw_readdatavalid readdatavalid input 1
add_interface_port avm_mem_gmem0_0_port_0_0_rw avm_mem_gmem0_0_port_0_0_rw_read read output 1
add_interface_port avm_mem_gmem0_0_port_0_0_rw avm_mem_gmem0_0_port_0_0_rw_readdata readdata input 64
add_interface_port avm_mem_gmem0_0_port_0_0_rw avm_mem_gmem0_0_port_0_0_rw_write write output 1
add_interface_port avm_mem_gmem0_0_port_0_0_rw avm_mem_gmem0_0_port_0_0_rw_writedata writedata output 64
add_interface_port avm_mem_gmem0_0_port_0_0_rw avm_mem_gmem0_0_port_0_0_rw_waitrequest waitrequest input 1
add_interface_port avm_mem_gmem0_0_port_0_0_rw avm_mem_gmem0_0_port_0_0_rw_burstcount burstcount output 1

#### Agent interface csr_ring_root_avs
add_interface csr_ring_root_avs avalon end
set_interface_property csr_ring_root_avs ENABLED true
set_interface_property csr_ring_root_avs associatedClock clock
set_interface_property csr_ring_root_avs associatedReset resetn
set_interface_property csr_ring_root_avs addressAlignment DYNAMIC
set_interface_property csr_ring_root_avs burstOnBurstBoundariesOnly false
set_interface_property csr_ring_root_avs explicitAddressSpan 0
set_interface_property csr_ring_root_avs holdTime 0
set_interface_property csr_ring_root_avs isMemoryDevice false
set_interface_property csr_ring_root_avs isNonVolatileStorage false
set_interface_property csr_ring_root_avs linewrapBursts false
set_interface_property csr_ring_root_avs maximumPendingReadTransactions 1
set_interface_property csr_ring_root_avs readLatency 0
set_interface_property csr_ring_root_avs readWaitTime 0
set_interface_property csr_ring_root_avs printableDevice false
set_interface_property csr_ring_root_avs setupTime 0
set_interface_property csr_ring_root_avs timingUnits Cycles
set_interface_property csr_ring_root_avs writeWaitTime 0
set_interface_assignment csr_ring_root_avs hls.cosim.name {}
add_interface_port csr_ring_root_avs csr_ring_root_avs_read read input 1
add_interface_port csr_ring_root_avs csr_ring_root_avs_readdata readdata output 64
add_interface_port csr_ring_root_avs csr_ring_root_avs_readdatavalid readdatavalid output 1
add_interface_port csr_ring_root_avs csr_ring_root_avs_write write input 1
add_interface_port csr_ring_root_avs csr_ring_root_avs_writedata writedata input 64
add_interface_port csr_ring_root_avs csr_ring_root_avs_address address input 5
add_interface_port csr_ring_root_avs csr_ring_root_avs_byteenable byteenable input 8
add_interface_port csr_ring_root_avs csr_ring_root_avs_waitrequest waitrequest output 1

#### Quartus settings (QIP strings)
set_qip_strings { "set_instance_assignment -entity \"%entityName%\" -library \"%libraryName%\" -name AUTO_SHIFT_REGISTER_RECOGNITION OFF -to *_NO_SHIFT_REG*"  }
