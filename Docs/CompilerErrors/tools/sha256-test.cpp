#include "../../CloseToHSL/Core/Decoder/Sha256Digest.h"
#include <cassert>
#include <string>
using YAKSys_Hybrid_CPU::Core::Decoder::Sha256Hex;
int main(){ assert(Sha256Hex({})==L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"); const unsigned char a[]={'a','b','c'}; assert(Sha256Hex(a)==L"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"); }
