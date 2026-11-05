#pragma once

#include "../../../Execution/StreamEngine/AddressGeneration/StreamEngine.AddressGen.h"
#include "VectorMicroOps.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../../Memory/Timing/MemoryCycleController.h"
#include "VectorTransferRetireEffect.h"
#include "../Types/MicroOp.h"
#include "../../Retire/SideEffects/VectorConfigRetireModel.h"
#include "../../../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <any>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

using namespace HybridCPU_ISE::Arch;
using namespace HybridCPU_ISE::CloseToHSL::Memory::Timing;

using namespace YAKSys_Hybrid_CPU::Execution;
using namespace YAKSys_Hybrid_CPU::Core::Registers;


namespace YAKSys_Hybrid_CPU::Core
{
	class VectorPermutationMicroOp : public VectorMicroOp
	{
		/// <summary>
		/// Initialize FSP metadata for permutation vector operations.
		/// Permutation operations read from source vector and index vector.
		/// </summary>
	public:
		void InitializeMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

	private:
		void ThrowIfUnsupportedAddressingContour();

		void LoadAllElements(std::shared_ptr<Processor::CPU_Core> &core);

		void ComputePermutation(std::shared_ptr<Processor::CPU_Core> &core);

		void StoreAllElements(std::shared_ptr<Processor::CPU_Core> &core);

