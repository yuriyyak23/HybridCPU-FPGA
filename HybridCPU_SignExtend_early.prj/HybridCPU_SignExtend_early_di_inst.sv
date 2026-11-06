// Example instance of the top level module for: 
//     HybridCPU_SignExtend_early_di
// To include this component in your design, include: 
//     HybridCPU_SignExtend_early_di.qsys
// in your Quartus project and follow the template 
// below to instantiate the IP.  Alternatively, the IP core 
// can be generated from a Qsys system.

HybridCPU_SignExtend_early_di HybridCPU_SignExtend_early_di_inst (
  // Interface: clock (clock end)
  .clock                                    ( ), // 1-bit clk input
  // Interface: resetn (reset end)
  .resetn                                   ( ), // 1-bit reset_n input
  // Interface: device_exception_bus (conduit end)
  .device_exception_bus                     ( ), // 64-bit data output
  // Interface: kernel_irqs (interrupt end)
  .kernel_irqs                              ( ), // 1-bit irq output
  // Interface: avm_mem_gmem0_0_port_0_0_rw (avalon start)
  .avm_mem_gmem0_0_port_0_0_rw_address      ( ), // 35-bit address output
  .avm_mem_gmem0_0_port_0_0_rw_byteenable   ( ), // 8-bit byteenable output
  .avm_mem_gmem0_0_port_0_0_rw_readdatavalid( ), // 1-bit readdatavalid input
  .avm_mem_gmem0_0_port_0_0_rw_read         ( ), // 1-bit read output
  .avm_mem_gmem0_0_port_0_0_rw_readdata     ( ), // 64-bit readdata input
  .avm_mem_gmem0_0_port_0_0_rw_write        ( ), // 1-bit write output
  .avm_mem_gmem0_0_port_0_0_rw_writedata    ( ), // 64-bit writedata output
  .avm_mem_gmem0_0_port_0_0_rw_waitrequest  ( ), // 1-bit waitrequest input
  .avm_mem_gmem0_0_port_0_0_rw_burstcount   ( ), // 1-bit burstcount output
  // Interface: csr_ring_root_avs (avalon end)
  .csr_ring_root_avs_read                   ( ), // 1-bit read input
  .csr_ring_root_avs_readdata               ( ), // 64-bit readdata output
  .csr_ring_root_avs_readdatavalid          ( ), // 1-bit readdatavalid output
  .csr_ring_root_avs_write                  ( ), // 1-bit write input
  .csr_ring_root_avs_writedata              ( ), // 64-bit writedata input
  .csr_ring_root_avs_address                ( ), // 5-bit address input
  .csr_ring_root_avs_byteenable             ( ), // 8-bit byteenable input
  .csr_ring_root_avs_waitrequest            ( )  // 1-bit waitrequest output
);
