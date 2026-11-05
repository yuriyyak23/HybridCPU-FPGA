#pragma once

#include "../Architecture/Registers/Architectural/ArchContextState.h"
#include "../Architecture/Registers/CSR/CsrFile.h"
#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <vector>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Reference-owned containment for committed ISA-visible core state.
	/// Publication authority remains with the existing retire, CSR, control-flow
	/// and context-transition owners.
	/// </summary>
	class ArchitecturalState final : public std::enable_shared_from_this<ArchitecturalState>
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal ArchContextCollection Contexts = null!;
	public:
		std::shared_ptr<ArchContextCollection> Contexts = nullptr;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal CsrFile Csr = null!;
		std::shared_ptr<CsrFile> Csr = nullptr;
		unsigned long long PodId = 0;
		unsigned long long PodAffinityMask = 0;
		unsigned long long MemoryDomainCertificate = 0;
		unsigned long long NocRouteConfiguration = 0;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal Processor.CPU_Core.FPExceptionContext[] FloatingPointContexts = null!;
		std::vector<Processor::CPU_Core::FPExceptionContext> FloatingPointContexts;
		Processor::CPU_Core::FlagsRegister CoreFlags;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal List<Processor.CPU_Core.FlagsRegister> FlagsContextStack = null!;
		std::vector<Processor::CPU_Core::FlagsRegister> FlagsContextStack;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal List<ulong> CallContextStack = null!;
		std::vector<unsigned long long> CallContextStack;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal List<ulong> InterruptContextStack = null!;
		std::vector<unsigned long long> InterruptContextStack;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal Processor.StackMemory Stack = null!;
		std::shared_ptr<Processor::StackMemory> Stack = nullptr;

		unsigned long long PredicateRegister0 = 0;
		unsigned long long PredicateRegister1 = 0;
		unsigned long long PredicateRegister2 = 0;
		unsigned long long PredicateRegister3 = 0;
		unsigned long long PredicateRegister4 = 0;
		unsigned long long PredicateRegister5 = 0;
		unsigned long long PredicateRegister6 = 0;
		unsigned long long PredicateRegister7 = 0;
		unsigned long long PredicateRegister8 = 0;
		unsigned long long PredicateRegister9 = 0;
		unsigned long long PredicateRegister10 = 0;
		unsigned long long PredicateRegister11 = 0;
		unsigned long long PredicateRegister12 = 0;
		unsigned long long PredicateRegister13 = 0;
		unsigned long long PredicateRegister14 = 0;
		unsigned long long PredicateRegister15 = 0;

		Processor::CPU_Core::RVV_Config VectorConfig;
		Processor::CPU_Core::VectorExceptionStatus VectorExceptionStatus;
		Processor::CPU_Core::VectorContext SavedVectorContext;
	};

}
