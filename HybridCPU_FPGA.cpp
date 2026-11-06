//#include <sycl/sycl.hpp>
//#include <sycl/ext/intel/fpga_extensions.hpp>
//#include <exception>
//#include <iostream>
//
//#include "CloseToHSL/Core/Execution/Scalar/ALU/ScalarAluOps.h"
//
//#if !defined(FPGA_EMULATOR) || !FPGA_EMULATOR
//#error This entry point requires FPGA_EMULATOR=1.
//#endif
//
//int main()
//{
//    try
//    {
//        sycl::queue queue{sycl::ext::intel::fpga_emulator_selector_v};
//        std::cout << "FPGA emulator device: "
//                  << queue.get_device().get_info<sycl::info::device::name>()
//                  << std::endl;
//
//        // The current C++ processor model runs on the host. Exercise one of
//        // its scalar ALU operations before submitting work to the emulator.
//        const unsigned long long aluResult =
//            YAKSys_Hybrid_CPU::Core::ScalarAluOps::SignExtendWord(0x80000000u);
//        unsigned long long emulatedResult = 0;
//        {
//            sycl::buffer<unsigned long long, 1> resultBuffer(&emulatedResult, sycl::range<1>{1});
//            queue.submit([&](sycl::handler& handler) {
//                sycl::accessor output{resultBuffer, handler, sycl::write_only, sycl::no_init};
//                handler.single_task([=] { output[0] = aluResult; });
//            });
//            queue.wait_and_throw();
//        }
//
//        std::cout << "HybridCPU scalar ALU SignExtendWord (host): 0x"
//                  << std::hex << aluResult << std::dec << std::endl;
//        if (emulatedResult != aluResult || aluResult != 0xffffffff80000000ULL)
//        {
//            std::cerr << "HybridCPU ALU result or FPGA emulator transfer failed." << std::endl;
//            return 1;
//        }
//        std::cout << "FPGA emulator kernel received the ALU result." << std::endl;
//        return 0;
//    }
//    catch (const sycl::exception& error)
//    {
//        std::cerr << "FPGA emulator error: " << error.what() << std::endl;
//    }
//    catch (const std::exception& error)
//    {
//        std::cerr << "HybridCPU startup error: " << error.what() << std::endl;
//    }
//    return 1;
//}












//////////////////////////////////////////////////////////////////////////////////////////////////////









// Standalone Intel oneAPI 2025.0 SYCL FPGA HLS proof-of-concept.
// This file does NOT compile the entire HybridCPU C++ runtime into hardware.
// It implements the same 32->64-bit sign-extension semantics as the C# and C++ ScalarAluOps.
#include <sycl/sycl.hpp>
#include <sycl/ext/intel/fpga_extensions.hpp>

#include <array>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>

namespace HybridCPU_HLS {
    // Unsigned bitwise implementation: no signed overflow or implementation-defined casts.
    inline std::uint64_t SignExtendWord(std::uint32_t value) {
        return static_cast<std::uint64_t>(value) |
            ((value & 0x80000000u) ? 0xffffffff00000000ULL : 0ULL);
    }
}

class HybridCPU_SignExtend_Kernel;

int main() {
    try {
#if defined(FPGA_HARDWARE) && FPGA_HARDWARE
        sycl::queue queue{ sycl::ext::intel::fpga_selector_v };
#else
        sycl::queue queue{ sycl::ext::intel::fpga_emulator_selector_v };
#endif
        constexpr std::size_t count = 8;
        std::array<std::uint32_t, count> inputs = {
            0x00000000u, 0x00000001u, 0x7fffffffu, 0x80000000u,
            0x80000001u, 0xffffffffu, 0xffff0000u, 0x00008000u
        };
        const std::array<std::uint64_t, count> expected = {
            0x0000000000000000ULL, 0x0000000000000001ULL,
            0x000000007fffffffULL, 0xffffffff80000000ULL,
            0xffffffff80000001ULL, 0xffffffffffffffffULL,
            0xffffffffffff0000ULL, 0x0000000000008000ULL
        };
        std::array<std::uint64_t, count> outputs{};
        {
            sycl::buffer<std::uint32_t, 1> input_buffer(inputs.data(), sycl::range<1>{count});
            sycl::buffer<std::uint64_t, 1> output_buffer(outputs.data(), sycl::range<1>{count});
            queue.submit([&](sycl::handler& handler) {
                sycl::accessor in{ input_buffer, handler, sycl::read_only };
                sycl::accessor out{ output_buffer, handler, sycl::write_only, sycl::no_init };
                handler.single_task<HybridCPU_SignExtend_Kernel>([=] {
                    for (int i = 0; i < 8; ++i) {
                        // Critical: computation is inside the FPGA device kernel.
                        out[i] = HybridCPU_HLS::SignExtendWord(in[i]);
                    }
                    });
                });
            queue.wait_and_throw();
        } // Buffer writeback happens when output_buffer is destroyed.

        unsigned failures = 0;
        for (std::size_t i = 0; i < count; ++i) {
            const bool passed = outputs[i] == expected[i];
            std::cout << (passed ? "PASS " : "FAIL ") << i
                << " input=0x" << std::hex << inputs[i]
                << " actual=0x" << outputs[i]
                << " expected=0x" << expected[i] << std::dec << '\n';
            failures += !passed;
        }
        std::cout << "HybridCPU HLS sign-extension tests: "
            << (count - failures) << "/" << count << " passed\n";
        return failures == 0 ? 0 : 1;
    }
    catch (const sycl::exception& e) {
        std::cerr << "SYCL failure: " << e.what() << '\n';
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Host failure: " << e.what() << '\n';
        return 1;
    }
}
