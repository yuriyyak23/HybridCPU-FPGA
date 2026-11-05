#pragma once
namespace YAKSys_Hybrid_CPU::Core::VmxFrozenOpcodeValues {
    inline constexpr unsigned short VMXON = 250;
    inline constexpr unsigned short VMXOFF = 251;
    inline constexpr unsigned short VMLAUNCH = 252;
    inline constexpr unsigned short VMRESUME = 253;
    inline constexpr unsigned short VMREAD = 254;
    inline constexpr unsigned short VMWRITE = 255;
    inline constexpr unsigned short VMCLEAR = 256;
    inline constexpr unsigned short VMPTRLD = 257;
    inline constexpr unsigned short VMPTRST = 258;
    inline constexpr unsigned short VMCALL = 259;
    inline constexpr unsigned short INVEPT = 267;
    inline constexpr unsigned short INVVPID = 268;
    inline constexpr unsigned short VMFUNC = 269;
    inline constexpr unsigned short VMSAVEX = 270;
    inline constexpr unsigned short VMRESTX = 271;
}
