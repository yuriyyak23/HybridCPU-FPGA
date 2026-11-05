#pragma once
#include <limits>
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Core::Decoder {
// A monotonic issuer owned by the scheduling path. It is not thread-safe,
// matching the unsynchronized C# implementation.
class OperationAttemptIssuer final {
    unsigned long long next_ = 0;
public:
    unsigned long long IssueNextAttempt() {
        if (next_ == (std::numeric_limits<unsigned long long>::max)())
            throw std::overflow_error("Operation attempt counter overflow");
        return ++next_;
    }
};
}
