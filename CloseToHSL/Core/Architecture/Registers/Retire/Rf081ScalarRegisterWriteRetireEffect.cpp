#include "Rf081ScalarRegisterWriteRetireEffect.h"
#include "Rf08RetireEffectIdentityContracts.h"

namespace YAKSys_Hybrid_CPU::Core::Registers::Retire
{

	ScalarRegisterWriteRetireEffect::ScalarRegisterWriteRetireEffect(std::shared_ptr<RetireRecordIdentityProjection> projection)
		: Projection(std::move(projection))
	{
	}

	std::shared_ptr<RetireRecordIdentityProjection> ScalarRegisterWriteRetireEffect::getProjection() const
	{
		return Projection;
	}

	YAKSys_Hybrid_CPU::Core::Registers::Retire::RetireRecord ScalarRegisterWriteRetireEffect::getRetireRecord() const
	{
		return getProjection()->getRetireRecord();
	}

	std::shared_ptr<RetireVisibleEffectIdentity> ScalarRegisterWriteRetireEffect::getIdentity() const
	{
		return getProjection()->getIdentity();
	}

	int ScalarRegisterWriteRetireEffect::getVirtualThreadId() const
	{
		return getIdentity()->getVirtualThreadId();
	}

	int ScalarRegisterWriteRetireEffect::getArchitecturalRegisterId() const
	{
		return getRetireRecord().getArchReg();
	}

	unsigned long long ScalarRegisterWriteRetireEffect::getValue() const
	{
		return getRetireRecord().getValue();
	}

	std::shared_ptr<ScalarRegisterWriteRetireEffect> ScalarRegisterWriteRetireEffect::Freeze(YAKSys_Hybrid_CPU::Core::Registers::Retire::RetireRecord const retireRecord, std::shared_ptr<RetireVisibleEffectIdentity> identity)
	{
		ArgumentNullException::ThrowIfNull(identity);
		if (retireRecord.getKind() != RetireRecordKind::RegisterWrite || identity->getEffectKind() != RetireVisibleEffectKind::RegisterWrite)
		{
			throw RetireEffectIdentityContractViolationException(L"Scalar RegisterWrite effect requires matching RegisterWrite payload and identity kinds.");
		}

		std::shared_ptr<RetireRecordIdentityProjection> projection = RetireRecordIdentityProjection::Create(retireRecord, identity);
		return std::shared_ptr<ScalarRegisterWriteRetireEffect>(new ScalarRegisterWriteRetireEffect(std::move(projection)));
	}
}
