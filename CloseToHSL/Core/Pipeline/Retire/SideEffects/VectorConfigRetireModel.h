#pragma once

namespace YAKSys_Hybrid_CPU::Core
{
	enum class VectorConfigOperationKind : unsigned char
	{
		None = 0,
		Vsetvl = 1,
		Vsetvli = 2,
		Vsetivli = 3
	};

	/// <summary>
	/// Deferred retire payload for vector-config lane-7 carriers.
	/// Keeps VL/VTYPE architectural publication and optional rd writeback coupled on the
	/// authoritative WB path instead of mutating VectorConfig during Execute().
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct VectorConfigRetireEffect
	class VectorConfigRetireEffect
	{
	private:
		const VectorConfigOperationKind Operation;
		const unsigned long long ActualVectorLength;
		const unsigned long long VType;
		const unsigned char TailAgnostic;
		const unsigned char MaskAgnostic;
		const bool HasRegisterWriteback;
		const unsigned short DestinationRegister;

		VectorConfigRetireEffect(VectorConfigOperationKind operation, unsigned long long actualVectorLength, unsigned long long vtype, unsigned char tailAgnostic, unsigned char maskAgnostic, bool hasRegisterWriteback, unsigned short destinationRegister);

	public:
		VectorConfigOperationKind getOperation() const;

		unsigned long long getActualVectorLength() const;

		unsigned long long getVType() const;

		unsigned char getTailAgnostic() const;

		unsigned char getMaskAgnostic() const;

		bool getHasRegisterWriteback() const;

		unsigned short getDestinationRegister() const;

		bool isValid() const;

		static VectorConfigRetireEffect Create(VectorConfigOperationKind operation, unsigned long long actualVectorLength, unsigned long long vtype, unsigned char tailAgnostic, unsigned char maskAgnostic, bool hasRegisterWriteback, unsigned short destinationRegister);

		VectorConfigRetireEffect() = default;
	};
}
