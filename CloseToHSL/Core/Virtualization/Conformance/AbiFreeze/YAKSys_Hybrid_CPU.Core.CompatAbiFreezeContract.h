#pragma once

#include "CompatAbiFreezeContract.h"
#include <memory>
#include <span>



namespace YAKSys_Hybrid_CPU::Core
{
	class CompatAbiFreezeContract final : public std::enable_shared_from_this<CompatAbiFreezeContract>
	{
	public:
		static constexpr unsigned short FrozenCoreOpcodeFirst = 250;
		static constexpr unsigned short FrozenCoreOpcodeLast = 257;
		static constexpr unsigned short FrozenPointerStoreOpcode = 258;
		static constexpr unsigned short FrozenCallOpcode = 259;
		static constexpr unsigned short FrozenExtensionOpcodeFirst = 267;
		static constexpr unsigned short FrozenExtensionOpcodeLast = 271;

		static std::span<const unsigned short> getFrozenCsrAliases();

		bool IsFrozenOpcode(unsigned short opcode);

		bool IsFrozenCsrAlias(unsigned short csrAddress);

		bool IsFrozenVmcsFieldAlias(unsigned short fieldId);

		CompatAbiFreezeViolation ValidateOpcode(unsigned short opcode);

		CompatAbiFreezeViolation ValidateCsrAlias(unsigned short csrAddress);

		CompatAbiFreezeViolation ValidateVmcsFieldAlias(unsigned short fieldId);
	};
}
