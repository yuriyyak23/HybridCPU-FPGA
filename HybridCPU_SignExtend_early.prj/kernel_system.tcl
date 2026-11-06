package require -exact qsys 12.1
set_validation_property AUTOMATIC_VALIDATION false

add_instance clk_1x altera_clock_bridge
set_instance_parameter_value clk_1x "EXPLICIT_CLOCK_RATE" "0"
set_instance_parameter_value clk_1x "NUM_CLOCK_OUTPUTS" "1"
add_interface clock_reset clock sink
set_interface_property clock_reset EXPORT_OF clk_1x.in_clk

add_instance reset altera_reset_bridge
set_instance_parameter_value reset "ACTIVE_LOW_RESET" "1"
set_instance_parameter_value reset "SYNCHRONOUS_EDGES" "deassert"
set_instance_parameter_value reset "NUM_RESET_OUTPUTS" "1"
add_interface clock_reset_reset reset sink
set_interface_property clock_reset_reset EXPORT_OF reset.in_reset
add_connection clk_1x.out_clk reset.clk

add_instance HybridCPU_SignExtend_early_sys HybridCPU_SignExtend_early_sys

add_connection clk_1x.out_clk HybridCPU_SignExtend_early_sys.clock_reset
add_connection reset.out_reset HybridCPU_SignExtend_early_sys.clock_reset_reset

## Connect global memory hosts to memory interfaces
add_interface mem0_rw avalon end
set_interface_property mem0_rw EXPORT_OF HybridCPU_SignExtend_early_sys.avm_mem_gmem0_0_port_0_0_rw

## Connect external hosts to internal memory interfaces

## Connect streaming adapters

## Connect IRQ ports
add_instance kernel_irq altera_irq_bridge
set_instance_parameter_value kernel_irq "IRQ_WIDTH" "1"
set_instance_parameter_value kernel_irq "IRQ_N" "0"
add_interface kernel_irq irq sender
set_interface_property kernel_irq EXPORT_OF kernel_irq.sender0_irq
add_connection clk_1x.out_clk kernel_irq.clk
add_connection reset.out_reset kernel_irq.clk_reset

add_connection kernel_irq.receiver_irq HybridCPU_SignExtend_early_sys.kernel_irq

## Connect board hosts
add_instance kernel_cra altera_avalon_mm_bridge
set_instance_parameter_value kernel_cra "DATA_WIDTH" "64"
set_instance_parameter_value kernel_cra "SYMBOL_WIDTH" "8"
set_instance_parameter_value kernel_cra "ADDRESS_WIDTH" "30"
set_instance_parameter_value kernel_cra "USE_AUTO_ADDRESS_WIDTH" "0"
set_instance_parameter_value kernel_cra "ADDRESS_UNITS" "SYMBOLS"
set_instance_parameter_value kernel_cra "MAX_BURST_SIZE" "1"
set_instance_parameter_value kernel_cra "MAX_PENDING_RESPONSES" "1"
set_instance_parameter_value kernel_cra "LINEWRAPBURSTS" "0"
set_instance_parameter_value kernel_cra "PIPELINE_COMMAND" "0"
set_instance_parameter_value kernel_cra "PIPELINE_RESPONSE" "0"
add_connection clk_1x.out_clk kernel_cra.clk
add_connection reset.out_reset kernel_cra.reset
add_interface avs_kernel_cra avalon end
set_interface_property kernel_cra EXPORT_OF kernel_cra.s0

add_connection kernel_cra.m0 HybridCPU_SignExtend_early_sys.csr_ring_root_avs
set_connection_parameter_value kernel_cra.m0/HybridCPU_SignExtend_early_sys.csr_ring_root_avs baseAddress "0x0"


source kernel_system_import.tcl
