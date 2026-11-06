# HybridCPU-v2 FPGA

**Experimental C++ port of [HybridCPU-v2](https://github.com/yuriyyak23/HybridCPU-v2), targeting Intel oneAPI DPC++ and FPGA development.**

## Project Status

**Early-stage draft / work in progress.**

This repository contains a near 1:1 translation of the original C# HybridCPU processor model into C++20. The primary goal is to preserve the original architecture, behavior, and code structure before further refactoring for FPGA hardware synthesis.

### Current Progress

- C# → C++20 source code migration.
- Processor architecture, ISA, execution pipeline, ALU, memory, and related subsystems ported.
- Successful C++ compilation without build errors in the tested configuration.
- Executable successfully launched using Intel FPGA Emulation.
- Basic SYCL kernel submission, execution, and host/device data transfer verified.
- Initial FPGA HLS experiment successfully generated SystemVerilog RTL for a standalone `SignExtendWord` kernel.

## Development Environment

| Component | Version |
|---|---|
| Operating System | Windows x64 |
| IDE | Visual Studio 2022 |
| C++ Standard | C++20 |
| Intel oneAPI Base Toolkit | 2025 |
| Intel oneAPI DPC++/C++ Compiler | 2025.0.0 (Build 20241008) |
| Intel FPGA Support Package | 2025.0.0 |
| Build Configuration | Debug / x64 |
| Execution Target | Intel FPGA Emulator |

## Verified FPGA Emulation

The main application builds and runs successfully:

```text
FPGA emulator device: Intel(R) FPGA Emulation Device
HybridCPU scalar ALU SignExtendWord (host): 0xffffffff80000000
FPGA emulator kernel received the ALU result.
```

**Important:** The current main application executes the processor model and scalar ALU operation on the host CPU. The SYCL kernel transfers the computed result through the FPGA Emulator.

FPGA Emulation runs device code on the host CPU. It does not execute on physical FPGA hardware or imply that the complete processor has been synthesized into RTL.

Standalone HLS experiments are separate from the main processor implementation.

## Limitations

This is a preliminary C++ port, not a production-ready FPGA or ASIC implementation.

The successful build and emulator execution confirm the tested software configuration only. Full processor functionality, hardware synthesizability, FPGA timing, and ASIC compatibility have not been validated.

Further work will focus on device-compatible C++ refactoring, FPGA HLS, RTL generation, and hardware verification.

## Original Project

[HybridCPU-v2 — Original C# Implementation](https://github.com/yuriyyak23/HybridCPU-v2)