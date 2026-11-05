#include <sycl/sycl.hpp>
#include <sycl/ext/intel/fpga_extensions.hpp>
#include <exception>
#include <iostream>

#include "CloseToHSL/Core/Execution/Scalar/ALU/ScalarAluOps.h"

#if !defined(FPGA_EMULATOR) || !FPGA_EMULATOR
#error This entry point requires FPGA_EMULATOR=1.
#endif

int main()
{
    try
    {
        sycl::queue queue{sycl::ext::intel::fpga_emulator_selector_v};
        std::cout << "FPGA emulator device: "
                  << queue.get_device().get_info<sycl::info::device::name>()
                  << std::endl;

        // The current C++ processor model runs on the host. Exercise one of
        // its scalar ALU operations before submitting work to the emulator.
        const unsigned long long aluResult =
            YAKSys_Hybrid_CPU::Core::ScalarAluOps::SignExtendWord(0x80000000u);
        unsigned long long emulatedResult = 0;
        {
            sycl::buffer<unsigned long long, 1> resultBuffer(&emulatedResult, sycl::range<1>{1});
            queue.submit([&](sycl::handler& handler) {
                sycl::accessor output{resultBuffer, handler, sycl::write_only, sycl::no_init};
                handler.single_task([=] { output[0] = aluResult; });
            });
            queue.wait_and_throw();
        }

        std::cout << "HybridCPU scalar ALU SignExtendWord (host): 0x"
                  << std::hex << aluResult << std::dec << std::endl;
        if (emulatedResult != aluResult || aluResult != 0xffffffff80000000ULL)
        {
            std::cerr << "HybridCPU ALU result or FPGA emulator transfer failed." << std::endl;
            return 1;
        }
        std::cout << "FPGA emulator kernel received the ALU result." << std::endl;
        return 0;
    }
    catch (const sycl::exception& error)
    {
        std::cerr << "FPGA emulator error: " << error.what() << std::endl;
    }
    catch (const std::exception& error)
    {
        std::cerr << "HybridCPU startup error: " << error.what() << std::endl;
    }
    return 1;
}