	protected:
		std::shared_ptr<VectorPermutationMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<VectorPermutationMicroOp>(VectorMicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Vector slide operation micro-operation (shift elements up/down)
	/// Examples: VSLIDEUP, VSLIDEDOWN
	///
	/// Slide operations shift vector elements by a constant offset.
	/// </summary>
	class VectorSlideMicroOp : public VectorMicroOp
	{
		/// <summary>
		/// Initialize FSP metadata for slide vector operations.
		/// Slide operations read from source vector and write shifted result.
		/// </summary>
	public:
		void InitializeMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

	private:
		void ThrowIfUnsupportedAddressingContour();

		void LoadAllElements(std::shared_ptr<Processor::CPU_Core> &core);

		void ComputeSlide(std::shared_ptr<Processor::CPU_Core> &core);

		void StoreAllElements(std::shared_ptr<Processor::CPU_Core> &core);

	protected:
		std::shared_ptr<VectorSlideMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<VectorSlideMicroOp>(VectorMicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Vector predicative movement micro-operation.
	/// Examples: VCOMPRESS, VEXPAND.
	///
	/// These opcodes are not generic two-source binary math. The authoritative mainline
	/// path must not silently collapse them into VectorBinaryOpMicroOp default-zero
	/// behavior while their dedicated follow-through remains unresolved.
	/// </summary>
	class VectorPredicativeMovementMicroOp : public VectorMicroOp
	{
	public:
		void InitializeMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

	private:
		void ThrowIfUnsupportedAddressingContour();

		void ThrowIfUnsupportedBufferFootprintContour(std::shared_ptr<Processor::CPU_Core> &core);

		void LoadAllElements(std::shared_ptr<Processor::CPU_Core> &core);

		void ComputePredicativeMovement(std::shared_ptr<Processor::CPU_Core> &core);

		void StoreAllElements(std::shared_ptr<Processor::CPU_Core> &core);

	protected:
		std::shared_ptr<VectorPredicativeMovementMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<VectorPredicativeMovementMicroOp>(VectorMicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Vector dot product operation micro-operation (two vectors -> scalar)
	/// Examples: VDOT, VDOTU, VDOTF
	///
	/// Dot product computes sum(vs1[i] * vs2[i]) for all active elements.
	/// Result is a scalar written to element 0 of destination.
	/// </summary>
	class VectorDotProductMicroOp : public VectorMicroOp
	{
		/// <summary>
		/// Initialize FSP metadata for dot product vector operations.
		/// Dot product reads from two vectors and writes one scalar result.
		/// For widening operations (VDOT_E4M3, VDOT_E5M2): reads 1-byte elements, writes 4-byte result.
		/// Includes alignment checks for widening operations.
		/// </summary>
	public:
		void InitializeMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

	private:
		void ThrowIfUnsupportedAddressingContour();

		void LoadAllElements(std::shared_ptr<Processor::CPU_Core> &core);

		void ComputeDotProduct(std::shared_ptr<Processor::CPU_Core> &core);

		void StoreScalarResult(std::shared_ptr<Processor::CPU_Core> &core);

		int GetDotProductElementSize();

		void ThrowIfUnsupportedDotProductElementDataType();

		void ThrowIfUnsupportedDotProductElementDataTypeForMetadata();

	protected:
		std::shared_ptr<VectorDotProductMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<VectorDotProductMicroOp>(VectorMicroOp::shared_from_this());
		}
	};

	// ===== Phase 6: New Stream/Vector Load/Store MicroOps =====
	// These classes implement the gradual migration of StreamEngine logic into micro-operations

	/// <summary>
	/// Compatibility micro-operation for legacy vector transfer opcodes that still execute
	/// through the StreamEngine but now must carry explicit WB retire authority.
	/// </summary>
	class VectorTransferMicroOp final : public VectorMicroOp
	{
	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private MemoryCycleController? _requestController;
		std::shared_ptr<MemoryCycleController> _requestController;
		std::optional<std::shared_ptr<MemoryRequestId>> _controllerRequestId;
		bool _controllerAdmissionBackpressured = false;
		bool _completedThroughController = false;
		std::shared_ptr<std::vector<unsigned char>> _transferBuffer;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private VectorTransferRetireEffect? _retireEffect;
		std::shared_ptr<VectorTransferRetireEffect> _retireEffect;
		unsigned int _acceptedOpcode = 0;
		unsigned long long _acceptedSourceAddress = 0;
		unsigned long long _acceptedDestinationAddress = 0;
		unsigned long long _acceptedElementCount = 0;
		int _acceptedElementSize = 0;
		unsigned short _acceptedStride = 0;

	public:
		bool getOwnsPendingMemoryCompletion() const;

		bool getHasControllerAdmissionBackpressure() const;

		bool getHasControllerRetireEffect() const;

		VectorTransferMicroOp();

		void InitializeMetadata() override;

	private:
		void PublishTransferClassification();

	public:
		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		std::shared_ptr<VectorTransferRetireEffect> CreateRetireEffect();

		bool CancelPendingControllerRequest();

	private:
		void ClearControllerRequestState(bool resetExecutionState);

		void ThrowIfUnsupportedAddressingContour();

		unsigned long long ResolveTransferStride(int elemSize);

		unsigned long long ResolveTransferByteCount(int elemSize);

		unsigned long long ResolveTransferTotalSize(int elemSize);

		void ResolveTransferEndpoints(unsigned long long &readPointer, unsigned long long &writePointer);

	protected:
		std::shared_ptr<VectorTransferMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<VectorTransferMicroOp>(VectorMicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Load segment micro-operation for 1D vector loads.
	/// Initiates BurstIO.Read and manages async memory request tokens.
	///
	/// Phase 6 Design:
	/// - Separates load operation from computation
	/// - Enables scheduler to insert other operations during memory wait
	/// - Uses async MemorySubsystem for burst transfers

	class VConfigMicroOp : public MicroOp
	{
	private:
		VectorConfigOperationKind OperationKind = static_cast<VectorConfigOperationKind>(0);
		unsigned short SrcReg1ID = VLIW_Instruction::NoReg;
		unsigned short SrcReg2ID = VLIW_Instruction::NoReg;
		unsigned long long EncodedVTypeImmediate = 0;
		unsigned long long EncodedAvlImmediate = 0;

		VectorConfigRetireEffect _resolvedRetireEffect;
		unsigned long long _actualVectorLength = 0;

	public:
		VectorConfigOperationKind getOperationKind() const;
	private:
		void setOperationKind(VectorConfigOperationKind value);
	public:
		unsigned short getSrcReg1ID() const;
	private:
		void setSrcReg1ID(unsigned short value);
	public:
		unsigned short getSrcReg2ID() const;
	private:
		void setSrcReg2ID(unsigned short value);
	public:
		unsigned long long getEncodedVTypeImmediate() const;
	private:
		void setEncodedVTypeImmediate(unsigned long long value);
	public:
		unsigned long long getEncodedAvlImmediate() const;
	private:
		void setEncodedAvlImmediate(unsigned long long value);

	public:
		VConfigMicroOp();

		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		void ConfigureForRegisterVType(unsigned char rs1, unsigned char rs2);

		void ConfigureForImmediateVType(VectorConfigOperationKind operationKind, unsigned char rs1, unsigned long long encodedVTypeImmediate);

		void ConfigureForImmediateAvlAndVType(unsigned long long encodedAvlImmediate, unsigned long long encodedVTypeImmediate);

		void InitializeMetadata();

		void RefreshWriteMetadata() override;

		VectorConfigRetireEffect CreateRetireEffect();

		bool TryGetPrimaryWriteBackResult(unsigned long long &value) override;

		void CapturePrimaryWriteBackResult(unsigned long long value) override;

		std::wstring GetDescription() override;

	private:
		static std::vector<int> BuildReadRegisterList(std::vector<unsigned short> &rawRegisters);

		bool HasArchitecturalDestinationRegister();

		unsigned long long ResolveApplicationVectorLength(std::shared_ptr<Processor::CPU_Core> &core, int vtId);

		unsigned long long ResolveVType(std::shared_ptr<Processor::CPU_Core> &core, int vtId);

		unsigned long long ResolveDescriptionVType();

	protected:
		std::shared_ptr<VConfigMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<VConfigMicroOp>(MicroOp::shared_from_this());
		}
	};
}
