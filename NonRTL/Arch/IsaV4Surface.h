#pragma once

#include "Generated/GeneratedIsaShadowCatalog.g.h"
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <chrono>

using namespace YAKSys_Hybrid_CPU::Arch::Generated;

namespace YAKSys_Hybrid_CPU::Arch
{
	/// <summary>
	/// Compatibility façade for the HybridCPU ISA v4 static policy contour.
	/// The committed generated C# catalog is projected from typed registry rows;
	/// this type preserves its established consumer-facing API until the RF-13
	/// legacy surface removal gate closes. Runtime support remains evidence owned by
	/// <see cref="InstructionSupportStatusCatalog"/>, never by this declaration.
	/// </summary>
	class IsaV4Surface final
	{
	private:
		static std::shared_ptr<const std::unordered_set<std::wstring>> Policy(const std::wstring &policyId);

	public:
		static inline const std::shared_ptr<const std::unordered_set<std::wstring>> MandatoryCoreClasses = Policy(L"MandatoryCoreClasses");
		static inline const std::shared_ptr<const std::unordered_set<std::wstring>> MandatoryCoreOpcodes = Policy(L"MandatoryCoreOpcodes");
		static inline const std::shared_ptr<const std::unordered_set<std::wstring>> SystemDeviceCommandOpcodes = Policy(L"SystemDeviceCommandOpcodes");
		static inline const std::shared_ptr<const std::unordered_set<std::wstring>> CarrierOnlyOpcodes = Policy(L"CarrierOnlyOpcodes");
		static inline const std::shared_ptr<const std::unordered_set<std::wstring>> MandatoryInteger64RepairOpcodes = Policy(L"MandatoryInteger64RepairOpcodes");
		static inline const std::shared_ptr<const std::unordered_set<std::wstring>> DescriptorOnlyOpcodes = Policy(L"DescriptorOnlyOpcodes");
		static inline const std::shared_ptr<const std::unordered_set<std::wstring>> ParserOnlyOpcodes = Policy(L"ParserOnlyOpcodes");
		static inline const std::shared_ptr<const std::unordered_set<std::wstring>> OptionalEnabledOpcodes = Policy(L"OptionalEnabledOpcodes");
		static inline const std::shared_ptr<const std::unordered_set<std::wstring>> OptionalDisabledOpcodes = Policy(L"OptionalDisabledOpcodes");
		static inline const std::shared_ptr<const std::unordered_set<std::wstring>> ReservedOpcodes = Policy(L"ReservedOpcodes");
		static inline const std::shared_ptr<const std::unordered_set<std::wstring>> ProhibitedOpcodes = Policy(L"ProhibitedOpcodes");
		static inline const std::shared_ptr<const std::unordered_set<std::wstring>> OptionalExtensions = Policy(L"OptionalExtensions");
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static readonly IReadOnlyDictionary<string, string> PipelineClassMap = GeneratedIsaCatalog.PipelineClassMap;
		static inline const std::unordered_map<std::wstring, std::wstring> &PipelineClassMap = GeneratedIsaCatalog::PipelineClassMap;

		/// <summary>HybridCPU ISA v4 is frozen; evolution requires a new catalog version.</summary>
		static constexpr int IsaVersion = 4;

		/// <summary>Frozen v4 mandatory hardware-opcode count.</summary>
		static constexpr int IsaMandatoryOpcodeCount = 111;

		/// <summary>Date on which the ISA v4 surface was formally frozen.</summary>
		static inline constexpr std::chrono::year_month_day FrozenDate{
			std::chrono::year{2026}, std::chrono::month{3}, std::chrono::day{14}};
	};
}


