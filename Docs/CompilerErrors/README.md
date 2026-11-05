# C# → C++ porting workspace: conversion overview

This folder contains the working record and small diagnostic tools used while porting the HybridCPU / HybridCPU ISE codebase from C# to C++. The application sources are in the workspace root (`../`); this folder is not part of the application binary and its probe programs are not FPGA kernels.

## Project context

- Target toolchain: Intel oneAPI DPC++/C++ Compiler (`icx-cl`) with Clang, SYCL, and Intel FPGA extensions. Do not assume a normal MSVC build just because Visual Studio drives the project.
- `FPGA_EMULATOR` selects the FPGA emulator path. A successful emulator run verifies host setup and the submitted SYCL kernel in emulation; it does not mean an FPGA hardware bitstream was produced.
- The main application is configured through `HybridCPU_FPGA.vcxproj`. The `CompilerErrors/tools` projects and `.cpp` probes are isolated diagnostics, not the main application.

## Folder map

| Path | Contents |
| --- | --- |
| `tools/` | Historical PowerShell repair/build scripts, MSBuild `.targets`, standalone C++ probes, and helper projects. |
| `tools/binding-csharp-check/` | Small C# diagnostic project retained for comparing/checking binding-related behavior. |
| `docs/notes/` | Design and porting notes grouped by subsystem. |
| `docs/compiler-errors/` | Captured compiler-error inputs used for iterative repair. |
| `docs/snapshots/` | Before/ original source snapshots preserved for reference or recovery. |
| `docs/build-environment/` | Visual Studio/MSBuild manifests, path maps, and environment diagnostics. |
| `docs/conversion-history/` | Detailed historical repair log and prior iteration notes. |
| `../legacy/` | Legacy source subtree formerly under `2\Legacy`; merged here because Windows treats `Legacy` and `legacy` as the same directory name. |

## Porting approach

The conversion work keeps the original behavior and ownership model where possible, replacing C# features with explicit C++ equivalents: `shared_ptr`/`optional` where ownership or absence is part of the contract, constructor initializer lists for immutable members, standard algorithms for LINQ-like operations, and explicit string/enum handling. Compile errors should be fixed at their primary cause; downstream diagnostics often disappear with it. Avoid empty implementations or permissive fallbacks that alter semantics.

## Working with the tools

Many scripts and helper `.targets` files were created for focused iterations. Some are historical and contain old absolute paths or assumptions about a previous workspace layout. Inspect a script before running it and update its project/compiler paths for the current checkout. `.targets` files are used by selected scripts to restrict `ClCompile` to a small subset; they are not application runtime configuration. Probe `.cpp` files are separate checks and are not automatically included in the main project.

The detailed historical record is in [`docs/conversion-history/Detailed-Fixes.md`](docs/conversion-history/Detailed-Fixes.md). No full solution build or tests are implied by this archive.