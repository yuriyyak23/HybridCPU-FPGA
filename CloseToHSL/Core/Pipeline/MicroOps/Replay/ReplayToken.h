#pragma once

#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <utility>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace HybridCPU_ISE::Core { class TraceEvent; }

namespace HybridCPU_ISE::Core
{
	/// <summary>
	/// Replay token for evidence-bounded execution reproduction and rollback support.
	/// </summary>
	class ReplayToken : public std::enable_shared_from_this<ReplayToken>
	{
	private:
		unsigned long long RandomSeed = 0;
		unsigned int VL = 0;
		unsigned int VLMAX = 0;
		unsigned char VSEW = 0;
		unsigned char LMUL = 0;
		bool TailAgnostic = false;
		bool MaskAgnostic = false;
		unsigned char RoundingMode = 0;
		unsigned char ExceptionMode = 0;
		unsigned long long MemorySize = 0;
		std::wstring TraceHash;
		long long Timestamp = 0;
		std::unordered_map<int, unsigned long long> PreExecutionRegisterState;
		int OwnerThreadId = 0;
		bool HasSideEffects = false;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private YAKSys_Hybrid_CPU.Processor.MainMemoryArea? _boundMainMemory;
		std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea> _boundMainMemory;

		// === Original replay fields ===
	public:
		unsigned long long getRandomSeed() const;
		void setRandomSeed(unsigned long long value);
		unsigned int getVL() const;
		void setVL(unsigned int value);
		unsigned int getVLMAX() const;
		void setVLMAX(unsigned int value);
		unsigned char getVSEW() const;
		void setVSEW(unsigned char value);
		unsigned char getLMUL() const;
		void setLMUL(unsigned char value);
		bool getTailAgnostic() const;
		void setTailAgnostic(bool value);
		bool getMaskAgnostic() const;
		void setMaskAgnostic(bool value);
		unsigned char getRoundingMode() const;
		void setRoundingMode(unsigned char value);
		unsigned char getExceptionMode() const;
		void setExceptionMode(unsigned char value);
		unsigned long long getMemorySize() const;
		void setMemorySize(unsigned long long value);
		std::wstring getTraceHash() const;
		void setTraceHash(const std::wstring &value);
		long long getTimestamp() const;
		void setTimestamp(long long value);

		// === Replay-envelope rollback support fields ===

		/// <summary>
		/// Pre-execution register state for rollback.
		/// Maps register ID to register value before operation execution.
		/// </summary>
		std::unordered_map<int, unsigned long long> getPreExecutionRegisterState() const;
		void setPreExecutionRegisterState(const std::unordered_map<int, unsigned long long> &value);

		/// <summary>
		/// Pre-execution memory state for rollback.
		/// Stores (Address, Data) tuples of memory contents before writes.
		/// </summary>
		    using MemorySnapshot = std::pair<unsigned long long, std::vector<unsigned char>>;
private:
    std::vector<MemorySnapshot> PreExecutionMemoryState;
public:
    ReplayToken();
    explicit ReplayToken(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea> mainMemory);
    std::vector<MemorySnapshot> getPreExecutionMemoryState() const;
    void setPreExecutionMemoryState(const std::vector<MemorySnapshot>& value);
    int getOwnerThreadId() const;
    void setOwnerThreadId(int value);
    bool getHasSideEffects() const;
    void setHasSideEffects(bool value);
    std::shared_ptr<ReplayToken> BindMainMemory(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea> mainMemory);
    void CaptureRegisterState(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::CPU_Core>& core, const std::vector<int>& registerIds);
    void CaptureMemoryState(unsigned long long address, int size);
    void Rollback(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::CPU_Core>& core);
    bool CanSafelyRollback() const;
    std::wstring ToJson() const;
    static std::shared_ptr<ReplayToken> FromJson(const std::wstring& json, std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea> mainMemory = nullptr);
    static std::wstring ComputeTraceHash(const std::vector<TraceEvent>& events);
    static std::shared_ptr<ReplayToken> CreateFromConfig(unsigned long long seed, unsigned int vl, unsigned int vlmax, unsigned char vsew, unsigned char lmul, bool tailAgnostic, bool maskAgnostic, unsigned char roundingMode, unsigned char exceptionMode, unsigned long long memorySize);
    bool MatchesConfig(const std::shared_ptr<ReplayToken>& other) const;
private:
    bool HasFullyBoundRollbackMemoryState() const;
    std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea> GetRequiredMainMemory(const std::wstring& operation) const;
    static bool HasExactMainMemoryRange(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea> memory, unsigned long long address, int size);
    static void ReadMainMemoryExact(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea> memory, unsigned long long address, std::vector<unsigned char>& buffer, const std::wstring& executionSurface);
    static void WriteMainMemoryExact(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea> memory, unsigned long long address, const std::vector<unsigned char>& buffer, const std::wstring& executionSurface);
    int ResolveOwnerThreadId(const std::shared_ptr<YAKSys_Hybrid_CPU::Processor::CPU_Core>& core) const;
};
}