#include "../../CloseToHSL/Core/Decoder/SemanticInstructionKey.h"
#include <cassert>
#include <stdexcept>
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
int main(){ auto c=std::make_shared<CanonicalDecodeContext>(); c->ManifestVersion=c->ManifestHash=c->ExtensionConfigurationFingerprint=c->DecoderEpoch=c->DecoderVersion=c->PrivilegeContext=c->DomainIdentity=c->AddressSpaceIdentity=c->VectorConfigurationFingerprint=L"v"; unsigned char data[]={1,2,3}; auto a=SemanticInstructionKey::Create(data,L"ann",c); auto b=SemanticInstructionKey::Create(data,L"ann",c); assert(a==b && a.GetHashCode()==b.GetHashCode() && a.Equals(std::any(b))); data[0]=9; c->DomainIdentity=L"changed"; assert(a==b && a.getRawBundleBytes()[0]==1); auto changed=SemanticInstructionKey::Create(data,L"ann",c); assert(a!=changed); c->DomainIdentity=L""; try {SemanticInstructionKey::Create(data,L"ann",c); assert(false);} catch(const std::invalid_argument&){} }
