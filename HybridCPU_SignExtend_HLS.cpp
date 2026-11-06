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
        sycl::queue queue{sycl::ext::intel::fpga_selector_v};
#else
        sycl::queue queue{sycl::ext::intel::fpga_emulator_selector_v};
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
                sycl::accessor in{input_buffer, handler, sycl::read_only};
                sycl::accessor out{output_buffer, handler, sycl::write_only, sycl::no_init};
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
    } catch (const sycl::exception& e) {
        std::cerr << "SYCL failure: " << e.what() << '\n';
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Host failure: " << e.what() << '\n';
        return 1;
    }
}
