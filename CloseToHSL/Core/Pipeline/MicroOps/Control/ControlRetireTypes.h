#pragma once
#include "../../../Diagnostics/TraceEventKind.h"

namespace YAKSys_Hybrid_CPU::Core
{
	enum class CsrStorageSurface : unsigned char
	{
		None = 0,
		VectorPodPlane = 1,
		WiredCsrFile = 2
	};

	/// <summary>
	/// Typed CSR retire payload carried by EX/MEM/WB lane state.
	/// Separates CSR read register-writeback from deferred CSR side effects.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct CsrRetireEffect
	class CsrRetireEffect
	{
	private:
		const bool ClearsArchitecturalExceptionState;
		const CsrStorageSurface StorageSurface;
		const unsigned short CsrAddress;
		const unsigned long long ReadValue;
		const bool HasRegisterWriteback;
		const unsigned short DestRegId;
		const bool HasCsrWrite;
		const unsigned long long CsrWriteValue;

		CsrRetireEffect(bool clearsArchitecturalExceptionState, CsrStorageSurface storageSurface, unsigned short csrAddress, unsigned long long readValue, bool hasRegisterWriteback, unsigned short destRegId, bool hasCsrWrite, unsigned long long csrWriteValue);

	public:
		bool getClearsArchitecturalExceptionState() const;

		CsrStorageSurface getStorageSurface() const;

		unsigned short getCsrAddress() const;

		unsigned long long getReadValue() const;

		bool getHasRegisterWriteback() const;

		unsigned short getDestRegId() const;

		bool getHasCsrWrite() const;

		unsigned long long getCsrWriteValue() const;

		static CsrRetireEffect ClearExceptionCounters();

		static CsrRetireEffect Create(CsrStorageSurface storageSurface, unsigned short csrAddress, unsigned long long readValue, bool hasRegisterWriteback, unsigned short destRegId, bool hasCsrWrite, unsigned long long csrWriteValue);

		CsrRetireEffect() = default;
	};

	/// <summary>
	/// Typed Atomic retire payload carried by EX/MEM/WB lane state and by
	/// direct compat retire transactions.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct AtomicRetireEffect
	class AtomicRetireEffect
	{
	private:
		const bool IsValid;
		const unsigned short Opcode;
		const unsigned char AccessSize;
		const unsigned long long Address;
		const unsigned long long SourceValue;
		const unsigned short DestinationRegister;
		const unsigned short CoreId;
		const int VirtualThreadId;
		const bool AcquireOrdering;
		const bool ReleaseOrdering;

		AtomicRetireEffect(bool isValid, unsigned short opcode, unsigned char accessSize, unsigned long long address, unsigned long long sourceValue, unsigned short destinationRegister, unsigned short coreId, int virtualThreadId, bool acquireOrdering, bool releaseOrdering);

	public:
		bool isValid() const;

		unsigned short getOpcode() const;

		unsigned char getAccessSize() const;

		unsigned long long getAddress() const;

		unsigned long long getSourceValue() const;

		unsigned short getDestinationRegister() const;

		unsigned short getCoreId() const;

		int getVirtualThreadId() const;

		bool getAcquireOrdering() const;

		bool getReleaseOrdering() const;

		bool getHasRegisterDestination() const;

		static AtomicRetireEffect Create(unsigned short opcode, unsigned char accessSize, unsigned long long address, unsigned long long sourceValue, unsigned short destinationRegister, unsigned short coreId, int virtualThreadId, bool acquireOrdering = false, bool releaseOrdering = false);

		AtomicRetireEffect();
	};

	/// <summary>
	/// Atomic retire/apply outcome materialized only at retire.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct AtomicRetireOutcome
	class AtomicRetireOutcome
	{
	private:
		const bool HasRegisterWriteback;
		const unsigned short RegisterDestination;
		const unsigned long long RegisterWritebackValue;
		const bool MemoryMutated;
		const YAKSys_Hybrid_CPU::Core::TraceEventKind TraceEventKind;

		AtomicRetireOutcome(bool hasRegisterWriteback, unsigned short registerDestination, unsigned long long registerWritebackValue, bool memoryMutated, YAKSys_Hybrid_CPU::Core::TraceEventKind traceEventKind);

	public:
		bool getHasRegisterWriteback() const;

		unsigned short getRegisterDestination() const;

		unsigned long long getRegisterWritebackValue() const;

		bool getMemoryMutated() const;

		YAKSys_Hybrid_CPU::Core::TraceEventKind getTraceEventKind() const;

		static AtomicRetireOutcome Create(AtomicRetireEffect const effect, unsigned long long registerWritebackValue, bool hasRegisterWriteback, bool memoryMutated);

	private:
		static YAKSys_Hybrid_CPU::Core::TraceEventKind ClassifyTraceEventKind(AtomicRetireEffect const effect, unsigned long long registerWritebackValue);

	public:
		AtomicRetireOutcome()
			: HasRegisterWriteback(false), RegisterDestination(0), RegisterWritebackValue(0), MemoryMutated(false), TraceEventKind(static_cast<YAKSys_Hybrid_CPU::Core::TraceEventKind>(0)) {}
	};

}
