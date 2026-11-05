#include "VectorConfigRetireModel.h"

namespace YAKSys_Hybrid_CPU::Core
{

	VectorConfigRetireEffect::VectorConfigRetireEffect(VectorConfigOperationKind operation, unsigned long long actualVectorLength, unsigned long long vtype, unsigned char tailAgnostic, unsigned char maskAgnostic, bool hasRegisterWriteback, unsigned short destinationRegister)
		: Operation(operation), ActualVectorLength(actualVectorLength), VType(vtype),
		  TailAgnostic(tailAgnostic), MaskAgnostic(maskAgnostic),
		  HasRegisterWriteback(hasRegisterWriteback), DestinationRegister(destinationRegister)
	{
	}

	VectorConfigOperationKind VectorConfigRetireEffect::getOperation() const
	{
		return Operation;
	}

	unsigned long long VectorConfigRetireEffect::getActualVectorLength() const
	{
		return ActualVectorLength;
	}

	unsigned long long VectorConfigRetireEffect::getVType() const
	{
		return VType;
	}

	unsigned char VectorConfigRetireEffect::getTailAgnostic() const
	{
		return TailAgnostic;
	}

	unsigned char VectorConfigRetireEffect::getMaskAgnostic() const
	{
		return MaskAgnostic;
	}

	bool VectorConfigRetireEffect::getHasRegisterWriteback() const
	{
		return HasRegisterWriteback;
	}

	unsigned short VectorConfigRetireEffect::getDestinationRegister() const
	{
		return DestinationRegister;
	}

	bool VectorConfigRetireEffect::isValid() const
	{
		return getOperation() != VectorConfigOperationKind::None;
	}

	VectorConfigRetireEffect VectorConfigRetireEffect::Create(VectorConfigOperationKind operation, unsigned long long actualVectorLength, unsigned long long vtype, unsigned char tailAgnostic, unsigned char maskAgnostic, bool hasRegisterWriteback, unsigned short destinationRegister)
	{
		return VectorConfigRetireEffect(operation, actualVectorLength, vtype, tailAgnostic, maskAgnostic, hasRegisterWriteback, destinationRegister);
	}
}
