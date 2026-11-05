#pragma once

#include "../../../Architecture/Registers/Retire/RetireCoordinator.h"
#include "../../../Architecture/Registers/Retire/RetireRecord.h"
#include <cmath>
#include <stdexcept>
#include <format>
#include <memory>
#include <span>

using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core::Pipeline
{
	/// <summary>
	 /// CommitUnit emits typed retire records in program order.
	 /// Architectural state is updated by <see cref="RetireCoordinator"/>.
	 /// </summary>
	class CommitUnit final : public std::enable_shared_from_this<CommitUnit>
	{
	private:
		const std::shared_ptr<RetireCoordinator> _retireCoordinator;

		/// <summary>
		/// Authoritative phase-04 factory for production commit flow.
		/// Architectural retirement is driven directly from the unified retire
		/// coordinator rather than from compatibility VT-bank shells.
		/// </summary>
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: private CommitUnit(RetireCoordinator retireCoordinator)
		CommitUnit(std::shared_ptr<RetireCoordinator> retireCoordinator);

	public:
		static std::shared_ptr<CommitUnit> FromRetireCoordinator(std::shared_ptr<RetireCoordinator> retireCoordinator);

		void Commit(std::span<const RetireRecord> records, unsigned char vtId);

	private:
		void ValidateVirtualThreadId(unsigned char vtId);
	};
}
