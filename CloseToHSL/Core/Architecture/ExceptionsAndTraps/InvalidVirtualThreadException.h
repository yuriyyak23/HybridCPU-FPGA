#pragma once

#include "../../Execution/ExecutionFaultContract.h"
#include "../Registers/Architectural/RegisterIdentity.h"
#include <string>
#include <stdexcept>
#include <format>
#include <memory>
#include "../../../../tangible_string_helper.h"

using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Thrown when live architectural state is addressed with a VT identifier outside
	/// the canonical 4-way SMT ownership range.
	/// </summary>
	class InvalidVirtualThreadException final : public std::runtime_error, public std::enable_shared_from_this<InvalidVirtualThreadException>
	{
	private:
		const std::wstring Operation;
		const int VtId;
		const int VtCount;

	public:
		std::wstring getOperation() const;
		int getVtId() const;
		int getVtCount() const;
		ExecutionFaultCategory getCategory() const;

		InvalidVirtualThreadException(const std::wstring &operation, int vtId, int vtCount);

	private:
		static std::wstring FormatDetail(const std::wstring &operation, int vtId, int vtCount);

		static std::wstring NormalizeOperation(const std::wstring &operation);
	};

	class VirtualThreadIdResolver final
	{
	public:
		static int ResolveOrThrow(int vtId, int vtCount, const std::wstring &operation);
	};
}
