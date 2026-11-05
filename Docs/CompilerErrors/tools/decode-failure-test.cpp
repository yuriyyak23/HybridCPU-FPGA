#include "../../CloseToHSL/Core/Decoder/DecodeFailure.h"
#include <cassert>
#include <stdexcept>
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
int main(){ const unsigned char bytes[]={'a','b','c'}; auto f=DecodeFailure::Create(DecodeFailureCode::OperandEncoding,3,L"opcode",bytes,L"invalid"); assert(f->Code==DecodeFailureCode::OperandEncoding && f->SlotIndex==3 && f->Field==L"opcode" && f->Message==L"invalid"); assert(f->RawHash==L"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"); try{DecodeFailure::Create(DecodeFailureCode::Unknown,0,L" ",bytes,L"x");assert(false);}catch(const std::invalid_argument&){} }
