#include "../../CloseToHSL/Core/Decoder/ExecutionBindingIdentity.h"
#include <iostream>
int main() {
 using namespace YAKSys_Hybrid_CPU::Arch::Generated;
 auto descriptor = *GeneratedIsaCatalog::Descriptors.at(0);
 std::wcout << YAKSys_Hybrid_CPU::Core::Decoder::GeneratedStaticBinding::FromDescriptor(descriptor)->DescriptorFingerprint;
}
namespace YAKSys_Hybrid_CPU::Arch::Generated {
const std::unordered_map<std::wstring, std::unordered_set<std::wstring>> GeneratedIsaCatalog::CreateStaticPolicies() { return {}; }
const std::unordered_map<std::wstring, std::wstring> GeneratedIsaCatalog::CreatePipelineClassMap() { return {}; }
std::vector<YAKSys_Hybrid_CPU::Arch::OpcodeInfo> GeneratedIsaCatalog::CreateOpcodeInfos() { return {}; }
}