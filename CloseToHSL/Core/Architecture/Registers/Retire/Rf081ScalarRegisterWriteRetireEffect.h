#pragma once

#include "RetireRecord.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Registers::Retire
{
	class RetireRecordIdentityProjection;
	class RetireVisibleEffectIdentity;

	/// <summary>
	/// Immutable RF-08.1 carrier for exactly one scalar architectural-register write.
	/// It binds the legacy payload to the already-frozen issued-attempt identity and
	/// deliberately owns no retire selection, prevalidation, or publication operation.
	/// </summary>
	class ScalarRegisterWriteRetireEffect final : public std::enable_shared_from_this<ScalarRegisterWriteRetireEffect>
	{
	private:
		const std::shared_ptr<RetireRecordIdentityProjection> Projection;

		ScalarRegisterWriteRetireEffect(std::shared_ptr<RetireRecordIdentityProjection> projection);

	public:
		std::shared_ptr<RetireRecordIdentityProjection> getProjection() const;
		YAKSys_Hybrid_CPU::Core::Registers::Retire::RetireRecord getRetireRecord() const;
		std::shared_ptr<RetireVisibleEffectIdentity> getIdentity() const;
		int getVirtualThreadId() const;
		int getArchitecturalRegisterId() const;
		unsigned long long getValue() const;

		static std::shared_ptr<ScalarRegisterWriteRetireEffect> Freeze(YAKSys_Hybrid_CPU::Core::Registers::Retire::RetireRecord const retireRecord, std::shared_ptr<RetireVisibleEffectIdentity> identity);
	};

}
