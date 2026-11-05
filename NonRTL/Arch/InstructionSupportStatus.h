#pragma once

#include "../../CloseToHSL/Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "OpcodeInfo.Types.h"
#include <string>
#include <unordered_map>
#include <optional>
#include <vector>
#include <stdexcept>
#include <memory>
#include "../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Arch
{
	enum class IsaInstructionStatus : unsigned char
	{
		Mandatory = 0,
		OptionalEnabled = 1,
		OptionalDisabled = 2,
		Reserved = 3,
		LegacyRetained = 4,
		ParserOnly = 5,
		DescriptorOnly = 6,
		Prohibited = 7,
		CarrierOnly = 8
	};

	enum class RuntimeInstructionEvidence : unsigned char
	{
		None = 0,
		DeclaredOnly = 1,
		DecoderAccepted = 2,
		DescriptorProjected = 3,
		Materialized = 4,
		Executable = 5,
		RetireVisible = 6,
		ConformanceTested = 7
	};

	// Runtime-owned declaration/evidence record.  This is observational data only;
	// decoder, scheduler and execution paths remain the authorities for legality.
	struct InstructionSupportStatus final
	{
		const std::wstring Mnemonic;
		const IsaInstructionStatus Status;
		const RuntimeInstructionEvidence RuntimeEvidence;
		const std::wstring ExtensionName;
		const bool HasNumericOpcode;
		const bool HasRuntimeOpcodeMetadata;
		const bool HasCanonicalDecoderAcceptance;
		const bool HasRegistryFactory;
		const bool HasExecutionSemantics;

		InstructionSupportStatus(
			const std::wstring &mnemonic,
			IsaInstructionStatus status,
			RuntimeInstructionEvidence runtimeEvidence,
			const std::wstring &extensionName = L"",
			bool hasNumericOpcode = false,
			bool hasRuntimeOpcodeMetadata = false,
			bool hasCanonicalDecoderAcceptance = false,
			bool hasRegistryFactory = false,
			bool hasExecutionSemantics = false)
			: Mnemonic(mnemonic),
			  Status(status),
			  RuntimeEvidence(runtimeEvidence),
			  ExtensionName(extensionName),
			  HasNumericOpcode(hasNumericOpcode),
			  HasRuntimeOpcodeMetadata(hasRuntimeOpcodeMetadata),
			  HasCanonicalDecoderAcceptance(hasCanonicalDecoderAcceptance),
			  HasRegistryFactory(hasRegistryFactory),
			  HasExecutionSemantics(hasExecutionSemantics)
		{
			if (Mnemonic.empty() || StringHelper::isEmptyOrWhiteSpace(Mnemonic))
			{
				throw std::invalid_argument("Instruction mnemonic must be non-empty.");
			}
		}

		bool getIsExecutableClaim() const
		{
			return (Status == IsaInstructionStatus::Mandatory || Status == IsaInstructionStatus::OptionalEnabled)
				&& RuntimeEvidence >= RuntimeInstructionEvidence::Executable
				&& HasExecutionSemantics;
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct InstructionSupportStatus
//	{
//		public InstructionSupportStatus(string mnemonic, IsaInstructionStatus status, RuntimeInstructionEvidence runtimeEvidence, string extensionName = "", bool hasNumericOpcode = false, bool hasRuntimeOpcodeMetadata = false, bool hasCanonicalDecoderAcceptance = false, bool hasRegistryFactory = false, bool hasExecutionSemantics = false)
//		{
//			Mnemonic = string.IsNullOrWhiteSpace(mnemonic) ? throw new ArgumentException("Instruction mnemonic must be non-empty.", nameof(mnemonic)) : mnemonic;
//			Status = status;
//			RuntimeEvidence = runtimeEvidence;
//			ExtensionName = extensionName ?? string.Empty;
//			HasNumericOpcode = hasNumericOpcode;
//			HasRuntimeOpcodeMetadata = hasRuntimeOpcodeMetadata;
//			HasCanonicalDecoderAcceptance = hasCanonicalDecoderAcceptance;
//			HasRegistryFactory = hasRegistryFactory;
//			HasExecutionSemantics = hasExecutionSemantics;
//		}
//
//		public string Mnemonic
//		{
//			get;
//		}
//
//		public IsaInstructionStatus Status
//		{
//			get;
//		}
//
//		public RuntimeInstructionEvidence RuntimeEvidence
//		{
//			get;
//		}
//
//		public string ExtensionName
//		{
//			get;
//		}
//
//		public bool HasNumericOpcode
//		{
//			get;
//		}
//
//		public bool HasRuntimeOpcodeMetadata
//		{
//			get;
//		}
//
//		public bool HasCanonicalDecoderAcceptance
//		{
//			get;
//		}
//
//		public bool HasRegistryFactory
//		{
//			get;
//		}
//
//		public bool HasExecutionSemantics
//		{
//			get;
//		}
//
//		public bool IsExecutableClaim
//		{
//			get
//			{
//				return Status is IsaInstructionStatus.Mandatory or IsaInstructionStatus.OptionalEnabled && RuntimeEvidence >= RuntimeInstructionEvidence.Executable && HasExecutionSemantics;
//			}
//		}
//	}

	/// <summary>
	/// Versioned, read-only projection of declared instruction-support evidence.
	/// It intentionally reports observations only: decoder, scheduler and runtime
	/// execution continue to own legality and execution authority.
	/// </summary>
	class InstructionSupportEvidenceReport final : public std::enable_shared_from_this<InstructionSupportEvidenceReport>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<InstructionSupportStatus> Rows;
		const std::vector<std::shared_ptr<InstructionSupportStatus>> Rows;

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal InstructionSupportEvidenceReport(IReadOnlyList<InstructionSupportStatus> rows)
		InstructionSupportEvidenceReport(const std::vector<std::shared_ptr<InstructionSupportStatus>> &rows);

		static constexpr int SchemaVersion = 1;

		int getVersion() const;

		std::wstring getKind() const;

		bool isRuntimeAuthority() const;

		const std::vector<std::shared_ptr<InstructionSupportStatus>> getRows() const;
	};

	/// <summary>
	/// Runtime-owned support terminology for the instruction refactor inventory.
	/// This catalog is a declaration/evidence surface only; it is not a legality
	/// service and does not participate in decoder, scheduler, or execution authority.
	/// </summary>
	class InstructionSupportStatusCatalog final
	{
	private:
	public:
		static const std::vector<std::wstring> getMandatoryInteger64RepairMnemonics();

		static const std::vector<std::shared_ptr<InstructionSupportStatus>> getExplicitStatuses();

		/// <summary>
		/// Returns the versioned, generated-from-catalog evidence projection.
		/// The report is not a runtime support, legality or execution authority.
		/// </summary>
		static std::shared_ptr<InstructionSupportEvidenceReport> getGeneratedEvidenceReport();

		static bool TryGetExplicitStatus(const std::wstring &mnemonic, std::shared_ptr<InstructionSupportStatus> &status);

		static std::shared_ptr<InstructionSupportStatus> GetStatus(const std::wstring &mnemonic);

		static bool TryGetStatus(Processor::CPU_Core::InstructionsEnum opcode, std::shared_ptr<InstructionSupportStatus> &status);

	private:
		static std::vector<std::shared_ptr<InstructionSupportStatus>> BuildExplicitStatuses();

		static void AddOptionalEnabledVectorConfigSystemSingleton(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorTransfer(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorBinaryCompute(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorSaturatingAddPolicy(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorUnaryCompute(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorFma(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorReductionScalarFootprint(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorDotProductScalarFootprint(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorDotProductWideScalarFootprint(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledPredicateMaskPublication(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorMaskPrefixPublication(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorZeroExtendPublication(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorScanPrefixPublication(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorComparisonPredicatePublication(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorPredicativeMovement(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorPermutation(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorPerm2(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorTranspose(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorSlide(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledVectorSlideOne(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalDisabledMatrix(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledMatrixProduction(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddCarrierOnlyL7Sdc(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledL7SdcProduction(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddReservedNoAllocation(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic, const std::wstring &extensionName);

		static void AddDescriptorOnlyNoAllocation(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic, const std::wstring &extensionName);

		static void AddOptionalEnabledScalarBitmanipCore(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledScalarBitfield(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledScalarAddressGeneration(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledScalarCarryLessChecksum(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static void AddOptionalEnabledScalarSelectCzero(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic);

		static const std::unordered_map<std::wstring, std::shared_ptr<InstructionSupportStatus>> BuildMnemonicIndex(const std::vector<std::shared_ptr<InstructionSupportStatus>> &statuses);

		static std::optional<OpcodeInfo> FindOpcodeInfoByMnemonic(const std::wstring &mnemonic);

		static std::wstring NormalizeMnemonic(const std::wstring &mnemonic);

		static inline const std::vector<std::wstring> s_mandatoryInteger64RepairMnemonics = {L"SRA", L"ADDIW", L"ADDW", L"SUBW", L"SLLW", L"SRLW", L"SRAW", L"SLLIW", L"SRLIW", L"SRAIW", L"MULW", L"DIVW", L"DIVUW", L"REMW", L"REMUW", L"SEXT.W", L"ZEXT.W"};

		static inline const std::vector<std::shared_ptr<InstructionSupportStatus>> s_explicitStatuses = BuildExplicitStatuses();

		static inline const std::unordered_map<std::wstring, std::shared_ptr<InstructionSupportStatus>> s_byMnemonic = BuildMnemonicIndex(s_explicitStatuses);

		static inline const std::shared_ptr<InstructionSupportEvidenceReport> s_generatedEvidenceReport = std::make_shared<InstructionSupportEvidenceReport>(s_explicitStatuses);
	};
}
