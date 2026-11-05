#include "../../CloseToHSL/Core/Decoder/CanonicalDecodeContext.h"
#include <cassert>
#include <type_traits>
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
int main() { CanonicalDecodeContext c; c.ManifestVersion=L"4"; c.ManifestHash=L"hash"; c.DecoderEpoch=L"epoch"; assert(c.ExecutableMemoryInvalidationEpoch==0 && c.CodeGenerationEpoch==0 && !c.IsReplayEligible); auto copy=c; assert(copy.ManifestVersion==L"4" && copy.ManifestHash==L"hash" && copy.DecoderEpoch==L"epoch"); }
