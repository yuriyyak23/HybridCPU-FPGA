#include "../../CloseToHSL/Core/ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileLayoutPolicyAbi.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile;
int main() {
    std::optional<std::shared_ptr<MatrixTileLayoutPolicy>> missing;
    assert(MatrixTileLayoutPolicyAbi::Validate(missing, MatrixTileProjectedOperationKind::Macc)->FaultKind == MatrixTileLayoutPolicyFaultKind::MissingPolicy);
    auto policy = MatrixTileLayoutPolicyAbi::CreateMaccPolicy();
    std::optional<std::shared_ptr<MatrixTileLayoutPolicy>> wrapped = policy;
    assert(MatrixTileLayoutPolicyAbi::Validate(wrapped, MatrixTileProjectedOperationKind::Macc)->IsValid);
    assert(MatrixTileLayoutPolicyAbi::Validate(wrapped, MatrixTileProjectedOperationKind::Transpose)->FaultKind == MatrixTileLayoutPolicyFaultKind::UnsupportedOperation);
    policy->Fingerprint ^= 1;
    assert(MatrixTileLayoutPolicyAbi::Validate(wrapped, MatrixTileProjectedOperationKind::Macc)->FaultKind == MatrixTileLayoutPolicyFaultKind::FingerprintMismatch);
}
