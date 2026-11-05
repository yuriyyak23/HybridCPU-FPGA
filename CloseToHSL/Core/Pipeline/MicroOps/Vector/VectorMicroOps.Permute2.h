#pragma once

#include "VectorMicroOps.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../Architecture/Registers/Retire/RetireRecord.h"
#include "../../../../../Arch/DataTypeEnum.h"
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{
	class VectorPermute2MicroOp final : public VectorMicroOp
	{
	public:
		class StagedVectorWrite;

	private:
		std::vector<StagedVectorWrite> _stagedWrites;
		int _stagedWriteCount = 0;

	public:
		void InitializeMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		void EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount) override;

	private:
		std::vector<unsigned char> ReadElement(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long baseAddress, unsigned long long lane, int elementSize);

		void PublishStagedWritesAtWriteBack(std::shared_ptr<Processor::CPU_Core> &core);

		void ThrowIfUnsupportedPublicationContour();

		void ThrowIfUnsupportedRuntimeContour();

		static void ThrowIfUnsupportedPermute2DataType(DataTypeEnum dataType, const std::wstring &surface);

		bool LaneMayOverwrite(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long lane);

	public:
		std::vector<StagedVectorWrite> GetStagedWrites();

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct StagedVectorWrite
		class StagedVectorWrite
		{
		private:
			const unsigned long long Lane;
			const unsigned long long Address;
			const std::vector<unsigned char> Data;

		public:
			StagedVectorWrite(unsigned long long lane, unsigned long long address, std::vector<unsigned char> &data);

			unsigned long long getLane() const;
			unsigned long long getAddress() const;
			std::vector<unsigned char> getData() const;

			StagedVectorWrite() = default;
		};

	protected:
		std::shared_ptr<VectorPermute2MicroOp> shared_from_this()
		{
			return std::static_pointer_cast<VectorPermute2MicroOp>(VectorMicroOp::shared_from_this());
		}
	};
}
