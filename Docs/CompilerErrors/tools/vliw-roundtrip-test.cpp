#include "../../NonRTL/Arch/Compat/VLIW_Bundle.h"
#include "../../NonRTL/Arch/InvalidOpcodeException.h"
#include "../../tangible_exception_helper.h"
#include <array>
#include <cassert>
#include <stdexcept>
#include <string>
using namespace HybridCPU_ISE::Arch;
using YAKSys_Hybrid_CPU::Arch::InvalidOpcodeException;
int main() {
 VLIW_Instruction a; a.setWord0(0x1234000012345678ULL); a.setWord1(0x1122334455667788ULL); a.setWord2(0xfedcba9876543210ULL); a.setWord3(0x0000000011223344ULL);
 std::array<unsigned char,40> bytes{}; assert(!a.TryWriteBytes(std::span<unsigned char>(bytes).first(31))); assert(a.TryWriteBytes(bytes)); assert(bytes[8]==0x88 && bytes[15]==0x11);
 VLIW_Instruction b; assert(b.TryReadBytes(bytes)); assert(b.getWord0()==a.getWord0() && b.getWord1()==a.getWord1() && b.getWord2()==a.getWord2() && b.getWord3()==a.getWord3());
 assert(!b.TryReadBytes(std::span<const unsigned char>(bytes).first(31))); try { b.TryReadBytes(bytes,-1); assert(false); } catch(const std::out_of_range&) {}
 auto bad=bytes; bad[5]=1; try { b.TryReadBytes(bad); assert(false); } catch(const InvalidOpcodeException&) {}
 unsigned char rd,rs1,rs2; auto packed=VLIW_Instruction::PackArchRegs(0,31,VLIW_Instruction::NoArchReg); assert(VLIW_Instruction::TryUnpackArchRegs(packed,rd,rs1,rs2) && rd==0 && rs1==31 && rs2==VLIW_Instruction::NoArchReg);
 try { VLIW_Instruction::PackArchRegs(32,0,0); assert(false); } catch(const ArgumentOutOfRangeException&) {} assert(!VLIW_Instruction::TryUnpackArchRegs(32,rd,rs1,rs2));
 VLIW_Bundle bundle; bundle.SetInstruction(3,a); std::array<unsigned char,256> data{}; assert(bundle.TryWriteBytes(data)); VLIW_Bundle restored; assert(restored.TryReadBytes(data)); assert(restored.GetInstruction(3).getWord1()==a.getWord1());
 assert(!bundle.TryWriteBytes(std::span<unsigned char>(data).first(255))); assert(!restored.TryReadBytes(std::span<const unsigned char>(data).first(255)));
 InvalidOpcodeException e(L"ошибка",std::runtime_error("inner")); assert(e.getInnerException()); try {std::rethrow_exception(e.getInnerException());} catch(const std::runtime_error &inner){assert(std::string(inner.what())=="inner");}
}
