#include <string>
#include <vector>
#include <span>
#include <bit>
#include <cstdint>
#include <iostream>
#include "../../CloseToHSL/Core/Decoder/Sha256Digest.h"
namespace HybridCPU_ISE::Core {
struct TraceEvent { long long PC; int BundleId, OpIndex; unsigned int Opcode, ExceptionCount; };
struct ReplayToken { static std::wstring ComputeTraceHash(const std::vector<TraceEvent>&); };
    std::wstring ReplayToken::ComputeTraceHash(const std::vector<TraceEvent>& events) {
        if (events.empty()) return L"";
        std::string text;
        for (const auto& event : events) {
            text += std::to_string(event.PC); text += '|';
            text += std::to_string(event.BundleId); text += '|';
            text += std::to_string(event.OpIndex); text += '|';
            text += std::to_string(std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(event.Opcode))); text += '|';
            text += std::to_string(event.ExceptionCount); text += '\n';
        }
        return YAKSys_Hybrid_CPU::Core::Decoder::Sha256Hex(std::span<const unsigned char>(reinterpret_cast<const unsigned char*>(text.data()), text.size()));
    }

}
int main(){using namespace HybridCPU_ISE::Core; std::vector<TraceEvent> events{{-1,2,3,0xffffffffu,5},{123,7,8,9,10}}; std::wcout << ReplayToken::ComputeTraceHash(events);}
