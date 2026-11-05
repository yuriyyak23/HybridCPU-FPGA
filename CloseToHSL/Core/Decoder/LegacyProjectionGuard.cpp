#include "LegacyAdmissionProjection.h"
#include "Sha256Digest.h"
#include <stdexcept>
#include <string>

namespace YAKSys_Hybrid_CPU::Core::Decoder {
namespace {
std::string ClassName(SlotClass c) {
    switch(c) {case SlotClass::AluClass:return "AluClass";case SlotClass::LsuClass:return "LsuClass";case SlotClass::DmaStreamClass:return "DmaStreamClass";case SlotClass::BranchControl:return "BranchControl";case SlotClass::SystemSingleton:return "SystemSingleton";case SlotClass::MatrixTileStreamClass:return "MatrixTileStreamClass";case SlotClass::Unclassified:return "Unclassified";default:return std::to_string(static_cast<unsigned char>(c));}
}
std::string PinName(SlotPinningKind p) {switch(p){case SlotPinningKind::ClassFlexible:return "ClassFlexible";case SlotPinningKind::HardPinned:return "HardPinned";default:return std::to_string(static_cast<unsigned char>(p));}}
std::string Registers(const std::vector<int>& xs) {std::string result; for(auto x:xs){if(!result.empty())result+=',';result+=std::to_string(x);}return result;}
}
std::shared_ptr<AdmissionProjectionFingerprint> LegacyProjectionGuard::Capture(std::shared_ptr<LegacyAdmissionProjection> p) {
    if(!p || !p->Placement) throw std::invalid_argument("projection and placement are required");
    const auto &v=*p->Placement;
    auto text=std::to_string(p->VirtualThreadId)+"|"+std::to_string(p->OwnerContextId)+"|"+std::to_string(p->DomainTag)+"|"+(p->IsStealable?"True":"False")+"|"+Registers(p->ReadRegisters)+"|"+Registers(p->WriteRegisters)+"|"+ClassName(v.RequiredSlotClass)+"|"+PinName(v.PinningKind)+"|"+std::to_string(v.PinnedLaneId)+"|"+std::to_string(v.DomainTag)+"|"+std::to_string(p->ResourceMask.Low)+"|"+std::to_string(p->ResourceMask.High);
    auto bytes=std::span<const unsigned char>(reinterpret_cast<const unsigned char*>(text.data()),text.size());
    return std::make_shared<AdmissionProjectionFingerprint>(Sha256Hex(bytes));
}
void LegacyProjectionGuard::EnsureCurrent(std::shared_ptr<AdmissionProjectionFingerprint> expected, std::shared_ptr<LegacyAdmissionProjection> current) {
    if(!expected) throw std::invalid_argument("expected fingerprint is required");
    if(Capture(current)->Value != expected->Value) throw std::runtime_error("Legacy admission projection is stale after a scheduling-relevant carrier mutation.");
}
}
