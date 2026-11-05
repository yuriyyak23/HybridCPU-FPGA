#include "AcceleratorTokenStatusWord.h"
#include "AcceleratorToken.h"
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens {
 AcceleratorTokenStatusWord AcceleratorTokenStatusWord::FromToken(const AcceleratorToken& token) {
  auto flags = token.isTerminal() ? AcceleratorTokenStatusFlags::Terminal | AcceleratorTokenStatusFlags::ModelOnly : AcceleratorTokenStatusFlags::ModelOnly;
  return {token.getState(), token.getFaultCode(), flags, token.getStatusSequence()};
 }
}
