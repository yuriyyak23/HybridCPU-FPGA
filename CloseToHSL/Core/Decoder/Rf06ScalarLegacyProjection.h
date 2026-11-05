#pragma once

#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "Rf06ExecutionContracts.h"
#include "../Pipeline/Safety/ResourceBitset.h"
#include "../Pipeline/MicroOps/Vector/MicroOp.Compute.h"
#include <string>
#include <vector>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>
#include <string>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core::Decoder { class CheckedScalarLegacyProjection; }

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	/// <summary>
	/// RF-06.2 checked projection for the first non-serial scalar ALU family.
	///
	/// This is an additive legacy-carrier adapter. It does not route, admit, issue,
	/// retire, or create replay state. The provider identity is consumed from the
	/// already frozen generated binding and is never reconstructed from an opcode.
	/// </summary>
	class Rf06ScalarLegacyProjection final
	{
	public:
		static inline const std::wstring MaterializerIdentity = L"legacy.materializer.scalar-alu";
		static inline const std::wstring ProviderIdentity = L"legacy.provider.scalar-alu";
		static inline const std::wstring LatencyIdentity = L"legacy-latency-1";

		static bool IsSupportedOpcode(unsigned int opcode);

		/// <summary>
		/// Builds the immutable RF-06 capability envelope from the one generated
		/// binding carried by canonical decode. No registry or opcode lookup occurs.
		/// </summary>
		static std::shared_ptr<ExecutionContract> CreateContract(std::shared_ptr<CanonicalDecodedInstruction> canonical);

		/// <summary>
		/// Projects a checked legacy scalar carrier from an already resolved
		/// immutable contract. The returned guard must be checked before a future
		/// consumer uses the mutable carrier after any legacy handoff.
		/// </summary>
		static std::shared_ptr<CheckedScalarLegacyProjection> Project(std::shared_ptr<CanonicalDecodedInstruction> canonical, std::shared_ptr<ExecutionContract> contract);

	private:
		static ResourceBitset BuildRegisterResourceMask(std::shared_ptr<CanonicalDecodedInstruction> canonical);

		static void ValidateCanonicalFamily(std::shared_ptr<CanonicalDecodedInstruction> canonical, std::shared_ptr<GeneratedStaticBinding> &binding);

		static bool IsPresentArchitecturalRegister(unsigned char rawValue);
	};

	/// <summary>
	/// Mutable legacy carrier plus an immutable fingerprint of the facts that made
	/// the projection valid. RF-07 outcome/completion state is intentionally absent.
	/// </summary>
		struct ScalarLegacyProjectionFingerprint final
	{
		std::wstring Value;
		static std::shared_ptr<ScalarLegacyProjectionFingerprint> Capture(std::shared_ptr<ExecutionContract> contract, std::shared_ptr<ScalarALUMicroOp> carrier);
		static void EnsureCurrent(std::shared_ptr<ScalarLegacyProjectionFingerprint> expected, std::shared_ptr<ExecutionContract> contract, std::shared_ptr<ScalarALUMicroOp> carrier);
	};
class CheckedScalarLegacyProjection final : public std::enable_shared_from_this<CheckedScalarLegacyProjection>
	{
	private:
		const std::shared_ptr<CanonicalDecodedInstruction> CanonicalInstruction;
		const std::shared_ptr<ExecutionContract> Contract;
		const std::shared_ptr<ScalarALUMicroOp> Carrier;

		const std::shared_ptr<ScalarLegacyProjectionFingerprint> _fingerprint;

	public:
		CheckedScalarLegacyProjection(std::shared_ptr<CanonicalDecodedInstruction> canonical, std::shared_ptr<ExecutionContract> contract, std::shared_ptr<ScalarALUMicroOp> carrier);

		std::shared_ptr<CanonicalDecodedInstruction> getCanonicalInstruction() const;
		std::shared_ptr<ExecutionContract> getContract() const;
		std::shared_ptr<ScalarALUMicroOp> getCarrier() const;

		void EnsureCurrent();
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct ScalarLegacyProjectionFingerprint(string Value)
//	{
//		internal static ScalarLegacyProjectionFingerprint Capture(ExecutionContract contract, ScalarALUMicroOp carrier)
//		{
//			ArgumentNullException.ThrowIfNull(contract);
//			ArgumentNullException.ThrowIfNull(carrier);
//
//			string canonical = string.Join("|", contract.GeneratedBinding.Opcode, contract.GeneratedBinding.MaterializerId.Value, contract.GeneratedBinding.RuntimeExecutionProviderId.Value, contract.GeneratedBinding.LatencyModelId.Value, contract.GeneratedBinding.DescriptorFingerprint, contract.RuntimeProvider.Id.Value, contract.RuntimeProvider.PayloadSchema, contract.InstructionClass, contract.SerializationClass, contract.Placement.RequiredSlotClass, contract.Placement.PinningKind, contract.Placement.PinnedLaneId, contract.Placement.DomainTag, contract.StaticEffectContract, contract.Memory.Kind, contract.StaticMemoryPlan != nullptr, string.Join(",", contract.ReadRegisters), string.Join(",", contract.WriteRegisters), contract.ResourceMask.Low, contract.ResourceMask.High, carrier.OpCode, carrier.DestRegID, carrier.Src1RegID, carrier.Src2RegID, carrier.Immediate, carrier.UsesImmediate, carrier.WritesRegister, carrier.IsMemoryOp, carrier.Latency, carrier.InstructionClass, carrier.SerializationClass, string.Join(",", carrier.ReadRegisters), string.Join(",", carrier.WriteRegisters), carrier.ResourceMask.Low, carrier.ResourceMask.High);
//
//			return new ScalarLegacyProjectionFingerprint(Convert.ToHexString(SHA256.HashData(Encoding.UTF8.GetBytes(canonical))).ToLowerInvariant());
//		}
//
//		internal static void EnsureCurrent(ScalarLegacyProjectionFingerprint expected, ExecutionContract contract, ScalarALUMicroOp carrier)
//		{
//			ScalarLegacyProjectionFingerprint actual = Capture(contract, carrier);
//			if (actual != expected)
//			{
//				throw new InvalidOperationException("RF-06.2 scalar legacy projection is stale after carrier mutation.");
//			}
//		}
//	}

}
