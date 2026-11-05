#include "../../CloseToHSL/Core/Decoder/CanonicalPayloadSnapshot.h"
#include <cassert>
#include <stdexcept>
using YAKSys_Hybrid_CPU::Core::Decoder::CanonicalPayloadSnapshot;
int main(){ unsigned char data[]={'a','b','c'}; auto x=CanonicalPayloadSnapshot::FromBytes(L"kind",data); auto y=CanonicalPayloadSnapshot::FromBytes(L"kind",data); data[0]='z'; assert(x->getContent()[0]=='a'); auto bytes=x->getContent(); bytes[0]='x'; assert(x->getContent()[0]=='a'); assert(x->getContentSha256()==L"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"); assert(x->Equals(y) && x->Equals(std::any(y)) && x->GetHashCode()==y->GetHashCode()); assert(!x->Equals(std::any(1))); try {CanonicalPayloadSnapshot::FromBytes(L" \t",data); assert(false);} catch(const std::invalid_argument&){} }
